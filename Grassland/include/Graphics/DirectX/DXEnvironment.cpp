#include "DXEnvironment.h"

namespace Grassland
{

    GRL_RESULT GRLDirectXSelectAdapter(IDXGIFactory* pFactory, IDXGIAdapter1** ppAdapter)
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

    GRLCDirectXEnvironment::GRLCDirectXEnvironment(uint32_t screen_width, uint32_t screen_height, const char* window_title, bool full_screen)
    {
        m_width = screen_width;
        m_height = screen_height;
        __Ref_Cnt = 1;
        HINSTANCE hInstance = GetModuleHandle(nullptr);
        WNDCLASSEX windowClass = { 0 };
        windowClass.cbSize = sizeof(WNDCLASSEX);
        windowClass.style = CS_HREDRAW | CS_VREDRAW;
        windowClass.lpfnWndProc = GRLDirectXEnvironmentProcFunc;
        windowClass.hInstance = hInstance;
        windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        windowClass.lpszClassName = L"GrasslandDirectXWindowClass";
        RegisterClassEx(&windowClass);

        RECT windowRect = { 0, 0, static_cast<LONG>(screen_width), static_cast<LONG>(screen_height) };
        AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

        std::wstring window_title_wc = GRLStringUTF8toUnicode(window_title);
        // Create the window and store a handle to it.
        m_hWnd = CreateWindow(
            windowClass.lpszClassName,
            window_title_wc.c_str(),
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

    GRL_RESULT GRLCDirectXEnvironment::PollEvents()
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

    GRL_RESULT GRLCDirectXEnvironment::AddRef()
    {
        __Ref_Cnt++;
        return 0;
    }
    GRL_RESULT GRLCDirectXEnvironment::Release()
    {
        __Ref_Cnt--;
        if (!__Ref_Cnt) delete this;
        return 0;
    }
    GRL_RESULT GRLCDirectXEnvironment::QueryInterface(GRLUUID uuid, void** ppObject)
    {
        return GRL_FALSE;
    }
    void GRLCDirectXEnvironment::WaitForGpu()
    {
        GRLComCall(m_commandQueue->Signal(m_fence.Get(), m_fenceValues[m_frameIndex]));

        // Wait until the fence has been processed.
        GRLComCall(m_fence->SetEventOnCompletion(m_fenceValues[m_frameIndex], m_fenceEvent));
        WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);

        // Increment the fence value for the current frame.
        m_fenceValues[m_frameIndex]++;
    }
    void GRLCDirectXEnvironment::MoveToNextFrame()
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
    HWND GRLCDirectXEnvironment::GetHWnd()
    {
        return m_hWnd;
    }

    ID3D12GraphicsCommandList* GRLCDirectXEnvironment::StartDraw()
    {
        GRLComCall(m_commandAllocator->Reset());
        GRLComCall(m_commandList->Reset(m_commandAllocator.Get(), nullptr));

        return m_commandList.Get();
    }

    void GRLCDirectXEnvironment::EndDraw()
    {
        GRLComCall(m_commandList->Close());

        ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };

        m_commandQueue->ExecuteCommandLists(1, ppCommandLists);
    }

    void GRLCDirectXEnvironment::Present(uint32_t enableInterval)
    {
        GRLComCall(m_swapChain->Present(enableInterval, 0));
        WaitForPreviousFrame();
    }

    ID3D12GraphicsCommandList* GRLCDirectXEnvironment::GetCommandList()
    {
        return m_commandList.Get();
    }

    ID3D12Device* GRLCDirectXEnvironment::GetDevice()
    {
        return m_device.Get();
    }

    void GRLCDirectXEnvironment::GetWindowSize(uint32_t* width, uint32_t* height)
    {
        if (width) *width = m_width;
        if (height) *height = m_height;
    }

