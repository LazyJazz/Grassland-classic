#include "GraphicsDirectX.h"

namespace Grassland
{
	GRLCD3D12PipelineState::GRLCD3D12PipelineState(
		GRLCD3D12Environment* pEnvironment, 
		const char* shader_path,
		GRL_GRAPHICS_PIPELINE_STATE_DESC* desc)
	{
		__Ref_Cnt = 1;
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
		std::wstring wshader_path = GRLStringUTF8toUnicode(shader_path);

		if (FAILED(
			GRLComCall(D3DCompileFromFile(wshader_path.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, &shaderErrorMsg))
		))
		{
			std::cout << "[Vertex Shader Compile Error]  Error Message: \n" << (char*)shaderErrorMsg->GetBufferPointer() << std::endl;
			return;
		}

		if (FAILED(
			GRLComCall(D3DCompileFromFile(wshader_path.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, &shaderErrorMsg))
		))
		{
			std::cout << "[Pixel Shader Compile Error]  Error Message: \n" << (char*)shaderErrorMsg->GetBufferPointer() << std::endl;
			return;
		}



		/***************************************
		* Create RootSignature
		****************************************/
		{
			D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

			if (FAILED(GRLComCall(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData)))))
			{
				featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
			}


			CD3DX12_DESCRIPTOR_RANGE1* ranges = nullptr;

			if (desc->numTexture)
			{
				ranges = new CD3DX12_DESCRIPTOR_RANGE1[desc->numTexture];
				for (int index = 0; index < desc->numTexture; index++)
					ranges[index].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, index, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
			}

			CD3DX12_ROOT_PARAMETER1* rootParameters = nullptr;
			if (desc->numTexture + desc->numConstantBuffer)
				rootParameters = new CD3DX12_ROOT_PARAMETER1[desc->numTexture + desc->numConstantBuffer];
			for (int index = 0; index < desc->numTexture; index++)
				rootParameters[index].InitAsDescriptorTable(1, &ranges[index], D3D12_SHADER_VISIBILITY_PIXEL);
			for (int index = 0; index < desc->numConstantBuffer; index++)
				rootParameters[index + desc->numTexture].InitAsConstantBufferView(index);

			D3D12_STATIC_SAMPLER_DESC sampler = {}, samplers[2];
			sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
			sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
			sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
			sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
			sampler.MipLODBias = 0;
			sampler.MaxAnisotropy = 0;
			sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
			sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
			sampler.MinLOD = 0.0f;
			sampler.MaxLOD = D3D12_FLOAT32_MAX;
			sampler.ShaderRegister = 0;
			sampler.RegisterSpace = 0;
			sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			samplers[0] = sampler;
			sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
			sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			sampler.MipLODBias = 0;
			sampler.MaxAnisotropy = 0;
			sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
			sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
			sampler.MinLOD = 0.0f;
			sampler.MaxLOD = D3D12_FLOAT32_MAX;
			sampler.ShaderRegister = 1;
			sampler.RegisterSpace = 0;
			samplers[1] = sampler;


			CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
			rootSignatureDesc.Init_1_1(desc->numTexture + desc->numConstantBuffer, rootParameters, desc->numTexture ? 2 : 0, desc->numTexture ? (samplers) : nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			ComPtr<ID3DBlob> signature;
			ComPtr<ID3DBlob> error;
			if (FAILED(GRLComCall(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error))))
			{
				std::cout << "[Create Root Signature ERROR] : \n" << (char*)error->GetBufferPointer() << std::endl;
			}
			GRLComCall(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));

			if (ranges)
				delete[] ranges;
			if (rootParameters)
				delete[] rootParameters;
		}

		/***************************************
		* Create PipelineState
		****************************************/
		{
			D3D12_INPUT_ELEMENT_DESC* inputElementDescs = new D3D12_INPUT_ELEMENT_DESC[desc->numInputElement];

			for (uint32_t index = 0, offset = 0; index < desc->numInputElement; index++)
			{
				inputElementDescs[index] = D3D12_INPUT_ELEMENT_DESC(
					{ "DATA", index, GRLFormatToDXGIFormat(desc->inputElementLayout[index]), 0, offset, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
				);
				offset += GRLFormatSizeInByte(desc->inputElementLayout[index]);
			}
			/* =
			{
				{ "DATA", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "DATA", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "DATA", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
			};*/

			D3D12_BLEND_DESC blendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

			if (desc->enableBlend)
			{
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
			psoDesc.InputLayout = { inputElementDescs, desc->numInputElement };
			psoDesc.pRootSignature = m_rootSignature.Get();
			psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
			psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
			psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			if (!desc->enableCullFace) psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
			psoDesc.BlendState = blendDesc; //CD3DX12_BLEND_DESC(D3D12_DEFAULT);


			psoDesc.DepthStencilState.DepthEnable = FALSE;
			psoDesc.DepthStencilState.StencilEnable = FALSE;
			if (desc->enableDepthTest)
			{
				psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
				psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
			}
			psoDesc.SampleMask = UINT_MAX;
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			psoDesc.NumRenderTargets = desc->numRenderTargets;
			for (int index = 0; index < desc->numRenderTargets; index++)
				psoDesc.RTVFormats[index] = GRLFormatToDXGIFormat(desc->renderTargetFormatsList[index]);
			psoDesc.SampleDesc.Count = 1;
			

			GRLComCall(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)));
		}
	}
	GRL_RESULT GRLCD3D12PipelineState::AddRef()
	{
		__Ref_Cnt++;
		return GRL_FALSE;
	}
	GRL_RESULT GRLCD3D12PipelineState::Release()
	{
		__Ref_Cnt--;
		if (!__Ref_Cnt)
			delete this;
		return GRL_FALSE;
	}
	GRL_RESULT GRLCD3D12PipelineState::QueryInterface(GRLUUID Uuid, void** ppObject)
	{
		if (Uuid == GRLID_IGraphicsPipelineState) *ppObject = (GRLIGraphicsPipelineState*)this;
		else if (Uuid == GRLID_CD3D12PipelineState) *ppObject = (GRLCD3D12PipelineState*)this;
		else if (Uuid == GRLID_IBase) *ppObject = (GRLIBase*)this;
		else return GRL_TRUE;
		AddRef();
		return GRL_FALSE;
	}
	ID3D12RootSignature* GRLCD3D12PipelineState::GetRootSignature()
	{
		return m_rootSignature.Get();
	}
	ID3D12PipelineState* GRLCD3D12PipelineState::GetPipelineState()
	{
		return m_pipelineState.Get();
	}
	GRL_RESULT GRLCD3D12Environment::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		return GRL_RESULT();
	}
	GRL_RESULT GRLCD3D12Environment::SetConstantBuffer(uint32_t constantBufferIndex, GRLIGraphicsBuffer * constantBuffer)
	{
		return GRL_RESULT();
	}
	GRL_RESULT GRLCD3D12Environment::SetTextures(uint32_t textureIndex, GRLIGraphicsTexture* pTexture)
	{
		return GRL_RESULT();
	}
	GRL_RESULT GRLCD3D12Environment::SetRenderTargets(GRLIGraphicsTexture* pRenderTargetList, GRLIGraphicsDepthMap* pDepthMap)
	{
		return GRL_RESULT();
	}
	GRL_RESULT GRLCD3D12Environment::SetInternalRenderTarget()
	{
		if (!m_duringDraw) return GRL_TRUE;
		m_numWorkingRTV = 1;
		m_isWorkingDSV = 0;
		m_workingRTVHandleList[0] = GetSwapChainRenderTargetViewHandle();
		m_commandList->OMSetRenderTargets(m_numWorkingRTV, m_workingRTVHandleList, m_isWorkingDSV, m_isWorkingDSV ? (&m_workingDSVHandle) : nullptr);
		return GRL_FALSE;
	}
	GRL_RESULT GRLCD3D12Environment::ClearRenderTargets(GRLColor color)
	{
		if (!m_duringDraw) return GRL_TRUE;
		float color_f[4] = { color.r, color.g,color.b,color.a };
		for (int i = 0; i < m_numWorkingRTV; i++)
			m_commandList->ClearRenderTargetView(m_workingRTVHandleList[i], color_f, 0, nullptr);
		return GRL_FALSE;
	}
	GRL_RESULT GRLCD3D12Environment::ClearDepthMap()
	{
		if (!m_duringDraw) return GRL_TRUE;
		for (int i = 0; i < m_numWorkingRTV; i++)
			m_commandList->ClearDepthStencilView(m_workingDSVHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0, 0, 0, nullptr);
		return GRL_FALSE;
	}
	GRL_RESULT GRLCD3D12Environment::DrawInstance(GRLIGraphicsBuffer* pVertexBuffer, uint32_t numInstance, GRL_RENDER_TOPOLOGY renderTopology)
	{
		return GRL_RESULT();
	}
	GRL_RESULT GRLCD3D12Environment::DrawIndexedInstance(GRLIGraphicsBuffer* pVertexBuffer, GRLIGraphicsBuffer* pIndexBuffer, uint32_t numInstance, GRL_RENDER_TOPOLOGY renderTopology)
	{
		return GRL_RESULT();
	}
	GRL_RESULT GRLCD3D12Environment::EndDraw()
	{
		if (FAILED(
			GRLComCall(m_commandList->Close())
		)) return GRL_TRUE;

		ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };

		m_commandQueue->ExecuteCommandLists(1, ppCommandLists);

		m_duringDraw = false;
	}
	void GRLCD3D12Environment::WaitForGpu()
	{
		GRLComCall(m_commandQueue->Signal(m_fence.Get(), m_fenceValues[m_frameIndex]));

		// Wait until the fence has been processed.
		GRLComCall(m_fence->SetEventOnCompletion(m_fenceValues[m_frameIndex], m_fenceEvent));
		WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);

		// Increment the fence value for the current frame.
		m_fenceValues[m_frameIndex]++;
	}
	void GRLCD3D12Environment::Present(uint32_t enableInterval)
	{
		GRLComCall(m_swapChain->Present(enableInterval, 0));
		WaitForPreviousFrame();
	}
	GRL_RESULT GRLCD3D12Environment::AddRef()
	{
		__Ref_Cnt++;
		return GRL_FALSE;
	}
	GRL_RESULT GRLCD3D12Environment::Release()
	{
		__Ref_Cnt--;
		if (!__Ref_Cnt)
			delete this;
		return GRL_FALSE;
	}
	GRL_RESULT GRLCD3D12Environment::QueryInterface(GRLUUID Uuid, void** ppObject)
	{
		if (Uuid == GRLID_IGraphicsEnvironment)*ppObject = (GRLIGraphicsEnvironment*)this;
		else if (Uuid == GRLID_CD3D12Environment)*ppObject = (GRLCD3D12Environment*)this;
		else if (Uuid == GRLID_IBase)*ppObject = (GRLIBase*)this;
		else return GRL_TRUE;
		AddRef();
		return GRL_FALSE;
	}

	ID3D12Device* GRLCD3D12Environment::GetDevice()
	{
		return m_device.Get();
	}

	ID3D12GraphicsCommandList* GRLCD3D12Environment::GetCommandList()
	{
		return m_commandList.Get();
	}

	ID3D12Resource* GRLCD3D12Environment::GetSwapChainFrameResource(int frameIndex)
	{
		return m_renderTargets[frameIndex].Get();
	}

	ID3D12Resource* GRLCD3D12Environment::GetSwapChainFrameResource()
	{
		return GetSwapChainFrameResource(m_frameIndex);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GRLCD3D12Environment::GetSwapChainRenderTargetViewHandle(int frameIndex)
	{
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), frameIndex, m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GRLCD3D12Environment::GetSwapChainRenderTargetViewHandle()
	{
		return GetSwapChainRenderTargetViewHandle(m_frameIndex);
	}

	void GRLCD3D12Environment::MoveToNextFrame()
	{
		// Schedule a Signal command in the queue.
		const UINT64 currentFenceValue = m_fenceValues[m_frameIndex];
		GRLComCall(m_commandQueue->Signal(m_fence.Get(), currentFenceValue));

		// Update the frame index.
		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

		// If the next frame is not ready to be rendered yet, wait until it is ready.
		if (m_fence->GetCompletedValue() < m_fenceValues[m_frameIndex])
		{
			GRLComCall(m_fence->SetEventOnCompletion(m_fenceValues[m_frameIndex], m_fenceEvent));
			WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);
		}

		// Set the fence value for the next frame.
		m_fenceValues[m_frameIndex] = currentFenceValue + 1;
	}
	
	GRLCD3D12Environment::GRLCD3D12Environment(uint32_t screen_width, uint32_t screen_height, const char* window_title)
	{
		m_duringDraw = 0;
		m_width = screen_width;
		m_height = screen_height;
		__Ref_Cnt = 1;
		HINSTANCE hInstance = GetModuleHandle(nullptr);
		WNDCLASSEX windowClass = { 0 };
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.style = CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc = GRLD3D12EnvironmentProcFunc;
		windowClass.hInstance = hInstance;
		windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		windowClass.lpszClassName = L"GrasslandDirectXWindowClass";
		RegisterClassEx(&windowClass);

		RECT windowRect = { 0, 0, static_cast<LONG>(screen_width), static_cast<LONG>(screen_height) };
		AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

		// Create the window and store a handle to it.
		m_hWnd = CreateWindow(
			windowClass.lpszClassName,
			GRLStringUTF8toUnicode(window_title).c_str(),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top,
			nullptr,        // We have no parent window.
			nullptr,        // We aren't using menus.
			hInstance,
			this);

		OnInit();

		ShowWindow(m_hWnd, SW_SHOW);
		DragAcceptFiles(m_hWnd, TRUE);
	}
	GRL_RESULT GRLCD3D12Environment::PollEvents()
	{
		MSG msg = {};
		while (msg.message != WM_QUIT && PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// Process any messages in the queue.
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return msg.message == WM_QUIT;
	}
	GRL_RESULT GRLCD3D12Environment::Resize(uint32_t width, uint32_t height)
	{
		SetWindowPos(m_hWnd, NULL, 0, 0, width, height, SWP_NOMOVE);
		return GRL_FALSE;
	}
	GRL_RESULT GRLCD3D12Environment::CreateTexture(uint32_t width, uint32_t height, GRL_FORMAT format, GRLIGraphicsTexture** ppTexture)
	{
		*ppTexture = new GRLCD3D12Texture(width, height, format, this);
		return GRL_FALSE;
	}
	GRL_RESULT GRLCD3D12Environment::CreateDepthMap(uint32_t width, uint32_t height, GRLIGraphicsDepthMap** ppDepthMap)
	{
		return GRL_RESULT();
	}
	GRL_RESULT GRLCD3D12Environment::CreateBuffer(uint64_t size, GRLIGraphicsBuffer** ppBuffer)
	{
		return GRL_RESULT();
	}
	GRL_RESULT GRLCD3D12Environment::CreatePipelineState(
		const char* shader_path,
		GRL_GRAPHICS_PIPELINE_STATE_DESC* desc,
		GRLIGraphicsPipelineState** ppPipelineState)
	{
		if (m_duringDraw) GRL_TRUE;
		*ppPipelineState = new GRLCD3D12PipelineState(this, shader_path, desc);
		return GRL_FALSE;
	}
	GRL_RESULT GRLCD3D12Environment::BeginDraw()
	{
		if(FAILED(
			GRLComCall(m_commandAllocator->Reset())
		)) return GRL_TRUE;
		if(FAILED(
			GRLComCall(m_commandList->Reset(m_commandAllocator.Get(), nullptr))
		)) return GRL_TRUE;
		m_duringDraw = true;
		return GRL_FALSE;
	}
	GRL_RESULT GRLCD3D12Environment::ApplyPipelineState(GRLIGraphicsPipelineState* pPipelineState)
	{
		if (!m_duringDraw) return GRL_TRUE;
		GRLPtr<GRLCD3D12PipelineState> pGRLD3D12PipelineState;
		if (pPipelineState->QueryInterface(GRLID_PPV_ARGS(&pGRLD3D12PipelineState)))
			return GRL_TRUE;
		m_commandList->SetPipelineState(pGRLD3D12PipelineState->GetPipelineState());
		m_commandList->SetGraphicsRootSignature(pGRLD3D12PipelineState->GetRootSignature());
		return GRL_FALSE;
	}
	LRESULT __stdcall GRLCD3D12Environment::GRLD3D12EnvironmentProcFunc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		static std::map<HWND, GRLCD3D12Environment*> env_map;
		GRLCD3D12Environment* pEnv = nullptr;
		if (env_map.count(hWnd))
			pEnv = env_map[hWnd];
		switch (Msg)
		{
		case WM_CREATE:
			pEnv = reinterpret_cast<GRLCD3D12Environment*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
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
		//printf("%x", Msg);
		//std::cout << " " << (LONG)wParam << " " << (LONG)lParam << std::endl;
		return DefWindowProc(hWnd, Msg, wParam, lParam);
	}
	GRL_RESULT GRLCD3D12Environment::OnInit()
	{
		return LoadPipeline();
	}
	GRL_RESULT GRLCD3D12Environment::OnResize(uint32_t width, uint32_t height)
	{
		m_width = width;
		m_height = height;

		for (UINT n = 0; n < GRLD3D12FrameCount; n++)
		{
			m_renderTargets[n].Reset();
			m_fenceValues[n] = m_fenceValues[m_frameIndex];
		}

		DXGI_SWAP_CHAIN_DESC1 desc = {};
		m_swapChain->GetDesc1(&desc);
		GRLComCall(m_swapChain->ResizeBuffers(GRLD3D12FrameCount, m_width, m_height, DXGI_FORMAT_UNKNOWN, desc.Flags));
		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

		__build_rtvResources();
		return GRL_FALSE;
	}
	GRL_RESULT GRLCD3D12Environment::LoadPipeline()
	{
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
	GRL_RESULT GRLCD3D12Environment::__create_d3d12_device()
	{
		UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
		// Enable the debug layer (requires the Graphics Tools "optional feature").
		// NOTE: Enabling the debug layer after device creation will invalidate the active device.
		{
			ComPtr<ID3D12Debug> debugController;
			if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
			{
				debugController->EnableDebugLayer();

				// Enable additional debug layers.
				dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
			}
		}
#endif
		if (FAILED(
			GRLComCall(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_factory)))
		)) return GRL_TRUE;

		ComPtr<IDXGIAdapter1> adapter;
		GRLD3D12SelectAdapter(m_factory.Get(), &adapter);

#if defined(_DEBUG)
		{
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);
			std::cout << "[INFO] Graphics Adapter: [" << GRLStringUnicodetoUTF8(desc.Description) << "]\n";
		}
