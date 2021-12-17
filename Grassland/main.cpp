#include <Grassland.h>

#include <Windows.h>
#include <d3d12.h>
#include <memory>

#include <iostream>
#include <thread>
#include <chrono>
#include <queue>

#include <DirectXMath.h>

using Microsoft::WRL::ComPtr;

using namespace Grassland;

struct Vertex
{
    GRLVec4 position;
    GRLVec4 normal;
    GRLVec4 texcoord;
};

struct ConstantBufferConstant
{
    GRLMat4 mat;
    int mode;
};

#define CBPaddingSize(x) ((((x)+255) & 0xffffff00u)-(x))

template<typename _Ty>
struct ConstantBuffer
{
    _Ty content;
    char padding[CBPaddingSize(sizeof(_Ty))];
};

int main2();

int main()
{
    return main2();
    SetProcessDPIAware();
    SetConsoleOutputCP(936);
    GRLPtr<GRLIGraphicsEnvironment> pEnvironment;
    GRLPtr<GRLIGraphicsPipelineState> pPipelineState;

    GRLCreateGraphicsEnvironment(1280, 720, "Grassland Graphics", GRL_GRAPHICS_API::D3D12, &pEnvironment);


    GRL_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc;
    pipelineStateDesc.enableBlend = 1;
    pipelineStateDesc.enableDepthTest = 1;
    pipelineStateDesc.enableCullFace = 1;
    pipelineStateDesc.inputElementLayout = new GRL_FORMAT[3];
    pipelineStateDesc.inputElementLayout[0] = GRL_FORMAT::FLOAT4;
    pipelineStateDesc.inputElementLayout[1] = GRL_FORMAT::FLOAT4;
    pipelineStateDesc.inputElementLayout[2] = GRL_FORMAT::FLOAT4;
    pipelineStateDesc.numInputElement = 3;
    pipelineStateDesc.numConstantBuffer = 1;
    pipelineStateDesc.numRenderTargets = 1;
    pipelineStateDesc.numTexture = 1;
    pipelineStateDesc.renderTargetFormatsList[0] = GRL_FORMAT::BYTE4;
    pEnvironment->CreatePipelineState("shaders\\DirectX\\shaders.hlsl", &pipelineStateDesc, &pPipelineState);

    while (!pEnvironment->PollEvents())
    {
        pEnvironment->BeginDraw();
        pEnvironment->SetInternalRenderTarget();
        pEnvironment->ClearRenderTargets(GRLColor(0.0,1.0,0.0,1.0));
        pEnvironment->ApplyPipelineState(pPipelineState.Get());
        pEnvironment->EndDraw();
        pEnvironment->Present(1);
    }
    return 0;
}

