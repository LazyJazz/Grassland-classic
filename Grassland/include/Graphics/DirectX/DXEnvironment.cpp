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
    HWND GRLCDirectXEnvironment::GetHWnd()
    {
        return m_hWnd;
    }

    ID3D12CommandList* GRLCDirectXEnvironment::StartDraw()
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

        GRLComCall(m_swapChain->Present(1, 0));
        WaitForPreviousFrame();
    }

    void GRLCDirectXEnvironment::ClearBackFrameColor(float* color)
    {

        CD3DX12_RESOURCE_BARRIER rb0 = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
        m_commandList->ResourceBarrier(1, &rb0);

        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));

        m_commandList->ClearRenderTargetView(rtvHandle, color, 0, nullptr);


        CD3DX12_RESOURCE_BARRIER rb1 = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
        m_commandList->ResourceBarrier(1, &rb1);
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
        m_fenceValue = 1;

        m_fenceEvent = CreateEvent(0, 0, 0, 0);
        WaitForPreviousFrame();
        return GRL_FALSE;
    }
    
    void GRLCDirectXEnvironment::WaitForPreviousFrame()
    {
        const UINT64 fence = m_fenceValue;
        GRLComCall(m_commandQueue->Signal(m_fence.Get(), fence));
        m_fenceValue++;

        // Wait until the previous frame is finished.
        if (m_fence->GetCompletedValue() != fence)
        {
            GRLComCall(m_fence->SetEventOnCompletion(fence, m_fenceEvent));
            WaitForSingleObject(m_fenceEvent, INFINITE);
        }

        m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
    }
}