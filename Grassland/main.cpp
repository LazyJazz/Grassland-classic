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
    DirectX::XMFLOAT4 position;
    DirectX::XMFLOAT4 normal;
    DirectX::XMFLOAT4 texcoord;
};

int main()
{
    SetConsoleOutputCP(936);

    GRLCDirectXEnvironment environment(1280, 720, "Grassland D3D12", false);
    DXGI_FORMAT formats[] = { DXGI_FORMAT_R8G8B8A8_UNORM };
    GRLCPipelineStateAndRootSignature *psoAndRootSignature = new GRLCPipelineStateAndRootSignature(
        &environment,
        "shaders\\DirectX\\shaders.hlsl",
        1,
        formats,
        0,
        0
    );

    Vertex vertices[] = {
        { {0.0f, 0.25f,0.0f,1.0f}, {1.0f, 0.0f,0.0f,0.0f}, {1.0f, 0.0f,0.0f,1.0f}},
        { {0.25f, -0.25f,0.0f,1.0f}, {1.0f, 0.0f,0.0f,0.0f}, {0.0f, 1.0f,0.0f,1.0f}},
        { {-0.25f, -0.25f,0.0f,1.0f}, {1.0f, 0.0f,0.0f,0.0f}, {0.0f, 0.0f,1.0f,1.0f}}
    };

    GRLCBuffer* pBuffer = new GRLCBuffer(&environment, sizeof(vertices));
    pBuffer->SetBufferData(vertices, sizeof(vertices), 0);

    //std::cout << (void*) & environment << std::endl;

    //ComPtr<ID3D12PipelineState> m_pipelineState;
    //ComPtr<ID3D12Device> m_device;
    //D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc = {};
    //pso_desc.DepthStencilState.DepthEnable = FALSE;
    //pso_desc.DepthStencilState.StencilEnable = FALSE;
    //m_device->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(&m_pipelineState));

    DirectX::XMFLOAT4 hsv(0.5, 1.0, 1.0, 1.0), rgba;
    while (!environment.PollEvents())
    {
        hsv.x += 0.001;
        DirectX::XMStoreFloat4(&rgba, DirectX::XMColorHSVToRGB(DirectX::XMLoadFloat4(&hsv)));
        float clearcolor[4] = { rgba.x,rgba.y,rgba.z,1.0 };
        ID3D12GraphicsCommandList * commandList = environment.StartDraw();
        commandList->SetPipelineState(psoAndRootSignature->GetPipelineState());
        commandList->SetGraphicsRootSignature(psoAndRootSignature->GetRootSignature());
        CD3DX12_RESOURCE_BARRIER resourceBarrier;

        uint32_t scr_width, scr_height;
        environment.GetWindowSize(&scr_width, &scr_height);
        std::cout << "[" << scr_width << ", " << scr_height << "]" << std::endl;
        CD3DX12_VIEWPORT viewPort(0.0f, 0.0f, (float)scr_width, (float)scr_height);
        CD3DX12_RECT scissorRect(0, 0, (LONG)scr_width, (LONG)scr_height);


        commandList->RSSetScissorRects(1, &scissorRect);
        commandList->RSSetViewports(1, &viewPort);

        resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(environment.GetFrameResource(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
        commandList->ResourceBarrier(1, &resourceBarrier);

        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = environment.GetBackFrameRTVHandle();

        commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);


        environment.ClearBackFrameColor(clearcolor);

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        D3D12_VERTEX_BUFFER_VIEW vertexBufferView = pBuffer->GetVertexBufferView(sizeof(Vertex));
        commandList->IASetVertexBuffers(0, 1, &vertexBufferView);

        commandList->DrawInstanced(3, 1, 0, 0);

        resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(environment.GetFrameResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
        commandList->ResourceBarrier(1, &resourceBarrier);

        environment.EndDraw();
    }

    pBuffer->Release();
    psoAndRootSignature->Release();
    return 0;
}