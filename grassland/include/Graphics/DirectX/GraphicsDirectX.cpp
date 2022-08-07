#include "GraphicsDirectX.h"

namespace Grassland {
HRESULT GRLDirectXErrorReport(HRESULT hr,
                              const char *code,
                              const char *file,
                              int line) {
  if (FAILED(hr)) {
    std::cout << "[ComCall Error!] code: " << std::hex << hr << std::endl
              << '(' << file << ":" << line << ')' << code << std::endl;
  }
  return hr;
}

GRLCD3D12PipelineState::GRLCD3D12PipelineState(
    GRLCD3D12Environment *pEnvironment,
    const char *shader_path,
    GRL_GRAPHICS_PIPELINE_STATE_DESC *desc) {
  __Ref_Cnt = 1;
  m_numConstantBuffer = desc->numConstantBuffer;
  m_numTexture = desc->numTexture;
  m_vertexStride = 0;
  ComPtr<ID3D12Device> device(pEnvironment->GetDevice());

  /***************************************
   * Create Shaders
   ****************************************/
  ComPtr<ID3DBlob> vertexShader;
  ComPtr<ID3DBlob> pixelShader;
  ComPtr<ID3DBlob> shaderErrorMsg;

#if defined(_DEBUG)
  // Enable better shader debugging with the graphics debugging tools.
  UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
  UINT compileFlags = 0;
#endif
  std::wstring wshader_path =
      GRLStringUTF8toUnicode(shader_path) + L"/d3d12_shaders.hlsl";

  if (FAILED(GRLComCall(D3DCompileFromFile(
          wshader_path.c_str(), nullptr, nullptr, "VSMain", "vs_5_0",
          compileFlags, 0, &vertexShader, &shaderErrorMsg)))) {
    std::cout << "[Vertex Shader Compilation Error]  Error Message: \n"
              << (char *)shaderErrorMsg->GetBufferPointer() << std::endl;
    return;
  }

  if (FAILED(GRLComCall(D3DCompileFromFile(
          wshader_path.c_str(), nullptr, nullptr, "PSMain", "ps_5_0",
          compileFlags, 0, &pixelShader, &shaderErrorMsg)))) {
    std::cout << "[Pixel Shader Compilation Error]  Error Message: \n"
              << (char *)shaderErrorMsg->GetBufferPointer() << std::endl;
    return;
  }

  /***************************************
   * Create RootSignature
   ****************************************/
  {
    D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

    featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

    if (FAILED(GRLComCall(
            device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE,
                                        &featureData, sizeof(featureData))))) {
      featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
    }

    CD3DX12_DESCRIPTOR_RANGE1 *ranges = nullptr;

    if (desc->numTexture) {
      ranges = new CD3DX12_DESCRIPTOR_RANGE1[desc->numTexture];
      for (int index = 0; index < desc->numTexture; index++)
        ranges[index].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, index, 0,
                           D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
    }

    CD3DX12_ROOT_PARAMETER1 *rootParameters = nullptr;
    if (desc->numTexture + desc->numConstantBuffer)
      rootParameters = new CD3DX12_ROOT_PARAMETER1[desc->numTexture +
                                                   desc->numConstantBuffer];
    for (int index = 0; index < desc->numTexture; index++)
      rootParameters[index].InitAsDescriptorTable(
          1, &ranges[index], D3D12_SHADER_VISIBILITY_PIXEL);
    for (int index = 0; index < desc->numConstantBuffer; index++)
      rootParameters[index + desc->numTexture].InitAsConstantBufferView(index);

    D3D12_STATIC_SAMPLER_DESC
    sampler = {}, *samplers = new D3D12_STATIC_SAMPLER_DESC[desc->numTexture];

    for (int i = 0; i < desc->numTexture; i++) {
      sampler.Filter = GRLFilterToD3D12Filter(desc->samplerDesc[i].filter);
      sampler.AddressU = GRLExtensionModeToD3D12TextureAddressMode(
          desc->samplerDesc[i].extensionU);
      sampler.AddressV = GRLExtensionModeToD3D12TextureAddressMode(
          desc->samplerDesc[i].extensionV);
      sampler.AddressW = GRLExtensionModeToD3D12TextureAddressMode(
          desc->samplerDesc[i].extensionW);
      sampler.MipLODBias = 0;
      sampler.MaxAnisotropy = 0;
      sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
      sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
      sampler.MinLOD = 0.0f;
      sampler.MaxLOD = D3D12_FLOAT32_MAX;
      sampler.ShaderRegister = i;
      sampler.RegisterSpace = 0;
      sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
      samplers[i] = sampler;
    }

    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.Init_1_1(
        desc->numTexture + desc->numConstantBuffer, rootParameters,
        desc->numTexture, desc->numTexture ? (samplers) : nullptr,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    if (FAILED(GRLComCall(D3DX12SerializeVersionedRootSignature(
            &rootSignatureDesc, featureData.HighestVersion, &signature,
            &error)))) {
      std::cout << "[Create Root Signature ERROR] : \n"
                << (char *)error->GetBufferPointer() << std::endl;
    }
    GRLComCall(device->CreateRootSignature(0, signature->GetBufferPointer(),
                                           signature->GetBufferSize(),
                                           IID_PPV_ARGS(&m_rootSignature)));

    if (ranges)
      delete[] ranges;
    if (rootParameters)
      delete[] rootParameters;
    if (samplers)
      delete[] samplers;
  }

  /***************************************
   * Create PipelineState
   ****************************************/
  {
    D3D12_INPUT_ELEMENT_DESC *inputElementDescs =
        new D3D12_INPUT_ELEMENT_DESC[desc->numInputElement];

    for (uint32_t index = 0, offset = 0; index < desc->numInputElement;
         index++) {
      inputElementDescs[index] = D3D12_INPUT_ELEMENT_DESC(
          {"DATA", index,
           GRLFormatToDXGIFormat(desc->inputElementLayout[index]), 0, offset,
           D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
      offset += GRLFormatSizeInBytes(desc->inputElementLayout[index]);
      m_vertexStride = offset;
    }
    /* =
    {
            { "DATA", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,
    D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, { "DATA", 1,
    DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16,
    D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, { "DATA", 2,
    DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32,
    D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };*/

    D3D12_BLEND_DESC blendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

    if (desc->enableBlend) {
      blendDesc.AlphaToCoverageEnable = FALSE;
      blendDesc.IndependentBlendEnable = FALSE;
      blendDesc.RenderTarget[0].BlendEnable = TRUE;
      blendDesc.RenderTarget[0].LogicOpEnable = FALSE;
      blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
      blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
      blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
      blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
      blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
      blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
    }

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = {inputElementDescs, desc->numInputElement};
    psoDesc.pRootSignature = m_rootSignature.Get();
    psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
    psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    if (!desc->cullFace)
      psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
    else if (desc->cullFace == 1)
      psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
    else if (desc->cullFace == -1)
      psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
    psoDesc.BlendState = blendDesc;  // CD3DX12_BLEND_DESC(D3D12_DEFAULT);

    psoDesc.DepthStencilState.DepthEnable = FALSE;
    psoDesc.DepthStencilState.StencilEnable = FALSE;
    if (desc->enableDepthTest) {
      psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
      psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    }
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = desc->numRenderTargets;
    for (int index = 0; index < desc->numRenderTargets; index++)
      psoDesc.RTVFormats[index] =
          GRLFormatToDXGIFormat(desc->renderTargetFormatsList[index]);
    psoDesc.SampleDesc.Count = 1;

    GRLComCall(device->CreateGraphicsPipelineState(
        &psoDesc, IID_PPV_ARGS(&m_pipelineState)));
  }
}
GRL_RESULT GRLCD3D12PipelineState::AddRef() {
  __Ref_Cnt++;
  return GRL_FALSE;
}
GRL_RESULT GRLCD3D12PipelineState::Release() {
  __Ref_Cnt--;
  if (!__Ref_Cnt)
    delete this;
  return GRL_FALSE;
}
GRL_RESULT GRLCD3D12PipelineState::QueryInterface(GRLUUID Uuid,
                                                  void **ppObject) {
  if (Uuid == GRLID_IGraphicsPipelineState)
    *ppObject = (GRLIGraphicsPipelineState *)this;
  else if (Uuid == GRLID_CD3D12PipelineState)
    *ppObject = (GRLCD3D12PipelineState *)this;
  else if (Uuid == GRLID_IBase)
    *ppObject = (GRLIBase *)this;
  else
    return GRL_TRUE;
  AddRef();
  return GRL_FALSE;
}
ID3D12RootSignature *GRLCD3D12PipelineState::GetRootSignature() {
  return m_rootSignature.Get();
}
ID3D12PipelineState *GRLCD3D12PipelineState::GetPipelineState() {
  return m_pipelineState.Get();
}
uint32_t GRLCD3D12PipelineState::GetTextureRootParameterIndex(
    uint32_t textureIndex) {
  return textureIndex;
}
uint32_t GRLCD3D12PipelineState::GetConstantBufferRootParameterIndex(
    uint32_t constantBufferIndex) {
  return constantBufferIndex + m_numTexture;
}
GRL_RESULT GRLCD3D12Environment::SetViewport(uint32_t x,
                                             uint32_t y,
                                             uint32_t width,
                                             uint32_t height) {
  if (!m_duringDraw)
    return GRL_TRUE;
  CD3DX12_VIEWPORT viewPort((float)x, (float)y, (float)width, (float)height);
  CD3DX12_RECT scissorRect((LONG)x, (LONG)y, (LONG)width, (LONG)height);
  m_commandList->RSSetViewports(1, &viewPort);
  m_commandList->RSSetScissorRects(1, &scissorRect);
  return GRL_FALSE;
}
GRL_RESULT GRLCD3D12Environment::SetConstantBuffer(
    uint32_t constantBufferIndex,
    GRLIGraphicsBuffer *constantBuffer) {
  if (!m_workingPipelineState.Get())
    return GRL_TRUE;
  GRLPtr<GRLCD3D12Buffer> pConstantBuffer;
  if (constantBuffer->QueryInterface(GRLID_PPV_ARGS(&pConstantBuffer)))
    return GRL_TRUE;
  m_commandList->SetGraphicsRootConstantBufferView(
      m_workingPipelineState->GetConstantBufferRootParameterIndex(
          constantBufferIndex),
      pConstantBuffer->GetResource()->GetGPUVirtualAddress());
  return GRL_FALSE;
}

GRL_RESULT GRLCD3D12Environment::SetTextures(
    uint32_t numTexture,
    GRLIGraphicsTexture *const *pTextures) {
  if (!m_workingPipelineState.Get())
    return GRL_TRUE;
  if (numTexture > m_workingPipelineState->m_numTexture)
    return GRL_TRUE;
  if (!m_duringDraw)
    return GRL_TRUE;

  GRLPtr<GRLCD3D12Texture> *textures = new GRLPtr<GRLCD3D12Texture>[numTexture];

  for (int index = 0; index < numTexture; index++) {
    if (pTextures[index]->QueryInterface(GRLID_PPV_ARGS(&textures[index]))) {
      delete[] textures;
      return GRL_TRUE;
    }
  }
  for (int index = 0; index < numTexture; index++) {
    textures[index]->ResourceStateTransition(
        m_commandList.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    m_commandList->SetGraphicsRootDescriptorTable(
        m_workingPipelineState->GetTextureRootParameterIndex(index),
        textures[index]->GetSRVHandle());
  }

  delete[] textures;
  return GRL_FALSE;
}

GRL_RESULT GRLCD3D12Environment::SetRenderTargets(
    uint32_t numRenderTarget,
    GRLIGraphicsTexture *const *pRenderTargets,
    GRLIGraphicsDepthMap *pDepthMap) {
  if (numRenderTarget > 8)
    return GRL_TRUE;
  if (!m_duringDraw)
    return GRL_TRUE;
  GRLPtr<GRLCD3D12Texture> renderTargets[8];
  GRLPtr<GRLCD3D12DepthMap> depthMap;
  m_numWorkingRTV = numRenderTarget;
  for (int index = 0; index < numRenderTarget; index++) {
    if (pRenderTargets[index]->QueryInterface(
            GRLID_PPV_ARGS(&renderTargets[index])))
      return GRL_TRUE;
    m_workingRTVHandleList[index] = renderTargets[index]->GetRTVHandle();
    renderTargets[index]->ResourceStateTransition(
        m_commandList.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET);
  }

  if (pDepthMap) {
    m_isWorkingDSV = true;
    if (pDepthMap->QueryInterface(GRLID_PPV_ARGS(&depthMap)))
      return GRL_TRUE;
    m_workingDSVHandle = depthMap->GetDSVHandle();
  } else
    m_isWorkingDSV = false;
  m_commandList->OMSetRenderTargets(
      m_numWorkingRTV, m_workingRTVHandleList, m_isWorkingDSV,
      m_isWorkingDSV ? (&m_workingDSVHandle) : nullptr);
  return GRL_FALSE;
}

GRL_RESULT GRLCD3D12Environment::SetInternalRenderTarget() {
  if (!m_duringDraw)
    return GRL_TRUE;
  m_numWorkingRTV = 1;
  m_isWorkingDSV = 1;
  m_workingDSVHandle = m_internalDepthMap->GetDSVHandle();
  m_workingRTVHandleList[0] = GetSwapChainRenderTargetViewHandle();
  m_commandList->OMSetRenderTargets(
      m_numWorkingRTV, m_workingRTVHandleList, m_isWorkingDSV,
      m_isWorkingDSV ? (&m_workingDSVHandle) : nullptr);
  return GRL_FALSE;
}
GRL_RESULT GRLCD3D12Environment::ClearRenderTargets(GRLColor color) {
  if (!m_duringDraw)
    return GRL_TRUE;
  float color_f[4] = {color.r, color.g, color.b, color.a};
  for (int i = 0; i < m_numWorkingRTV; i++)
    m_commandList->ClearRenderTargetView(m_workingRTVHandleList[i], color_f, 0,
                                         nullptr);
  return GRL_FALSE;
}
GRL_RESULT GRLCD3D12Environment::ClearDepthMap() {
  if (!m_duringDraw)
    return GRL_TRUE;
  if (m_isWorkingDSV)
    m_commandList->ClearDepthStencilView(
        m_workingDSVHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0, 0, 0, nullptr);
  return GRL_FALSE;
}
GRL_RESULT GRLCD3D12Environment::DrawInstance(
    GRLIGraphicsBuffer *pVertexBuffer,
    uint32_t numVertices,
    GRL_RENDER_TOPOLOGY renderTopology) {
  if (!m_duringDraw)
    return GRL_TRUE;
  GRLPtr<GRLCD3D12Buffer> vertexBuffer;
  if (pVertexBuffer->QueryInterface(GRLID_PPV_ARGS(&vertexBuffer)))
    return GRL_TRUE;
  D3D12_VERTEX_BUFFER_VIEW vertexBufferView =
      vertexBuffer->GetVertexBufferView(m_workingPipelineState->m_vertexStride);
  m_commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
  m_commandList->IASetPrimitiveTopology(
      GRLTopologyToD3D12Topology(renderTopology));
  m_commandList->DrawInstanced(numVertices, 1, 0, 0);
  return GRL_FALSE;
}
GRL_RESULT GRLCD3D12Environment::DrawIndexedInstance(
    GRLIGraphicsBuffer *pVertexBuffer,
    GRLIGraphicsBuffer *pIndexBuffer,
    uint32_t numVertices,
    GRL_RENDER_TOPOLOGY renderTopology) {
  if (!m_duringDraw)
    return GRL_TRUE;
  GRLPtr<GRLCD3D12Buffer> vertexBuffer;
  GRLPtr<GRLCD3D12Buffer> indexBuffer;
  if (pVertexBuffer->QueryInterface(GRLID_PPV_ARGS(&vertexBuffer)))
    return GRL_TRUE;
  if (pIndexBuffer->QueryInterface(GRLID_PPV_ARGS(&indexBuffer)))
    return GRL_TRUE;
  D3D12_VERTEX_BUFFER_VIEW vertexBufferView =
      vertexBuffer->GetVertexBufferView(m_workingPipelineState->m_vertexStride);
  D3D12_INDEX_BUFFER_VIEW indexBufferView = indexBuffer->GetIndexBufferView();
  m_commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
  m_commandList->IASetIndexBuffer(&indexBufferView);
  m_commandList->IASetPrimitiveTopology(
      GRLTopologyToD3D12Topology(renderTopology));
  m_commandList->DrawIndexedInstanced(numVertices, 1, 0, 0, 0);
  return GRL_FALSE;
}
GRL_RESULT GRLCD3D12Environment::EndDraw() {
  if (FAILED(GRLComCall(m_commandList->Close())))
    return GRL_TRUE;

  ID3D12CommandList *ppCommandLists[] = {m_commandList.Get()};

  m_commandQueue->ExecuteCommandLists(1, ppCommandLists);

  m_duringDraw = false;
  return GRL_FALSE;
}
void GRLCD3D12Environment::WaitForGpu() {
  GRLComCall(
      m_commandQueue->Signal(m_fence.Get(), m_fenceValues[m_frameIndex]));

  // Wait until the fence has been processed.
  GRLComCall(
      m_fence->SetEventOnCompletion(m_fenceValues[m_frameIndex], m_fenceEvent));
  WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);

  // Increment the fence value for the current frame.
  m_fenceValues[m_frameIndex]++;
}
void GRLCD3D12Environment::Present(uint32_t enableInterval) {
  GRLComCall(m_swapChain->Present(enableInterval, 0));
  WaitForPreviousFrame();
}
GRL_RESULT GRLCD3D12Environment::AddRef() {
  __Ref_Cnt++;
  return GRL_FALSE;
}
GRL_RESULT GRLCD3D12Environment::Release() {
  __Ref_Cnt--;
  if (!__Ref_Cnt)
    delete this;
  return GRL_FALSE;
}
GRL_RESULT GRLCD3D12Environment::QueryInterface(GRLUUID Uuid, void **ppObject) {
  if (Uuid == GRLID_IGraphicsEnvironment)
    *ppObject = (GRLIGraphicsEnvironment *)this;
  else if (Uuid == GRLID_CD3D12Environment)
    *ppObject = (GRLCD3D12Environment *)this;
  else if (Uuid == GRLID_IBase)
    *ppObject = (GRLIBase *)this;
  else
    return GRL_TRUE;
  AddRef();
  return GRL_FALSE;
}

ID3D12Device *GRLCD3D12Environment::GetDevice() {
  return m_device.Get();
}

ID3D12GraphicsCommandList *GRLCD3D12Environment::GetCommandList() {
  return m_commandList.Get();
}

ID3D12Resource *GRLCD3D12Environment::GetSwapChainFrameResource(
    int frameIndex) {
  return m_renderTargets[frameIndex].Get();
}

ID3D12Resource *GRLCD3D12Environment::GetSwapChainFrameResource() {
  return GetSwapChainFrameResource(m_frameIndex);
}

D3D12_CPU_DESCRIPTOR_HANDLE
GRLCD3D12Environment::GetSwapChainRenderTargetViewHandle(int frameIndex) {
  return CD3DX12_CPU_DESCRIPTOR_HANDLE(
      m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), frameIndex,
      m_device->GetDescriptorHandleIncrementSize(
          D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
}

D3D12_CPU_DESCRIPTOR_HANDLE
GRLCD3D12Environment::GetSwapChainRenderTargetViewHandle() {
  return GetSwapChainRenderTargetViewHandle(m_frameIndex);
}

GRL_RESULT GRLCD3D12Environment::AcquireSRVHandle(
    D3D12_CPU_DESCRIPTOR_HANDLE &cpuHandle,
    D3D12_GPU_DESCRIPTOR_HANDLE &gpuHandle) {
  if (m_queueSrvCPUHandle.empty())
    return GRL_TRUE;
  cpuHandle = m_queueSrvCPUHandle.front();
  m_queueSrvCPUHandle.pop();
  gpuHandle = m_queueSrvGPUHandle.front();
  m_queueSrvGPUHandle.pop();
  return GRL_FALSE;
}

GRL_RESULT GRLCD3D12Environment::ReturnSRVHandle(
    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle,
    D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle) {
  m_queueSrvCPUHandle.push(cpuHandle);
  m_queueSrvGPUHandle.push(gpuHandle);
  return GRL_FALSE;
}

void GRLCD3D12Environment::MoveToNextFrame() {
  // Schedule a Signal command in the queue.
  const UINT64 currentFenceValue = m_fenceValues[m_frameIndex];
  GRLComCall(m_commandQueue->Signal(m_fence.Get(), currentFenceValue));

  // Update the frame index.
  m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

  // If the next frame is not ready to be rendered yet, wait until it is ready.
  if (m_fence->GetCompletedValue() < m_fenceValues[m_frameIndex]) {
    GRLComCall(m_fence->SetEventOnCompletion(m_fenceValues[m_frameIndex],
                                             m_fenceEvent));
    WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);
  }

  // Set the fence value for the next frame.
  m_fenceValues[m_frameIndex] = currentFenceValue + 1;
}

ID3D12GraphicsCommandList *GRLCD3D12Environment::BeginCopy() {
  WaitForCopy();
  if (FAILED(GRLComCall(m_copyCommandAllocator->Reset())))
    return nullptr;
  if (FAILED(GRLComCall(
          m_copyCommandList->Reset(m_copyCommandAllocator.Get(), nullptr))))
    return nullptr;
  return m_copyCommandList.Get();
}

GRL_RESULT GRLCD3D12Environment::EndCopy() {
  if (FAILED(GRLComCall(m_copyCommandList->Close())))
    return GRL_TRUE;
  ID3D12CommandList *commandLists[] = {m_copyCommandList.Get()};
  m_commandQueue->ExecuteCommandLists(1, commandLists);
  GRLComCall(m_commandQueue->Signal(m_copyFence.Get(), m_copyFenceValue));
  return GRL_FALSE;
}

void GRLCD3D12Environment::WaitForCopy() {
  // Wait until the fence has been processed.
  if (m_copyFence->GetCompletedValue() < m_copyFenceValue) {
    GRLComCall(
        m_copyFence->SetEventOnCompletion(m_copyFenceValue, m_copyFenceEvent));
    WaitForSingleObjectEx(m_copyFenceEvent, INFINITE, FALSE);
  }

  // Increment the fence value for the current frame.
  m_copyFenceValue++;
}

GRLCD3D12Environment::GRLCD3D12Environment(uint32_t screen_width,
                                           uint32_t screen_height,
                                           const char *window_title) {
  m_duringDraw = 0;
  m_width = screen_width;
  m_height = screen_height;
  __Ref_Cnt = 1;
  HINSTANCE hInstance = GetModuleHandle(nullptr);
  WNDCLASSEX windowClass = {0};
  windowClass.cbSize = sizeof(WNDCLASSEX);
  windowClass.style = CS_HREDRAW | CS_VREDRAW;
  windowClass.lpfnWndProc = GRLD3D12EnvironmentProcFunc;
  windowClass.hInstance = hInstance;
  windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
  windowClass.lpszClassName = "GrasslandDirectXWindowClass";
  RegisterClassEx(&windowClass);


  RECT windowRect = {0, 0, static_cast<LONG>(screen_width),
                     static_cast<LONG>(screen_height)};
  AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
#ifdef _UNICODE
  std::wstring wtitle = GRLStringUTF8toUnicode(window_title);
#else
  std::string wtitle = window_title;
#endif

#ifdef _DEBUG
  wtitle = wtitle + L" [D3D12]";
#endif
  // Create the window and store a handle to it.
  m_hWnd = CreateWindow(windowClass.lpszClassName, wtitle.c_str(),
                        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                        windowRect.right - windowRect.left,
                        windowRect.bottom - windowRect.top,
                        nullptr,  // We have no parent window.
                        nullptr,  // We aren't using menus.
                        hInstance, this);

  OnInit();

  ShowWindow(m_hWnd, SW_SHOW);
  DragAcceptFiles(m_hWnd, TRUE);
}
void GRLCD3D12Environment::GetSize(uint32_t *width, uint32_t *height) {
  if (width)
    *width = m_width;
  if (height)
    *height = m_height;
}
GRL_RESULT GRLCD3D12Environment::PollEvents() {
  MSG msg = {};
  while (msg.message != WM_QUIT && PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
    // Process any messages in the queue.
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return msg.message == WM_QUIT;
}
GRL_RESULT GRLCD3D12Environment::Resize(uint32_t width, uint32_t height) {
  SetWindowPos(m_hWnd, NULL, 0, 0, width, height, SWP_NOMOVE);
  return GRL_FALSE;
}
GRL_RESULT GRLCD3D12Environment::CreateTexture(
    uint32_t width,
    uint32_t height,
    GRL_FORMAT format,
    GRLIGraphicsTexture **ppTexture) {
  if (m_queueSrvCPUHandle.empty())
    return GRL_TRUE;
  *ppTexture = new GRLCD3D12Texture(width, height, format, this);
  return GRL_FALSE;
}
GRL_RESULT GRLCD3D12Environment::CreateDepthMap(
    uint32_t width,
    uint32_t height,
    GRLIGraphicsDepthMap **ppDepthMap) {
  *ppDepthMap = new GRLCD3D12DepthMap(width, height, this);
  return GRL_FALSE;
}
GRL_RESULT GRLCD3D12Environment::CreateBuffer(uint64_t size,
                                              GRL_GRAPHICS_BUFFER_TYPE type,
                                              GRL_GRAPHICS_BUFFER_USAGE usage,
                                              void *pData,
                                              GRLIGraphicsBuffer **ppBuffer) {
  *ppBuffer = new GRLCD3D12Buffer(size, type, usage, pData, this);
  return GRL_FALSE;
}
GRL_RESULT GRLCD3D12Environment::CreatePipelineState(
    const char *shader_path,
    GRL_GRAPHICS_PIPELINE_STATE_DESC *desc,
    GRLIGraphicsPipelineState **ppPipelineState) {
  if (m_duringDraw)
    GRL_TRUE;
  *ppPipelineState = new GRLCD3D12PipelineState(this, shader_path, desc);
  return GRL_FALSE;
}
GRL_RESULT GRLCD3D12Environment::BeginDraw() {
  if (m_duringDraw)
    return GRL_TRUE;
  if (FAILED(GRLComCall(m_commandAllocator->Reset())))
    return GRL_TRUE;
  if (FAILED(
          GRLComCall(m_commandList->Reset(m_commandAllocator.Get(), nullptr))))
    return GRL_TRUE;
  m_duringDraw = true;
  m_workingPipelineState.Reset();

  ID3D12DescriptorHeap *ppHeaps[] = {m_srvHeap.Get()};
  m_commandList->SetDescriptorHeaps(1, ppHeaps);
  return GRL_FALSE;
}
GRL_RESULT GRLCD3D12Environment::ApplyPipelineState(
    GRLIGraphicsPipelineState *pPipelineState) {
  if (!m_duringDraw)
    return GRL_TRUE;
  GRLPtr<GRLCD3D12PipelineState> pGRLD3D12PipelineState;
  if (pPipelineState->QueryInterface(GRLID_PPV_ARGS(&pGRLD3D12PipelineState)))
    return GRL_TRUE;
  m_commandList->SetPipelineState(pGRLD3D12PipelineState->GetPipelineState());
  m_commandList->SetGraphicsRootSignature(
      pGRLD3D12PipelineState->GetRootSignature());
  m_workingPipelineState = pGRLD3D12PipelineState;
  return GRL_FALSE;
}
LRESULT __stdcall GRLCD3D12Environment::GRLD3D12EnvironmentProcFunc(
    HWND hWnd,
    UINT Msg,
    WPARAM wParam,
    LPARAM lParam) {
  static std::map<HWND, GRLCD3D12Environment *> env_map;
  GRLCD3D12Environment *pEnv = nullptr;
  if (env_map.count(hWnd))
    pEnv = env_map[hWnd];
  switch (Msg) {
    case WM_CREATE:
      pEnv = reinterpret_cast<GRLCD3D12Environment *>(
          reinterpret_cast<CREATESTRUCT *>(lParam)->lpCreateParams);
      env_map[hWnd] = pEnv;
      return 0;
    case WM_SIZE:
      pEnv->OnResize(LOWORD(lParam), HIWORD(lParam));
      return 0;
    case WM_CLOSE:
      DestroyWindow(hWnd);
      return 0;
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
  }
  // printf("%x", Msg);
  // std::cout << " " << (LONG)wParam << " " << (LONG)lParam << std::endl;
  return DefWindowProc(hWnd, Msg, wParam, lParam);
}
GRL_RESULT GRLCD3D12Environment::OnInit() {
  return LoadPipeline();
}
GRL_RESULT GRLCD3D12Environment::OnResize(uint32_t width, uint32_t height) {
  m_width = width;
  m_height = height;

  for (UINT n = 0; n < GRLD3D12FrameCount; n++) {
    m_renderTargets[n].Reset();
    m_fenceValues[n] = m_fenceValues[m_frameIndex];
  }

  DXGI_SWAP_CHAIN_DESC1 desc = {};
  m_swapChain->GetDesc1(&desc);
  GRLComCall(m_swapChain->ResizeBuffers(GRLD3D12FrameCount, m_width, m_height,
                                        DXGI_FORMAT_UNKNOWN, desc.Flags));
  m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

  m_internalDepthMap.Reset();

  __build_rtvdsvResources();
  return GRL_FALSE;
}
GRL_RESULT GRLCD3D12Environment::LoadPipeline() {
  if (__create_d3d12_device())
    return GRL_TRUE;
  if (__create_command_queue())
    return GRL_TRUE;
  if (__create_swapchain())
    return GRL_TRUE;
  if (__create_fence())
    return GRL_TRUE;
  return GRL_FALSE;
}
GRL_RESULT GRLCD3D12Environment::__create_d3d12_device() {
  UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
  // Enable the debug layer (requires the Graphics Tools "optional feature").
  // NOTE: Enabling the debug layer after device creation will invalidate the
  // active device.
  {
    ComPtr<ID3D12Debug> debugController;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
      debugController->EnableDebugLayer();

      // Enable additional debug layers.
      dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
    }
  }
#endif
  if (FAILED(GRLComCall(
          CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_factory)))))
    return GRL_TRUE;

  ComPtr<IDXGIAdapter1> adapter;
  GRLD3D12SelectAdapter(m_factory.Get(), &adapter);

#if defined(_DEBUG)
  {
    DXGI_ADAPTER_DESC1 desc;
    adapter->GetDesc1(&desc);
    std::cout << "[INFO] Graphics Adapter: ["
              << GRLStringUnicodetoUTF8(desc.Description) << "]\n";
  }
#endif

  if (FAILED(GRLComCall(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0,
                                          IID_PPV_ARGS(&m_device)))))
    return GRL_TRUE;