    void GRLCDirectXEnvironment::ClearBackFrameColor(float* color)
    {

        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));

        m_commandList->ClearRenderTargetView(rtvHandle, color, 0, nullptr);
    }

    D3D12_CPU_DESCRIPTOR_HANDLE GRLCDirectXEnvironment::GetRTVHandle(int frameIndex)
    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), frameIndex, m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
        return rtvHandle;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE GRLCDirectXEnvironment::GetBackFrameRTVHandle()
    {
        return GetRTVHandle(m_frameIndex);
    }

    ID3D12Resource* GRLCDirectXEnvironment::GetFrameResource()
    {
        return GetFrameResource(m_frameIndex);
    }

    ID3D12Resource* GRLCDirectXEnvironment::GetFrameResource(int frameIndex)
    {
        return m_renderTargets[frameIndex].Get();
    }

    void GRLCDirectXEnvironment::Resize(uint32_t width, uint32_t height)
    {
        SetWindowPos(m_hWnd, NULL, 0, 0, width, height, SWP_NOMOVE);
    }

    LRESULT __stdcall GRLCDirectXEnvironment::GRLDirectXEnvironmentProcFunc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
    {
        static std::map<HWND, GRLCDirectXEnvironment*> env_map;
        GRLCDirectXEnvironment*pEnv = nullptr;
        if (env_map.count(hWnd))
            pEnv = env_map[hWnd];
        switch (Msg)
        {
        case WM_CREATE:
            pEnv = reinterpret_cast<GRLCDirectXEnvironment*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
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
    
    GRL_RESULT GRLCDirectXEnvironment::OnInit()
    {
        return LoadPipeline();
    }

    GRL_RESULT GRLCDirectXEnvironment::OnResize(uint32_t width, uint32_t height)
    {
        m_width = width;
        m_height = height;

        for (UINT n = 0; n < GRLFrameCount; n++)
        {
            m_renderTargets[n].Reset();
            m_fenceValues[n] = m_fenceValues[m_frameIndex];
        }

        DXGI_SWAP_CHAIN_DESC1 desc = {};
        m_swapChain->GetDesc1(&desc);
        GRLComCall(m_swapChain->ResizeBuffers(GRLFrameCount, m_width, m_height, DXGI_FORMAT_UNKNOWN, desc.Flags));
        m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

        __build_rtvResources();
        return GRL_FALSE;
    }

    GRL_RESULT GRLCDirectXEnvironment::LoadPipeline()
    {
        __create_d3d12_device();
        __create_command_queue();
        __create_swapchain();
        __create_fence();
        return GRL_FALSE;
    }
    
    GRL_RESULT GRLCDirectXEnvironment::__create_d3d12_device()
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
        GRLComCall(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_factory)));

        ComPtr<IDXGIAdapter1> adapter;
        GRLDirectXSelectAdapter(m_factory.Get(), &adapter);

#if defined(_DEBUG)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);
            std::cout << "[INFO] Graphics Adapter: [" << GRLStringUnicodetoUTF8(desc.Description) << "]\n";
        }