int main2()
{
    SetConsoleOutputCP(936);

    GRLCDirectXEnvironment environment(1280, 720, "Grassland D3D12", false);
    DXGI_FORMAT formats[] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32G32B32A32_FLOAT};
    GRLCDirectXPipelineStateAndRootSignature* psoAndRootSignature = new GRLCDirectXPipelineStateAndRootSignature(
        &environment,
        "shaders\\DirectX\\shaders.hlsl",
        1,
        formats,
        1,
        1,
        true,
        true
    );
    GRLCDirectXPipelineStateAndRootSignature* psoAndRootSignatureTex = new GRLCDirectXPipelineStateAndRootSignature(
        &environment,
        "shaders\\DirectX\\shaders.hlsl",
        1,
        formats + 1,
        1,
        1,
        true,
        true
    );

    Vertex vertices[24];
    uint32_t indices[36];

    {
        GRLMat4 mat[6] = {
            GRLTransformLookAt(GRLVec3(0.0,0.0,0.0), GRLVec3(0.0,0.0,1.0)),
            GRLTransformLookAt(GRLVec3(0.0,0.0,0.0), GRLVec3(0.0,0.0,-1.0)),
            GRLTransformLookAt(GRLVec3(0.0,0.0,0.0), GRLVec3(0.0,1.0,0.0)),
            GRLTransformLookAt(GRLVec3(0.0,0.0,0.0), GRLVec3(0.0,-1.0,0.0)),
            GRLTransformLookAt(GRLVec3(0.0,0.0,0.0), GRLVec3(1.0,0.0,0.0)),
            GRLTransformLookAt(GRLVec3(0.0,0.0,0.0), GRLVec3(-1.0,0.0,0.0))
        };
        for (int i = 0; i < 6; i++)
        {
            vertices[i * 4] = Vertex({ {-0.5f, -0.5f,0.5f,1.0f}, {1.0f, 0.0f,0.0f,0.0f}, {0.0f, 0.0f,0.0f,1.0f} });
            vertices[i * 4 + 1] = Vertex({ {-0.5f, -0.5f,0.5f,1.0f}, {1.0f, 0.0f,0.0f,0.0f}, {1.0f, 0.0f,0.0f,1.0f} });
            vertices[i * 4 + 2] = Vertex({ {-0.5f, -0.5f,0.5f,1.0f}, {1.0f, 0.0f,0.0f,0.0f}, {0.0f, 1.0f,0.0f,1.0f} });
            vertices[i * 4 + 3] = Vertex({ {-0.5f, -0.5f,0.5f,1.0f}, {1.0f, 0.0f,0.0f,0.0f}, {1.0f, 1.0f,0.0f,1.0f} });
            vertices[i * 4].position = (mat[i] * GRLVec4(-0.5, -0.5, 0.5, 1.0));
            vertices[i * 4 + 1].position = (mat[i] * GRLVec4(0.5, -0.5, 0.5, 1.0));
            vertices[i * 4 + 2].position = (mat[i] * GRLVec4(-0.5, 0.5, 0.5, 1.0));
            vertices[i * 4 + 3].position = (mat[i] * GRLVec4(0.5, 0.5, 0.5, 1.0));
            vertices[i * 4 + 0].normal = GRLVec4(mat[i][0][2], mat[i][1][2], mat[i][2][2], 0.0);
            vertices[i * 4 + 1].normal = GRLVec4(mat[i][0][2], mat[i][1][2], mat[i][2][2], 0.0);
            vertices[i * 4 + 2].normal = GRLVec4(mat[i][0][2], mat[i][1][2], mat[i][2][2], 0.0);
            vertices[i * 4 + 3].normal = GRLVec4(mat[i][0][2], mat[i][1][2], mat[i][2][2], 0.0);
            indices[i * 6] = i * 4;
            indices[i * 6 + 1] = i * 4 + 1;
            indices[i * 6 + 2] = i * 4 + 2;
            indices[i * 6 + 3] = i * 4 + 2;
            indices[i * 6 + 4] = i * 4 + 1;
            indices[i * 6 + 5] = i * 4 + 3;
        }
    }

    Vertex verticesTex[] = {
        { {-0.5f, -0.5f,0.5f,1.0f}, {1.0f, 0.0f,0.0f,0.0f}, {0.0f, 0.0f,1.0f,1.0f}},
        { {-0.5f, 0.5f,0.5f,1.0f}, {1.0f, 0.0f,0.0f,0.0f}, {0.0f, 1.0f,1.0f,1.0f}},
        { {0.5f, -0.5f,0.5f,1.0f}, {1.0f, 0.0f,0.0f,0.0f}, {1.0f, 0.0f,1.0f,1.0f}},
        { {0.5f, 0.5f,0.5f,1.0f}, {1.0f, 0.0f,0.0f,0.0f}, {1.0f, 1.0f,1.0f,1.0f}},
        { {-0.5f, -0.5f,-0.5f,1.0f}, {1.0f, 0.0f,0.0f,0.0f}, {0.0f, 0.0f,0.0f,1.0f}},
        { {-0.5f, 0.5f,-0.5f,1.0f}, {1.0f, 0.0f,0.0f,0.0f}, {0.0f, 1.0f,0.0f,1.0f}},
        { {0.5f, -0.5f,-0.5f,1.0f}, {1.0f, 0.0f,0.0f,0.0f}, {1.0f, 0.0f,0.0f,1.0f}},
        { {0.5f, 0.5f,-0.5f,1.0f}, {1.0f, 0.0f,0.0f,0.0f}, {1.0f, 1.0f,0.0f,1.0f}}
    };

    uint32_t indicesTex[] = {
        0,2,1,
        1,2,3,
        4,5,6,
        5,7,6,
        0,6,2,
        0,4,6,
        1,3,5,
        3,7,5,
        2,6,3,
        3,6,7,
        0,1,4,
        1,5,4
    };//*/

    ConstantBuffer<ConstantBufferConstant> cb; 

    GRLCDirectXBuffer* pVertexBuffer = new GRLCDirectXBuffer(&environment, sizeof(vertices), 0, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
    GRLCDirectXBuffer* pIndexBuffer = new GRLCDirectXBuffer(&environment, sizeof(indices), 0, D3D12_RESOURCE_STATE_INDEX_BUFFER);
    GRLCDirectXBuffer* pVertexBufferUpload = new GRLCDirectXBuffer(&environment, sizeof(vertices));
    GRLCDirectXBuffer* pIndexBufferUpload = new GRLCDirectXBuffer(&environment, sizeof(indices));
    pVertexBufferUpload->SetBufferData(vertices, sizeof(vertices), 0);
    pIndexBufferUpload->SetBufferData(indices, sizeof(indices), 0);
    GRLCDirectXBuffer* pVertexBufferTex = new GRLCDirectXBuffer(&environment, sizeof(verticesTex));
    GRLCDirectXBuffer* pIndexBufferTex = new GRLCDirectXBuffer(&environment, sizeof(indicesTex));
    pVertexBufferTex->SetBufferData(verticesTex, sizeof(verticesTex), 0);
    pIndexBufferTex->SetBufferData(indicesTex, sizeof(indicesTex), 0);
    GRLCDirectXBuffer* pConstantBuffer = new GRLCDirectXBuffer(&environment, sizeof(cb));
    GRLCDirectXBuffer* pConstantBufferTex = new GRLCDirectXBuffer(&environment, sizeof(cb));

    GRLCDirectXDepthMap* pDepthMap = new GRLCDirectXDepthMap(&environment, 1280, 720);
    GRLCDirectXDepthMap* pDepthMapTex = new GRLCDirectXDepthMap(&environment, 255, 255);
    GRLCDirectXTexture* pTexture = new GRLCDirectXTexture(&environment, 255, 255, nullptr);
    GRLCDirectXBuffer* pTextureUpload = new GRLCDirectXBuffer(&environment, GetRequiredIntermediateSize(pTexture->GetResource(), 0, 1), 1);
    GRLCDirectXBuffer* pTextureReadback = new GRLCDirectXBuffer(&environment, GetRequiredIntermediateSize(pTexture->GetResource(), 0, 1), -1, D3D12_RESOURCE_STATE_COPY_DEST);

    std::cout << pTextureReadback->GetBufferSize() << std::endl;

    GRLColor* pData = new GRLColor[256*256];
    //GRLComCall(pTextureUpload->GetResource()->Map(0, &range, reinterpret_cast<void**>(&pData)));
    for (int x = 0; x < 256; x++)
    {
        for (int y = 0; y < 256; y++)
        {
            pData[x + y * 256] = GRLColor(x, y, x^y);
        }
    }
    pTextureUpload->GetResource()->Unmap(0, nullptr);
    

    //GRLCDirectXBuffer* pUselessBuffer = new GRLCDirectXBuffer(&environment, 1ull << 30ull, 0);

    {
        environment.WaitForGpu();
        auto commandList = environment.StartDraw();
        CD3DX12_RESOURCE_BARRIER rb[8];

        rb[0] = CD3DX12_RESOURCE_BARRIER::Transition(pTexture->GetResource(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
        rb[1] = CD3DX12_RESOURCE_BARRIER::Transition(pIndexBuffer->GetResource(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

        commandList->CopyBufferRegion(pVertexBuffer->GetResource(), 0, pVertexBufferUpload->GetResource(), 0, min(pVertexBuffer->GetBufferSize(), pVertexBufferUpload->GetBufferSize()));

        commandList->CopyBufferRegion(pIndexBuffer->GetResource(), 0, pIndexBufferUpload->GetResource(), 0, min(pIndexBuffer->GetBufferSize(), pIndexBufferUpload->GetBufferSize()));


        //commandList->ResourceBarrier(1, rb);
        D3D12_SUBRESOURCE_DATA subresource_data;
        subresource_data.pData = pData;
        subresource_data.RowPitch = 256 * 16;
        subresource_data.SlicePitch = 256 * 16 * 256;
        UpdateSubresources(commandList, pTexture->GetResource(), pTextureUpload->GetResource(), 0, 0, 1, &subresource_data);
        //commandList->CopyBufferRegion(pTexture->GetResource(), 0, pTextureUpload->GetResource(), 0, pTextureUpload->GetBufferSize());
        //commandList->ResourceBarrier(1, rb + 1);

        rb[2] = CD3DX12_RESOURCE_BARRIER::Transition(pVertexBuffer->GetResource(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
        rb[3] = CD3DX12_RESOURCE_BARRIER::Transition(pIndexBuffer->GetResource(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);

        rb[4] = CD3DX12_RESOURCE_BARRIER::Transition(pTexture->GetResource(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_RENDER_TARGET);
        commandList->ResourceBarrier(1, rb + 4);
        //rb[6] = CD3DX12_RESOURCE_BARRIER::Transition(pIndexBuffer->GetResource(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
        //rb[7] = CD3DX12_RESOURCE_BARRIER::Transition(pIndexBufferUpload->GetResource(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_GENERIC_READ);
        //commandList->ResourceBarrier(2, rb + 6);
        environment.EndDraw();
        //environment.MoveToNextFrame();
        environment.WaitForGpu();
    }

    //std::cout << (void*) & environment << std::endl;

    //ComPtr<ID3D12PipelineState> m_pipelineState;
    //ComPtr<ID3D12Device> m_device;
    //D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc = {};
    //pso_desc.DepthStencilState.DepthEnable = FALSE;
    //pso_desc.DepthStencilState.StencilEnable = FALSE;
    //m_device->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(&m_pipelineState));

    DirectX::XMFLOAT4 hsv(0.5, 1.0, 1.0, 1.0), rgba;
    GRLMat4 rot(1);
    GRLMat4 rotTex(1);
    uint32_t g_cur_width, g_cur_height;
    environment.GetWindowSize(&g_cur_width, &g_cur_height);

    std::queue<std::chrono::steady_clock::time_point> Q;
    //std::cout << (std::chrono::steady_clock::now() - std::chrono::steady_clock::now()) / std::chrono::nanoseconds(1) << std::endl;
    while (!environment.PollEvents())
    {

        {
            std::chrono::steady_clock::time_point ttp = std::chrono::steady_clock::now();
            Q.push(ttp);
            if (Q.size() == 100)
            {
                std::chrono::steady_clock::time_point ftp = Q.front();
                std::chrono::steady_clock::duration dur = ttp - ftp;
                std::cout << "FPS:" << 99.0 / ((double)(dur / std::chrono::nanoseconds(1)) * 1e-9) << std::endl;
                while (!Q.empty())
                    Q.pop();
            }
        }
        uint32_t scr_width, scr_height;
        environment.GetWindowSize(&scr_width, &scr_height);
        //std::cout << "[" << scr_width << ", " << scr_height << "]" << std::endl;
        CD3DX12_VIEWPORT viewPort(0.0f, 0.0f, (float)scr_width, (float)scr_height);
        CD3DX12_RECT scissorRect(0, 0, (LONG)scr_width, (LONG)scr_height);
        CD3DX12_VIEWPORT viewPortTex(0.0f, 0.0f, (float)255, (float)255);
        CD3DX12_RECT scissorRectTex(0, 0, (LONG)255, (LONG)255);

        rot *= GRLTransformRotation(GRLRadian(0.03f), GRLRadian(0.02f), GRLRadian(0.01f));
        rotTex *= GRLTransformRotation(GRLRadian(0.1f), GRLRadian(0.2f), GRLRadian(0.3f));

        cb.content.mat =
            (GRLTransformProjection(GRLRadian(30.0f), (float)scr_width / (float)scr_height, 1.0f, 10.0f) *
                GRLTransformTranslate(0.0f, 0.0f, 5.0f) * rot).transpose();
        cb.content.mode = 1;
        if (g_cur_width != scr_width || g_cur_height != scr_height)
        {
            g_cur_width = scr_width;
            g_cur_height = scr_height;
            //pDepthMap->Resize(scr_width, scr_height);
            pDepthMap->Resize(scr_width, scr_height);
            if (FAILED(GRLComCall(environment.GetDevice()->GetDeviceRemovedReason())))
            {
                std::cout << "Device Crashed!" << std::endl;
            }
        }
        hsv.x += 0.001;
        DirectX::XMStoreFloat4(&rgba, DirectX::XMColorHSVToRGB(DirectX::XMLoadFloat4(&hsv)));
        //float clearcolor[4] = { rgba.x,rgba.y,rgba.z,1.0 };
        ID3D12GraphicsCommandList * commandList = environment.StartDraw();
        commandList->SetPipelineState(psoAndRootSignatureTex->GetPipelineState());
        commandList->SetGraphicsRootSignature(psoAndRootSignatureTex->GetRootSignature());
        CD3DX12_RESOURCE_BARRIER resourceBarrier[8];

        pConstantBuffer->SetBufferData(&cb, sizeof(cb), 0);
        cb.content.mat =
            (GRLTransformProjection(GRLRadian(30.0f), 1.0f, 1.0f, 10.0f) *
                GRLTransformTranslate(0.0f, 0.0f, 5.0f) * rotTex).transpose();
        cb.content.mode = 0;
        pConstantBufferTex->SetBufferData(&cb, sizeof(cb), 0);

        resourceBarrier[0] = CD3DX12_RESOURCE_BARRIER::Transition(environment.GetFrameResource(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
        commandList->ResourceBarrier(1, resourceBarrier); 

        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = environment.GetBackFrameRTVHandle();
        D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = pDepthMap->GetDSVHandle();
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandleTex = pTexture->GetRTV();
        D3D12_CPU_DESCRIPTOR_HANDLE dsvHandleTex = pDepthMapTex->GetDSVHandle();
        float clearcolor[4] = { 0.6,0.7,0.8,1.0 };
        float texcolor[4] = { 0.8,0.7,0.6,1.0 };
        environment.ClearBackFrameColor(clearcolor);
        //commandList->ClearRenderTargetView(rtvHandleTex, texcolor, 0, nullptr);
        commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0, 0, 0, nullptr);
        commandList->ClearDepthStencilView(dsvHandleTex, D3D12_CLEAR_FLAG_DEPTH, 1.0, 0, 0, nullptr);
        commandList->SetGraphicsRootConstantBufferView(1, pConstantBufferTex->GetResource()->GetGPUVirtualAddress());
        commandList->OMSetRenderTargets(1, &rtvHandleTex, true, &dsvHandleTex);

        commandList->RSSetScissorRects(1, &scissorRectTex);
        commandList->RSSetViewports(1, &viewPortTex);
        commandList->SetGraphicsRootConstantBufferView(1, pConstantBufferTex->GetResource()->GetGPUVirtualAddress());

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        D3D12_VERTEX_BUFFER_VIEW vertexBufferViewTex = pVertexBufferTex->GetVertexBufferView(sizeof(Vertex));
        commandList->IASetVertexBuffers(0, 1, &vertexBufferViewTex);
        D3D12_INDEX_BUFFER_VIEW indexBufferViewTex = pIndexBufferTex->GetIndexBufferView();
        commandList->IASetIndexBuffer(&indexBufferViewTex);
        //commandList->DrawIndexedInstanced(36, 1, 0, 0, 0);

        commandList->OMSetRenderTargets(1, &rtvHandle, true, &dsvHandle);

        commandList->RSSetScissorRects(1, &scissorRect);
        commandList->RSSetViewports(1, &viewPort);
        commandList->SetGraphicsRootConstantBufferView(1, pConstantBuffer->GetResource()->GetGPUVirtualAddress());
        ID3D12DescriptorHeap* ppHeaps[] = { pTexture->GetSRVHeap() };
        commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
        commandList->SetGraphicsRootDescriptorTable(0, pTexture->GetSRV());

        resourceBarrier[1] = CD3DX12_RESOURCE_BARRIER::Transition(pTexture->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        commandList->ResourceBarrier(1, resourceBarrier + 1);

        D3D12_VERTEX_BUFFER_VIEW vertexBufferView = pVertexBuffer->GetVertexBufferView(sizeof(Vertex));
        commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
        D3D12_INDEX_BUFFER_VIEW indexBufferView = pIndexBuffer->GetIndexBufferView();
        commandList->IASetIndexBuffer(&indexBufferView);
        commandList->DrawIndexedInstanced(36, 1, 0, 0, 0);

        resourceBarrier[3] = CD3DX12_RESOURCE_BARRIER::Transition(pTexture->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_SOURCE);
        resourceBarrier[2] = CD3DX12_RESOURCE_BARRIER::Transition(environment.GetFrameResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
        commandList->ResourceBarrier(2, resourceBarrier + 2);

        D3D12_PLACED_SUBRESOURCE_FOOTPRINT Layout;
        Layout.Offset = 0;
        Layout.Footprint.Depth = 1;
        Layout.Footprint.Width = 255;
        Layout.Footprint.Height = 255;
        Layout.Footprint.RowPitch = 256 * 16;
        Layout.Footprint.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        
        CD3DX12_TEXTURE_COPY_LOCATION Dst(pTextureReadback->GetResource(), Layout);
        CD3DX12_TEXTURE_COPY_LOCATION Src(pTexture->GetResource(), 0);
        commandList->CopyTextureRegion(&Dst, 0, 0, 0, &Src, nullptr);

        resourceBarrier[4] = CD3DX12_RESOURCE_BARRIER::Transition(pTexture->GetResource(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
        commandList->ResourceBarrier(1, resourceBarrier + 4);
        

        environment.EndDraw();
        environment.Present(0);

        {
            GRLColor* pBuffer;
            GRLIImage* pImage;
            GRLCreateImage(255, 255, &pImage);
            pImage->GetImageBuffer(&pBuffer);
            CD3DX12_RANGE range(0, 255 * 256 * 16 - 16);
            GRLColor* pSrcBuffer;
            GRLComCall(pTextureReadback->GetResource()->Map(0, &range, reinterpret_cast<void**>(&pSrcBuffer)));
            for (int i = 0; i < 255; i++)
                memcpy(pBuffer + 255 * i, pSrcBuffer + 256 * i, 255 * 16);
            pTextureReadback->GetResource()->Unmap(0, nullptr);
            pImage->StoreBMP("texture.bmp");
            pImage->Release();
        }
    }

    pVertexBuffer->Release();
    psoAndRootSignature->Release();
    return 0;
}