  return GRL_FALSE;
}

GRL_RESULT GRLCD3D12Environment::__create_swapchain() {
  DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
  swapChainDesc.BufferCount = GRLD3D12FrameCount;
  swapChainDesc.Width = m_width;
  swapChainDesc.Height = m_height;
  swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
  swapChainDesc.SampleDesc.Count = 1;

  ComPtr<IDXGISwapChain1> swapChain;
  if (FAILED(GRLComCall(m_factory->CreateSwapChainForHwnd(
          m_commandQueue.Get(),  // Swap chain needs the queue so that it can
                                 // force a flush on it.
          m_hWnd, &swapChainDesc, nullptr, nullptr, &swapChain))

                 ))
    return GRL_TRUE;

  if (FAILED(GRLComCall(
          m_factory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER))))
    return GRL_TRUE;

  if (FAILED(GRLComCall(swapChain.As(&m_swapChain))))
    return GRL_TRUE;

  uint32_t numSrvHandle = (1 << 19);
  D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
  srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
  srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
  srvHeapDesc.NumDescriptors = numSrvHandle;

  if (FAILED(GRLComCall(m_device->CreateDescriptorHeap(
          &srvHeapDesc, IID_PPV_ARGS(&m_srvHeap)))))
    return GRL_TRUE;

  uint32_t srvHandleIncrementSize = m_device->GetDescriptorHandleIncrementSize(
      D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
  CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle(
      m_srvHeap->GetCPUDescriptorHandleForHeapStart());
  CD3DX12_GPU_DESCRIPTOR_HANDLE srvGPUHandle(
      m_srvHeap->GetGPUDescriptorHandleForHeapStart());
  for (int i = 0; i < numSrvHandle; i++) {
    m_queueSrvCPUHandle.push(srvHandle);
    m_queueSrvGPUHandle.push(srvGPUHandle);
    srvHandle.Offset(srvHandleIncrementSize);
    srvGPUHandle.Offset(srvHandleIncrementSize);
  }

  __build_rtvdsvResources();
  return GRL_FALSE;
}
GRL_RESULT GRLCD3D12Environment::__create_command_queue() {
  D3D12_COMMAND_QUEUE_DESC queueDesc = {};
  queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
  queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

  if (FAILED(GRLComCall(m_device->CreateCommandQueue(
          &queueDesc, IID_PPV_ARGS(&m_commandQueue)))))
    return GRL_TRUE;
  if (FAILED(GRLComCall(m_device->CreateCommandAllocator(
          D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)))))
    return GRL_TRUE;
  if (FAILED(GRLComCall(m_device->CreateCommandList(
          0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr,
          IID_PPV_ARGS(&m_commandList)))))
    return GRL_TRUE;
  if (FAILED(GRLComCall(m_commandList->Close())))
    return GRL_TRUE;
  if (FAILED(GRLComCall(m_device->CreateCommandAllocator(
          D3D12_COMMAND_LIST_TYPE_DIRECT,
          IID_PPV_ARGS(&m_copyCommandAllocator)))))
    return GRL_TRUE;
  if (FAILED(GRLComCall(m_device->CreateCommandList(
          0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_copyCommandAllocator.Get(),
          nullptr, IID_PPV_ARGS(&m_copyCommandList)))))
    return GRL_TRUE;
  if (FAILED(GRLComCall(m_copyCommandList->Close())))
    return GRL_TRUE;

  return GRL_FALSE;
}
GRL_RESULT GRLCD3D12Environment::__create_fence() {
  if (FAILED(GRLComCall(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE,
                                              IID_PPV_ARGS(&m_fence)))))
    return GRL_TRUE;
  for (int n = 0; n < GRLD3D12FrameCount; n++)
    m_fenceValues[n] = 1;

  m_fenceEvent = CreateEvent(0, 0, 0, 0);
  if (FAILED(GRLComCall(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE,
                                              IID_PPV_ARGS(&m_copyFence)))))
    return GRL_TRUE;
  m_copyFenceValue = 1;

  m_copyFenceEvent = CreateEvent(0, 0, 0, 0);
  WaitForPreviousFrame();
  GRLComCall(m_commandQueue->Signal(m_copyFence.Get(), m_copyFenceValue));
  return GRL_FALSE;
}
GRL_RESULT GRLCD3D12Environment::__build_rtvdsvResources() {
  m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

  D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
  rtvHeapDesc.NumDescriptors = GRLD3D12FrameCount;
  rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
  rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

  m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap));

  CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
      m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
  uint32_t m_rtvDescripterSize = m_device->GetDescriptorHandleIncrementSize(
      D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

  for (int index = 0; index < GRLD3D12FrameCount; index++) {
    GRLComCall(
        m_swapChain->GetBuffer(index, IID_PPV_ARGS(&m_renderTargets[index])));
    m_device->CreateRenderTargetView(m_renderTargets[index].Get(), nullptr,
                                     rtvHandle);

    rtvHandle.Offset(m_rtvDescripterSize);
  }

  *(&m_internalDepthMap) = new GRLCD3D12DepthMap(m_width, m_height, this);

  return GRL_FALSE;
}
void GRLCD3D12Environment::WaitForPreviousFrame() {
  MoveToNextFrame();
  WaitForGpu();
}
GRL_RESULT GRLD3D12SelectAdapter(IDXGIFactory *pFactory,
                                 IDXGIAdapter1 **ppAdapter) {
  *ppAdapter = nullptr;

  ComPtr<IDXGIFactory6> factory6;
  ComPtr<IDXGIAdapter1> adapter1;

  if (FAILED(GRLComCall(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))) {
    return GRL_TRUE;
  }

  for (UINT adapterIndex = 0;
       SUCCEEDED(GRLComCall(factory6->EnumAdapterByGpuPreference(
           adapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
           IID_PPV_ARGS(&adapter1))));
       ++adapterIndex) {
    DXGI_ADAPTER_DESC1 desc;
    adapter1->GetDesc1(&desc);

    if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
      // Don't select the Basic Render Driver adapter.
      // If you want a software adapter, pass in "/warp" on the command line.
      continue;
    }

    // Check to see whether the adapter supports Direct3D 12, but don't create
    // the actual device yet.
    if (SUCCEEDED(
            GRLComCall(D3D12CreateDevice(adapter1.Get(), D3D_FEATURE_LEVEL_12_0,
                                         _uuidof(ID3D12Device), nullptr)))) {
      break;
    }
  }

  *ppAdapter = adapter1.Detach();
  return GRL_FALSE;
}
DXGI_FORMAT GRLFormatToDXGIFormat(GRL_FORMAT grl_format) {
  switch (grl_format) {
    case GRL_FORMAT::FLOAT:
      return DXGI_FORMAT_R32_FLOAT;
    case GRL_FORMAT::FLOAT2:
      return DXGI_FORMAT_R32G32_FLOAT;
    case GRL_FORMAT::FLOAT3:
      return DXGI_FORMAT_R32G32B32_FLOAT;
    case GRL_FORMAT::FLOAT4:
      return DXGI_FORMAT_R32G32B32A32_FLOAT;
    case GRL_FORMAT::INT:
      return DXGI_FORMAT_R32_SINT;
    case GRL_FORMAT::INT2:
      return DXGI_FORMAT_R32G32_SINT;
    case GRL_FORMAT::INT3:
      return DXGI_FORMAT_R32G32B32_SINT;
    case GRL_FORMAT::INT4:
      return DXGI_FORMAT_R32G32B32A32_SINT;
    case GRL_FORMAT::UINT:
      return DXGI_FORMAT_R32_UINT;
    case GRL_FORMAT::UINT2:
      return DXGI_FORMAT_R32G32_UINT;
    case GRL_FORMAT::UINT3:
      return DXGI_FORMAT_R32G32B32_UINT;
    case GRL_FORMAT::UINT4:
      return DXGI_FORMAT_R32G32B32A32_UINT;
    case GRL_FORMAT::BYTE:
      return DXGI_FORMAT_R8_UNORM;
    case GRL_FORMAT::BYTE2:
      return DXGI_FORMAT_R8G8_UNORM;
    case GRL_FORMAT::BYTE4:
      return DXGI_FORMAT_R8G8B8A8_UNORM;
  }
  return DXGI_FORMAT_UNKNOWN;
}
D3D12_PRIMITIVE_TOPOLOGY GRLTopologyToD3D12Topology(GRL_RENDER_TOPOLOGY topo) {
  switch (topo) {
    case GRL_RENDER_TOPOLOGY::TRIANGLE:
      return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    case GRL_RENDER_TOPOLOGY::LINE:
      return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
  }
  return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}