#endif

        GRLComCall(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_device)));

        

        return GRL_FALSE;
    }
    
    GRL_RESULT GRLCDirectXEnvironment::__create_swapchain()
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
        GRLComCall(m_factory->CreateSwapChainForHwnd(
            m_commandQueue.Get(),        // Swap chain needs the queue so that it can force a flush on it.
            m_hWnd,
            &swapChainDesc,
            nullptr,
            nullptr,
            &swapChain
        ));

        GRLComCall(m_factory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER));

        GRLComCall(swapChain.As(&m_swapChain));
        
        __build_rtvResources();

        return GRL_FALSE;
    }
    
    GRL_RESULT GRLCDirectXEnvironment::__create_command_queue()
    {
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

        GRLComCall(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));
        GRLComCall(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));
        GRLComCall(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList)));

        GRLComCall(m_commandList->Close());


        return GRL_FALSE;
    }

    GRL_RESULT GRLCDirectXEnvironment::__create_fence()
    {
        GRLComCall(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
        for (int n = 0; n < GRLFrameCount; n++)
            m_fenceValues[n] = 1;

        m_fenceEvent = CreateEvent(0, 0, 0, 0);
        WaitForPreviousFrame();
        return GRL_FALSE;
    }

    GRL_RESULT GRLCDirectXEnvironment::__build_rtvResources()
    {
        m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = GRLFrameCount;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

        m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap));

        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
        uint32_t m_rtvDescripterSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        for (int index = 0; index < GRLFrameCount; index++)
        {
            GRLComCall(m_swapChain->GetBuffer(index, IID_PPV_ARGS(&m_renderTargets[index])));
            m_device->CreateRenderTargetView(m_renderTargets[index].Get(), nullptr, rtvHandle);

            rtvHandle.Offset(m_rtvDescripterSize);
        }

        return GRL_FALSE;
    }
    
    void GRLCDirectXEnvironment::WaitForPreviousFrame()
    {
        MoveToNextFrame();
        WaitForGpu();
    }
    GRLCDirectXPipelineStateAndRootSignature::GRLCDirectXPipelineStateAndRootSignature(
        GRLCDirectXEnvironment* pEnvironment, 
        const char* shaderFilePath, 
        uint32_t numberRenderTarget, 
        DXGI_FORMAT* formats, 
        uint32_t numberConstantBuffer, 
        uint32_t numberTexture,
        bool enableDepthTest,
        bool enableBlend
    )
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

        if(FAILED(
            GRLComCall(D3DCompileFromFile(GRLStringUTF8toUnicode(shaderFilePath).c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, &shaderErrorMsg))
        ))
        {
            std::cout << "[Vertex Shader Compile Error]  Error Message: \n" << (char*)shaderErrorMsg->GetBufferPointer() << std::endl;
            return;
        }

        if (FAILED(
        GRLComCall(D3DCompileFromFile(GRLStringUTF8toUnicode(shaderFilePath).c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, &shaderErrorMsg))
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
            
            if (numberTexture)
            {
                ranges = new CD3DX12_DESCRIPTOR_RANGE1[numberTexture];
                for (int index = 0; index < numberTexture; index++)
                    ranges[index].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, index, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
            }

            CD3DX12_ROOT_PARAMETER1 *rootParameters = nullptr;
            if (numberTexture + numberConstantBuffer)
                rootParameters = new CD3DX12_ROOT_PARAMETER1[numberTexture + numberConstantBuffer];
            for (int index = 0; index < numberTexture; index++)
                rootParameters[index].InitAsDescriptorTable(1, &ranges[index], D3D12_SHADER_VISIBILITY_PIXEL);
            for (int index = 0; index < numberConstantBuffer; index++)
                rootParameters[index + numberTexture].InitAsConstantBufferView(index);

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
            rootSignatureDesc.Init_1_1(numberTexture + numberConstantBuffer, rootParameters, numberTexture ? 2 : 0, numberTexture ? (samplers) : nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

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
            D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
            {
                { "DATA", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
                { "DATA", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
                { "DATA", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
            };

            D3D12_BLEND_DESC blendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

            if(enableBlend)
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
            psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
            psoDesc.pRootSignature = m_rootSignature.Get();
            psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
            psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
            psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
            //psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
            psoDesc.BlendState = blendDesc; //CD3DX12_BLEND_DESC(D3D12_DEFAULT);

            
            psoDesc.DepthStencilState.DepthEnable = FALSE;
            psoDesc.DepthStencilState.StencilEnable = FALSE;
            if (enableDepthTest)
            {
                psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
                psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
            }
            psoDesc.SampleMask = UINT_MAX;
            psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
            psoDesc.NumRenderTargets = numberRenderTarget;
            for (int index = 0; index < numberRenderTarget; index++)
                psoDesc.RTVFormats[index] = formats[index];
            psoDesc.SampleDesc.Count = 1;

            GRLComCall(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)));
        }
    }
    ID3D12RootSignature* GRLCDirectXPipelineStateAndRootSignature::GetRootSignature()
    {
        return m_rootSignature.Get();
    }
    ID3D12PipelineState* GRLCDirectXPipelineStateAndRootSignature::GetPipelineState()
    {
        return m_pipelineState.Get();
    }
    GRL_RESULT GRLCDirectXPipelineStateAndRootSignature::AddRef()
    {
        __Ref_Cnt++;
        return GRL_FALSE;
    }
    GRL_RESULT GRLCDirectXPipelineStateAndRootSignature::Release()
    {
        __Ref_Cnt--;
        if (!__Ref_Cnt)
            delete this;
        return GRL_FALSE;
    }
    GRL_RESULT GRLCDirectXPipelineStateAndRootSignature::QueryInterface(GRLUUID uuid, void** ppObject)
    {
        return GRL_FALSE;
    }
    GRLCDirectXBuffer::GRLCDirectXBuffer(GRLCDirectXEnvironment* pEnvironment, uint64_t size, uint32_t uploadBuffer, D3D12_RESOURCE_STATES resourceState)
    {
        __Ref_Cnt = 1;
        m_size = size;
        ComPtr<ID3D12Device> device(pEnvironment->GetDevice());
        CD3DX12_HEAP_PROPERTIES d3dx12_heap_properties = CD3DX12_HEAP_PROPERTIES(uploadBuffer ? ((uploadBuffer == 1) ? D3D12_HEAP_TYPE_UPLOAD : D3D12_HEAP_TYPE_READBACK) : D3D12_HEAP_TYPE_DEFAULT);
        CD3DX12_RESOURCE_DESC d3dx12_resource_desc = CD3DX12_RESOURCE_DESC::Buffer(size);
        GRLComCall(device->CreateCommittedResource(
            &d3dx12_heap_properties,
            D3D12_HEAP_FLAG_NONE,
            &d3dx12_resource_desc,
            resourceState,
            nullptr,
            IID_PPV_ARGS(&m_buffer)));
    }
    
    
    ID3D12Resource* GRLCDirectXBuffer::GetResource()
    {
        return m_buffer.Get();
    }
    D3D12_VERTEX_BUFFER_VIEW GRLCDirectXBuffer::GetVertexBufferView(uint32_t stride)
    {
        D3D12_VERTEX_BUFFER_VIEW res;
        res.BufferLocation = m_buffer->GetGPUVirtualAddress();
        res.SizeInBytes = m_size;
        res.StrideInBytes = stride;
        return res;
    }

    D3D12_INDEX_BUFFER_VIEW GRLCDirectXBuffer::GetIndexBufferView()
    {
        D3D12_INDEX_BUFFER_VIEW res;
        res.BufferLocation = m_buffer->GetGPUVirtualAddress();
        res.SizeInBytes = m_size;
        res.Format = DXGI_FORMAT_R32_UINT;
        return res;
    }
    uint64_t GRLCDirectXBuffer::GetBufferSize()
    {
        return m_size;
    }
    
    GRL_RESULT GRLCDirectXBuffer::SetBufferData(void* pData, uint64_t data_size, uint64_t buffer_offset)
    {
        CD3DX12_RANGE range(0, 0);
        uint8_t* pDest;
        if (FAILED(
            GRLComCall(m_buffer->Map(0, &range, reinterpret_cast<void**>(&pDest)))
        ))
        {
            return GRL_TRUE;
        }
        pDest = pDest + buffer_offset;
        memcpy(pDest, pData, min(data_size, m_size - buffer_offset));
        m_buffer->Unmap(0, nullptr);
        return GRL_FALSE;
    }

    
    GRL_RESULT GRLCDirectXBuffer::AddRef()
    {
        __Ref_Cnt++;
        return GRL_FALSE;
    }

    GRL_RESULT GRLCDirectXBuffer::Release()
    {
        __Ref_Cnt--;
        if (!__Ref_Cnt)
            delete this;
        return GRL_FALSE;
    }

    GRL_RESULT GRLCDirectXBuffer::QueryInterface(GRLUUID uuid, void** ppObject)
    {
        return GRL_FALSE;
    }
    
    GRLCDirectXDepthMap::GRLCDirectXDepthMap(GRLCDirectXEnvironment* pEnvironment, uint32_t width, uint32_t height)
    {
        __Ref_Cnt = 1;
        m_width = width;
        m_height = height;

        __build_resource(pEnvironment->GetDevice());
    }

    ID3D12Resource* GRLCDirectXDepthMap::GetResource()
    {
        return m_depthMap.Get();
    }

    D3D12_CPU_DESCRIPTOR_HANDLE GRLCDirectXDepthMap::GetDSVHandle()
    {
        return m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
    }

    void GRLCDirectXDepthMap::Resize(uint32_t width, uint32_t height)
    {
        m_width = width;
        m_height = height;
        ComPtr<ID3D12Device> device;
        m_depthMap->GetDevice(IID_PPV_ARGS(&device));
        m_dsvHeap.Reset();
        m_depthMap.Reset();
        __build_resource(device.Get());
    }

    GRL_RESULT GRLCDirectXDepthMap::AddRef()
    {
        __Ref_Cnt++;
        return GRL_FALSE;
    }

    GRL_RESULT GRLCDirectXDepthMap::Release()
    {
        __Ref_Cnt--;
        if (!__Ref_Cnt)
            delete this;
        return GRL_FALSE;
    }
    GRL_RESULT GRLCDirectXDepthMap::QueryInterface(GRLUUID uuid, void** ppObject)
    {
        return GRL_TRUE;
    }
    void GRLCDirectXDepthMap::__build_resource(ID3D12Device * device)
    {
        CD3DX12_HEAP_PROPERTIES d3dx12_heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        CD3DX12_RESOURCE_DESC d3dx12_resource_desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, m_width, m_height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
        D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
        depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
        depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
        depthOptimizedClearValue.DepthStencil.Stencil = 0;
        GRLComCall(device->CreateCommittedResource(
            &d3dx12_heap_properties,
            D3D12_HEAP_FLAG_NONE,
            &d3dx12_resource_desc,
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &depthOptimizedClearValue,
            IID_PPV_ARGS(&m_depthMap)));

        D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};

        dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        dsvHeapDesc.NumDescriptors = 1;

        GRLComCall(
            device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsvHeap))
        );


        device->CreateDepthStencilView(m_depthMap.Get(), nullptr, m_dsvHeap->GetCPUDescriptorHandleForHeapStart());
    }
    GRLCDirectXTexture::GRLCDirectXTexture(GRLCDirectXEnvironment* pEnvironment, uint32_t width, uint32_t height, void* pData)
    {
        __Ref_Cnt = 1;
        m_width = width;
        m_height = height;
        ComPtr<ID3D12Device> device(pEnvironment->GetDevice());
        __build_resource(device.Get());
    }
    D3D12_CPU_DESCRIPTOR_HANDLE GRLCDirectXTexture::GetRTV()
    {
        return m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
    }
    D3D12_GPU_DESCRIPTOR_HANDLE GRLCDirectXTexture::GetSRV()
    {
        return m_srvHeap->GetGPUDescriptorHandleForHeapStart();
    }
    ID3D12DescriptorHeap* GRLCDirectXTexture::GetSRVHeap()
    {
        return m_srvHeap.Get();
    }
    ID3D12Resource* GRLCDirectXTexture::GetResource()
    {
        return m_texture.Get();
    }
    GRL_RESULT GRLCDirectXTexture::AddRef()
    {
        __Ref_Cnt++;
        return GRL_FALSE;
    }
    GRL_RESULT GRLCDirectXTexture::Release()
    {
        __Ref_Cnt--;
        if (!__Ref_Cnt)
            delete this;
        return GRL_FALSE;
    }
    GRL_RESULT GRLCDirectXTexture::QueryInterface(GRLUUID uuid, void** ppObject)
    {
        return GRL_FALSE;
    }
    void GRLCDirectXTexture::__build_resource(ID3D12Device* device)
    {
        CD3DX12_HEAP_PROPERTIES d3dx12_heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        CD3DX12_RESOURCE_DESC d3dx12_resource_desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT, m_width, m_height
        ,1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
        
        GRLComCall(
            device->CreateCommittedResource(
                &d3dx12_heap_properties,
                D3D12_HEAP_FLAG_NONE,
                &d3dx12_resource_desc,
                D3D12_RESOURCE_STATE_COPY_DEST,
                nullptr,
                IID_PPV_ARGS(&m_texture)
            ));
        

        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};

        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        rtvHeapDesc.NumDescriptors = 1;
        GRLComCall(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));

        D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};

        srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        srvHeapDesc.NumDescriptors = 1;
        GRLComCall(device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_srvHeap)));

        
        device->CreateRenderTargetView(m_texture.Get(), nullptr, m_rtvHeap->GetCPUDescriptorHandleForHeapStart());


        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = d3dx12_resource_desc.Format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;
        device->CreateShaderResourceView(m_texture.Get(), &srvDesc, m_srvHeap->GetCPUDescriptorHandleForHeapStart());
    }
}