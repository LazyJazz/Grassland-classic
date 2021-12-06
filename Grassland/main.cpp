#include <Grassland.h>

#include <Windows.h>
#include <d3d12.h>
#include <memory>

#include <iostream>
#include <thread>
#include <chrono>
#include <queue>

using Microsoft::WRL::ComPtr;

using namespace Grassland;

int main()
{
    SetConsoleOutputCP(936);

    GRLCDirectXEnvironment environment(1280, 720, "Grassland D3D12", false);
    //std::cout << (void*) & environment << std::endl;

    //ComPtr<ID3D12PipelineState> m_pipelineState;
    //ComPtr<ID3D12Device> m_device;
    //D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc = {};
    //pso_desc.DepthStencilState.DepthEnable = FALSE;
    //pso_desc.DepthStencilState.StencilEnable = FALSE;
    //m_device->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(&m_pipelineState));

    while (!environment.PollEvents())
    {
        float clearcolor[4] = { 0.6,0.7,0.8,1.0 };
        auto commandList = environment.StartDraw();
        environment.ClearBackFrameColor(clearcolor);
        environment.EndDraw();
    }
    return 0;
}