D3D12_FILTER GRLFilterToD3D12Filter(
    GRL_GRAPHICS_SAMPLER_FILTER sampler_filter) {
  switch (sampler_filter) {
    case GRL_GRAPHICS_SAMPLER_FILTER::MIN_MAG_LINEAR:
      return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
    case GRL_GRAPHICS_SAMPLER_FILTER::MIP_LINEAR:
      return D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR;
    case GRL_GRAPHICS_SAMPLER_FILTER::LINEAR:
      return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
  }
  return D3D12_FILTER_MIN_MAG_MIP_POINT;
}
D3D12_TEXTURE_ADDRESS_MODE GRLExtensionModeToD3D12TextureAddressMode(
    GRL_GRAPHICS_TEXTURE_EXTENSION_MODE extension_mode) {
  switch (extension_mode) {
    case GRL_GRAPHICS_TEXTURE_EXTENSION_MODE::CLAMP:
      return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    case GRL_GRAPHICS_TEXTURE_EXTENSION_MODE::MIRROR:
      return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
    case GRL_GRAPHICS_TEXTURE_EXTENSION_MODE::REPEAT:
      return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
      //		case GRL_GRAPHICS_TEXTURE_EXTENSION_MODE::CLAMP:
      // D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
  }
  return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
}
GRL_RESULT GRLCreateD3D12Environment(uint32_t width,
                                     uint32_t height,
                                     const char *window_title,
                                     GRLIGraphicsEnvironment **ppEnvironment) {
  *ppEnvironment = new GRLCD3D12Environment(width, height, window_title);
  return GRL_FALSE;
}
GRLCD3D12Texture::GRLCD3D12Texture(uint32_t width,
                                   uint32_t height,
                                   GRL_FORMAT format,
                                   GRLCD3D12Environment *pEnvironment) {
  m_environment = pEnvironment;
  __Ref_Cnt = 1;
  m_width = width;
  m_height = height;
  m_format = format;
  m_dxgi_format = GRLFormatToDXGIFormat(format);
  ComPtr<ID3D12Device> device(pEnvironment->GetDevice());
  CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
  CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
      m_dxgi_format, m_width, m_height, 1, 0, 1, 0,
      D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET |
          D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
  GRLComCall(device->CreateCommittedResource(
      &heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc,
      D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, nullptr,
      IID_PPV_ARGS(&m_texture)));

  m_curResourceState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

  heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
  device->GetCopyableFootprints(&resourceDesc, 0, 1, 0, &m_footprint,
                                &m_numRows, &m_rowSizeInBytes, &m_bufferSize);
  resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(m_bufferSize);
  GRLComCall(device->CreateCommittedResource(
      &heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc,
      D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
      IID_PPV_ARGS(&m_uploadBuffer)));
  heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK);
  GRLComCall(device->CreateCommittedResource(
      &heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc,
      D3D12_RESOURCE_STATE_COPY_DEST, nullptr,
      IID_PPV_ARGS(&m_downloadBuffer)));

  D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
  rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
  rtvHeapDesc.NumDescriptors = 1;
  rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
  GRLComCall(
      device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));

  // D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
  device->CreateRenderTargetView(
      m_texture.Get(), nullptr,
      m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

  m_environment->AcquireSRVHandle(m_srvCPUHandle, m_srvGPUHandle);

  D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
  srvDesc.Format = m_dxgi_format;
  srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
  srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
  srvDesc.Texture2D.MipLevels = 1;

  device->CreateShaderResourceView(m_texture.Get(), &srvDesc, m_srvCPUHandle);
}
GRLCD3D12Texture::~GRLCD3D12Texture() {
  m_environment->ReturnSRVHandle(m_srvCPUHandle, m_srvGPUHandle);
}
GRL_RESULT GRLCD3D12Texture::WritePixels(void *pData) {
  CD3DX12_RANGE range(0, 0);
  uint8_t *gpu_buffer;
  uint8_t *cpu_buffer = reinterpret_cast<uint8_t *>(pData);
  GRLComCall(
      m_uploadBuffer->Map(0, &range, reinterpret_cast<void **>(&gpu_buffer)));
  int32_t m_pixel_size = GRLFormatSizeInBytes(m_format);

  for (int i = 0; i < m_numRows; i++) {
    memcpy(gpu_buffer, cpu_buffer, m_width * m_pixel_size);
    gpu_buffer += m_rowSizeInBytes;
    cpu_buffer += m_width * m_pixel_size;
  }
  m_uploadBuffer->Unmap(0, nullptr);

  auto commandList = m_environment->BeginCopy();
  if (!commandList)
    return GRL_TRUE;
  ResourceStateTransition(commandList, D3D12_RESOURCE_STATE_COPY_DEST);
  CD3DX12_TEXTURE_COPY_LOCATION
  src(m_uploadBuffer.Get(), m_footprint), dst(m_texture.Get(), 0);
  commandList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
  m_environment->EndCopy();
  return GRL_FALSE;
}
GRL_RESULT GRLCD3D12Texture::ReadPixels(void *pData) {
  auto commandList = m_environment->BeginCopy();
  if (!commandList)
    return GRL_TRUE;
  ResourceStateTransition(commandList, D3D12_RESOURCE_STATE_COPY_SOURCE);
  CD3DX12_TEXTURE_COPY_LOCATION
  dst(m_downloadBuffer.Get(), m_footprint), src(m_texture.Get(), 0);
  commandList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
  m_environment->EndCopy();

  CD3DX12_RANGE range(0, m_bufferSize);
  uint8_t *gpu_buffer;
  uint8_t *cpu_buffer = reinterpret_cast<uint8_t *>(pData);
  GRLComCall(
      m_downloadBuffer->Map(0, &range, reinterpret_cast<void **>(&gpu_buffer)));
  int32_t m_pixel_size = GRLFormatSizeInBytes(m_format);

  for (int i = 0; i < m_numRows; i++) {
    memcpy(cpu_buffer, gpu_buffer, m_width * m_pixel_size);
    cpu_buffer += m_width * m_pixel_size;
    gpu_buffer += m_rowSizeInBytes;
  }
  m_uploadBuffer->Unmap(0, nullptr);

  return GRL_FALSE;
}
GRL_RESULT GRLCD3D12Texture::GetSize(uint32_t *pWidth, uint32_t *pHeight) {
  if (pWidth)
    *pWidth = m_width;
  if (pHeight)
    *pHeight = m_height;
  return GRL_FALSE;
}
GRL_FORMAT GRLCD3D12Texture::GetFormat() {
  return m_format;
}
GRL_RESULT GRLCD3D12Texture::AddRef() {
  __Ref_Cnt++;
  return GRL_FALSE;
}
GRL_RESULT GRLCD3D12Texture::Release() {
  __Ref_Cnt--;
  if (!__Ref_Cnt)
    delete this;
  return GRL_FALSE;
}
GRL_RESULT GRLCD3D12Texture::QueryInterface(GRLUUID uuid, void **ppObject) {
  if (uuid == GRLID_IBase)
    *ppObject = (GRLIBase *)this;
  else if (uuid == GRLID_IGraphicsTexture)
    *ppObject = (GRLIGraphicsTexture *)this;
  else if (uuid == GRLID_CD3D12Texture)
    *ppObject = (GRLCD3D12Texture *)this;
  else
    return GRL_TRUE;
  AddRef();
  return GRL_FALSE;
}
ID3D12Resource *GRLCD3D12Texture::GetResource() {
  return m_texture.Get();
}
D3D12_CPU_DESCRIPTOR_HANDLE GRLCD3D12Texture::GetRTVHandle() {
  return m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
}
D3D12_GPU_DESCRIPTOR_HANDLE GRLCD3D12Texture::GetSRVHandle() {
  return m_srvGPUHandle;
}
void GRLCD3D12Texture::ResourceStateTransition(
    ID3D12GraphicsCommandList *commandList,
    D3D12_RESOURCE_STATES newResourceState) {
  if (newResourceState == m_curResourceState)
    return;
  CD3DX12_RESOURCE_BARRIER resource_barrier;
  resource_barrier = CD3DX12_RESOURCE_BARRIER::Transition(
      m_texture.Get(), m_curResourceState, newResourceState);
  commandList->ResourceBarrier(1, &resource_barrier);
  m_curResourceState = newResourceState;
}
GRLCD3D12Buffer::GRLCD3D12Buffer(uint64_t size,
                                 GRL_GRAPHICS_BUFFER_TYPE type,
                                 GRL_GRAPHICS_BUFFER_USAGE usage,
                                 void *pData,
                                 GRLCD3D12Environment *pEnvironment) {
  if (type == GRL_GRAPHICS_BUFFER_TYPE::CONSTANT)
    size = ((size + 255) & 0xffffffffffffff00u);
  __Ref_Cnt = 1;
  m_environment = pEnvironment;
  m_size = size;
  m_type = type;
  ComPtr<ID3D12Device> device(pEnvironment->GetDevice());
  CD3DX12_HEAP_PROPERTIES heapProperties;
  CD3DX12_RESOURCE_DESC resourceDesc;

  if (usage == GRL_GRAPHICS_BUFFER_USAGE::DEFAULT)
    if (type == GRL_GRAPHICS_BUFFER_TYPE::CONSTANT)
      usage = GRL_GRAPHICS_BUFFER_USAGE::DYNAMIC;
    else
      usage = GRL_GRAPHICS_BUFFER_USAGE::STATIC;
  m_usage = usage;

  D3D12_RESOURCE_STATES initResourceState = D3D12_RESOURCE_STATE_GENERIC_READ;

  if (usage == GRL_GRAPHICS_BUFFER_USAGE::STATIC) {
    switch (type) {
      case GRL_GRAPHICS_BUFFER_TYPE::CONSTANT:
      case GRL_GRAPHICS_BUFFER_TYPE::VERTEX:
        initResourceState = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
        break;
      case GRL_GRAPHICS_BUFFER_TYPE::INDEX:
        initResourceState = D3D12_RESOURCE_STATE_INDEX_BUFFER;
        break;
    }
  }

  resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(size);

  if (usage == GRL_GRAPHICS_BUFFER_USAGE::STATIC)
    heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
  else
    heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
  GRLComCall(device->CreateCommittedResource(
      &heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, initResourceState,
      nullptr, IID_PPV_ARGS(&m_buffer)));

  m_uploadBuffer.Reset();

  if (m_usage == GRL_GRAPHICS_BUFFER_USAGE::STATIC) {
    heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    GRLComCall(device->CreateCommittedResource(
        &heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
        IID_PPV_ARGS(&m_uploadBuffer)));
  }

  if (pData) {
    if (usage == GRL_GRAPHICS_BUFFER_USAGE::STATIC) {
      CD3DX12_RANGE range(0, 0);
      uint8_t *pDst;
      GRLComCall(
          m_uploadBuffer->Map(0, &range, reinterpret_cast<void **>(&pDst)));
      memcpy(pDst, pData, size);
      m_uploadBuffer->Unmap(0, nullptr);

      if (m_usage == GRL_GRAPHICS_BUFFER_USAGE::STATIC) {
        auto commandList = m_environment->BeginCopy();
        if (!commandList)
          return;

        CD3DX12_RESOURCE_BARRIER resourceBarrier[2];
        resourceBarrier[0] = CD3DX12_RESOURCE_BARRIER::Transition(
            m_buffer.Get(), initResourceState, D3D12_RESOURCE_STATE_COPY_DEST);
        commandList->ResourceBarrier(1, resourceBarrier);
        commandList->CopyBufferRegion(m_buffer.Get(), 0, m_uploadBuffer.Get(),
                                      0, size);
        resourceBarrier[1] = CD3DX12_RESOURCE_BARRIER::Transition(
            m_buffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, initResourceState);
        commandList->ResourceBarrier(1, resourceBarrier + 1);
        m_environment->EndCopy();
      }
    } else {
      CD3DX12_RANGE range(0, 0);
      uint8_t *pDst;
      GRLComCall(m_buffer->Map(0, &range, reinterpret_cast<void **>(&pDst)));
      memcpy(pDst, pData, size);
      m_buffer->Unmap(0, nullptr);
    }
  }
}
GRL_RESULT GRLCD3D12Buffer::WriteData(uint64_t size,
                                      uint64_t offset,
                                      void *pData) {
  if (m_usage == GRL_GRAPHICS_BUFFER_USAGE::DYNAMIC) {
    CD3DX12_RANGE range(0, 0);
    uint8_t *pDst;
    GRLComCall(m_buffer->Map(0, &range, reinterpret_cast<void **>(&pDst)));
    memcpy(pDst + offset, pData, size);
    m_buffer->Unmap(0, nullptr);
  } else {
    CD3DX12_RANGE range(0, 0);
    uint8_t *pDst;
    GRLComCall(
        m_uploadBuffer->Map(0, &range, reinterpret_cast<void **>(&pDst)));
    memcpy(pDst + offset, pData, size);
    m_uploadBuffer->Unmap(0, nullptr);
    D3D12_RESOURCE_STATES initResourceState = D3D12_RESOURCE_STATE_GENERIC_READ;
    switch (m_type) {
      case GRL_GRAPHICS_BUFFER_TYPE::CONSTANT:
      case GRL_GRAPHICS_BUFFER_TYPE::VERTEX:
        initResourceState = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
        break;
      case GRL_GRAPHICS_BUFFER_TYPE::INDEX:
        initResourceState = D3D12_RESOURCE_STATE_INDEX_BUFFER;
        break;
    }
    auto commandList = m_environment->BeginCopy();
    if (!commandList)
      return GRL_TRUE;

    CD3DX12_RESOURCE_BARRIER resourceBarrier[2];
    resourceBarrier[0] = CD3DX12_RESOURCE_BARRIER::Transition(
        m_buffer.Get(), initResourceState, D3D12_RESOURCE_STATE_COPY_DEST);
    commandList->ResourceBarrier(1, resourceBarrier);
    commandList->CopyBufferRegion(m_buffer.Get(), 0, m_uploadBuffer.Get(), 0,
                                  size);
    resourceBarrier[1] = CD3DX12_RESOURCE_BARRIER::Transition(
        m_buffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, initResourceState);
    commandList->ResourceBarrier(1, resourceBarrier + 1);
    m_environment->EndCopy();
  }

  return GRL_FALSE;
}
GRL_RESULT GRLCD3D12Buffer::AddRef() {
  __Ref_Cnt++;
  return GRL_FALSE;
}
GRL_RESULT GRLCD3D12Buffer::Release() {
  __Ref_Cnt--;
  if (!__Ref_Cnt)
    delete this;
  return GRL_FALSE;
}
GRL_RESULT GRLCD3D12Buffer::QueryInterface(GRLUUID uuid, void **ppObject) {
  if (uuid == GRLID_IBase)
    *ppObject = reinterpret_cast<GRLIBase *>(this);
  else if (uuid == GRLID_IGraphicsBuffer)
    *ppObject = reinterpret_cast<GRLIGraphicsBuffer *>(this);
  else if (uuid == GRLID_CD3D12Buffer)
    *ppObject = reinterpret_cast<GRLCD3D12Buffer *>(this);
  else
    return GRL_TRUE;
  AddRef();
  return GRL_FALSE;
}
D3D12_VERTEX_BUFFER_VIEW GRLCD3D12Buffer::GetVertexBufferView(
    uint32_t strideInBytes) {
  D3D12_VERTEX_BUFFER_VIEW res;
  res.BufferLocation = m_buffer->GetGPUVirtualAddress();
  res.SizeInBytes = m_size;
  res.StrideInBytes = strideInBytes;
  return res;
}
D3D12_INDEX_BUFFER_VIEW GRLCD3D12Buffer::GetIndexBufferView() {
  D3D12_INDEX_BUFFER_VIEW res;
  res.BufferLocation = m_buffer->GetGPUVirtualAddress();
  res.Format = DXGI_FORMAT_R32_UINT;
  res.SizeInBytes = m_size;
  return res;
}
ID3D12Resource *GRLCD3D12Buffer::GetResource() {
  return m_buffer.Get();
}
GRLCD3D12DepthMap::GRLCD3D12DepthMap(uint32_t width,
                                     uint32_t height,
                                     GRLCD3D12Environment *pEnvironment) {
  __Ref_Cnt = 1;
  m_width = width;
  m_height = height;
  ComPtr<ID3D12Device> device(pEnvironment->GetDevice());
  CD3DX12_HEAP_PROPERTIES d3dx12_heap_properties =
      CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
  CD3DX12_RESOURCE_DESC d3dx12_resource_desc = CD3DX12_RESOURCE_DESC::Tex2D(
      DXGI_FORMAT_D32_FLOAT, m_width, m_height, 1, 0, 1, 0,
      D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
  D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
  depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
  depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
  depthOptimizedClearValue.DepthStencil.Stencil = 0;
  GRLComCall(device->CreateCommittedResource(
      &d3dx12_heap_properties, D3D12_HEAP_FLAG_NONE, &d3dx12_resource_desc,
      D3D12_RESOURCE_STATE_DEPTH_WRITE, &depthOptimizedClearValue,
      IID_PPV_ARGS(&m_depthMap)));

  D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};

  dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
  dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
  dsvHeapDesc.NumDescriptors = 1;

  GRLComCall(
      device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsvHeap)));

  device->CreateDepthStencilView(
      m_depthMap.Get(), nullptr,
      m_dsvHeap->GetCPUDescriptorHandleForHeapStart());
}
GRL_RESULT GRLCD3D12DepthMap::GetSize(uint32_t *pWidth, uint32_t *pHeight) {
  if (pWidth)
    *pWidth = m_width;
  if (pHeight)
    *pHeight = m_height;
  return GRL_FALSE;
}
GRL_RESULT GRLCD3D12DepthMap::AddRef() {
  __Ref_Cnt++;
  return GRL_FALSE;
}
GRL_RESULT GRLCD3D12DepthMap::Release() {
  __Ref_Cnt--;
  if (!__Ref_Cnt)
    delete this;
  return GRL_FALSE;
}
GRL_RESULT GRLCD3D12DepthMap::QueryInterface(GRLUUID uuid, void **ppObject) {
  if (uuid == GRLID_IBase)
    *ppObject = reinterpret_cast<GRLIBase *>(this);
  else if (uuid == GRLID_IGraphicsDepthMap)
    *ppObject = reinterpret_cast<GRLIGraphicsDepthMap *>(this);
  else if (uuid == GRLID_CD3D12DepthMap)
    *ppObject = reinterpret_cast<GRLCD3D12DepthMap *>(this);
  else
    return GRL_TRUE;
  AddRef();
  return GRL_FALSE;
}
D3D12_CPU_DESCRIPTOR_HANDLE GRLCD3D12DepthMap::GetDSVHandle() {
  return m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
}
}  // namespace Grassland