#endif

		if (FAILED(
			GRLComCall(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_device)))
		)) return GRL_TRUE;



		return GRL_FALSE;
	}


	GRL_RESULT GRLCD3D12Environment::__create_swapchain()
	{
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = GRLFrameCount;
		swapChainDesc.Width = m_width;
		swapChainDesc.Height = m_height;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc.Count = 1;

		ComPtr<IDXGISwapChain1> swapChain;
		if (FAILED(
			GRLComCall(m_factory->CreateSwapChainForHwnd(
				m_commandQueue.Get(),        // Swap chain needs the queue so that it can force a flush on it.
				m_hWnd,
				&swapChainDesc,
				nullptr,
				nullptr,
				&swapChain
			))

		)) return GRL_TRUE;

		if (FAILED(
			GRLComCall(m_factory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER))
		)) return GRL_TRUE;

		if (FAILED(
			GRLComCall(swapChain.As(&m_swapChain))
		)) return GRL_TRUE;

		__build_rtvResources();

		return GRL_FALSE;
	}
	GRL_RESULT GRLCD3D12Environment::__create_command_queue()
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		if (FAILED(
			GRLComCall(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)))
		)) return GRL_TRUE;
		if (FAILED(
			GRLComCall(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)))
		)) return GRL_TRUE;
		if (FAILED(
			GRLComCall(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList)))
		)) return GRL_TRUE;
		if (FAILED(
			GRLComCall(m_commandList->Close())
		)) return GRL_TRUE;


		return GRL_FALSE;
	}
	GRL_RESULT GRLCD3D12Environment::__create_fence()
	{
		if (FAILED(
			GRLComCall(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)))
		)) return GRL_TRUE;
		for (int n = 0; n < GRLD3D12FrameCount; n++)
			m_fenceValues[n] = 1;

		m_fenceEvent = CreateEvent(0, 0, 0, 0);
		WaitForPreviousFrame();
		return GRL_FALSE;
	}
	GRL_RESULT GRLCD3D12Environment::__build_rtvResources()
	{
		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = GRLD3D12FrameCount;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

		m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap));

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
		uint32_t m_rtvDescripterSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		for (int index = 0; index < GRLD3D12FrameCount; index++)
		{
			GRLComCall(m_swapChain->GetBuffer(index, IID_PPV_ARGS(&m_renderTargets[index])));
			m_device->CreateRenderTargetView(m_renderTargets[index].Get(), nullptr, rtvHandle);

			rtvHandle.Offset(m_rtvDescripterSize);
		}

		return GRL_FALSE;
	}
	void GRLCD3D12Environment::WaitForPreviousFrame()
	{
		MoveToNextFrame();
		WaitForGpu();
	}
	GRL_RESULT GRLD3D12SelectAdapter(IDXGIFactory* pFactory, IDXGIAdapter1** ppAdapter)
	{
		*ppAdapter = nullptr;

		ComPtr<IDXGIFactory6> factory6;
		ComPtr<IDXGIAdapter1> adapter1;

		if (FAILED(GRLComCall(pFactory->QueryInterface(IID_PPV_ARGS(&factory6)))))
		{
			return GRL_TRUE;
		}

		for (
			UINT adapterIndex = 0;
			SUCCEEDED(GRLComCall(
				factory6->EnumAdapterByGpuPreference(
					adapterIndex,
					DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
					IID_PPV_ARGS(&adapter1))));
			++adapterIndex)
		{
			DXGI_ADAPTER_DESC1 desc;
			adapter1->GetDesc1(&desc);

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// Don't select the Basic Render Driver adapter.
				// If you want a software adapter, pass in "/warp" on the command line.
				continue;
			}

			// Check to see whether the adapter supports Direct3D 12, but don't create the
			// actual device yet.
			if (SUCCEEDED(GRLComCall(D3D12CreateDevice(adapter1.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr))))
			{
				break;
			}
		}

		*ppAdapter = adapter1.Detach();
		return GRL_FALSE;
	}
	DXGI_FORMAT GRLFormatToDXGIFormat(GRL_FORMAT grl_format)
	{
		switch (grl_format)
		{
		case GRL_FORMAT::FLOAT: return DXGI_FORMAT_R32_FLOAT;
		case GRL_FORMAT::FLOAT2: return DXGI_FORMAT_R32G32_FLOAT;
		case GRL_FORMAT::FLOAT3: return DXGI_FORMAT_R32G32B32_FLOAT;
		case GRL_FORMAT::FLOAT4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case GRL_FORMAT::INT: return DXGI_FORMAT_R32_SINT;
		case GRL_FORMAT::INT2: return DXGI_FORMAT_R32G32_SINT;
		case GRL_FORMAT::INT3: return DXGI_FORMAT_R32G32B32_SINT;
		case GRL_FORMAT::INT4: return DXGI_FORMAT_R32G32B32A32_SINT;
		case GRL_FORMAT::UINT: return DXGI_FORMAT_R32_UINT;
		case GRL_FORMAT::UINT2: return DXGI_FORMAT_R32G32_UINT;
		case GRL_FORMAT::UINT3: return DXGI_FORMAT_R32G32B32_UINT;
		case GRL_FORMAT::UINT4: return DXGI_FORMAT_R32G32B32A32_UINT;
		case GRL_FORMAT::BYTE: return DXGI_FORMAT_R8_UNORM;
		case GRL_FORMAT::BYTE2: return DXGI_FORMAT_R8G8_UNORM;
		case GRL_FORMAT::BYTE4: return DXGI_FORMAT_R8G8B8A8_UNORM;
		}
		return DXGI_FORMAT_UNKNOWN;
	}
	GRL_RESULT GRLCreateD3D12Environment(uint32_t width, uint32_t height, const char* window_title, GRLIGraphicsEnvironment** ppEnvironment)
	{
		*ppEnvironment = new GRLCD3D12Environment(width, height, window_title);
		return GRL_FALSE;
	}
	GRLCD3D12Texture::GRLCD3D12Texture(uint32_t width, uint32_t height, GRL_FORMAT format, GRLCD3D12Environment* pEnvironment)
	{
		__Ref_Cnt = 1;
		m_width = width;
		m_height = height;
		m_format = format;
		m_dxgi_format = GRLFormatToDXGIFormat(format);
		ComPtr<ID3D12Device> device(pEnvironment->GetDevice());
		CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
		CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
			m_dxgi_format,
			m_width,
			m_height,
			1,
			0,
			1,
			0,
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
		);
		device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			nullptr,
			IID_PPV_ARGS(&m_texture)
		);

		heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(
			(uint64_t)m_width* (uint64_t)m_height*(uint64_t)GRLFormatSizeInByte(m_format)
		);
		device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			nullptr,
			IID_PPV_ARGS(&m_texture)
		);
	}
	GRL_RESULT GRLCD3D12Texture::WritePixels(void* pData)
	{
		return GRL_RESULT();
	}
	GRL_RESULT GRLCD3D12Texture::ReadPixels(void* pData)
	{
		return GRL_RESULT();
	}
	GRL_RESULT GRLCD3D12Texture::GetSize(uint32_t* pWidth, uint32_t* pHeight)
	{
		if (pWidth) *pWidth = m_width;
		if (pHeight) *pHeight = m_height;
		return GRL_FALSE;
	}
	GRL_RESULT GRLCD3D12Texture::AddRef()
	{
		__Ref_Cnt++;
		return GRL_FALSE;
	}
	GRL_RESULT GRLCD3D12Texture::Release()
	{
		__Ref_Cnt--;
		if (!__Ref_Cnt)
			delete this;
		return GRL_FALSE;
	}
	GRL_RESULT GRLCD3D12Texture::QueryInterface(GRLUUID uuid, void** ppObject)
	{
		if (uuid == GRLID_IBase) *ppObject = (GRLIBase*)this;
		else if (uuid == GRLID_IGraphicsTexture) *ppObject = (GRLIGraphicsTexture*)this;
		else if (uuid == GRLID_CD3D12Texture) *ppObject = (GRLCD3D12Texture*)this;
		else return GRL_TRUE;
		AddRef();
		return GRL_FALSE;
	}
}