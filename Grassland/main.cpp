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

struct ConstantBufferConstant
{
    GRLMat4 mat;
};

#define CBPaddingSize(x) ((((x)+255) & 0xffffff00u)-(x))

template<typename _Ty>
struct ConstantBuffer
{
    _Ty content;
    char padding[CBPaddingSize(sizeof(_Ty))];
};

int main()
{
    SetConsoleOutputCP(936);

    GRLCDirectXEnvironment environment(1280, 720, "Grassland D3D12", false);
    DXGI_FORMAT formats[] = { DXGI_FORMAT_R8G8B8A8_UNORM };
    GRLCDirectXPipelineStateAndRootSignature *psoAndRootSignature = new GRLCDirectXPipelineStateAndRootSignature(
        &environment,
        "shaders\\DirectX\\shaders.hlsl",
        1,
        formats,
        1,
        0,
        true
    );

    Vertex vertices[] = {
        { {-0.5f, -0.5f,0.5f,1.0f}, {1.0f, 0.0f,0.0f,0.0f}, {0.0f, 0.0f,1.0f,1.0f}},
        { {-0.5f, 0.5f,0.5f,1.0f}, {1.0f, 0.0f,0.0f,0.0f}, {0.0f, 1.0f,1.0f,1.0f}},
        { {0.5f, -0.5f,0.5f,1.0f}, {1.0f, 0.0f,0.0f,0.0f}, {1.0f, 0.0f,1.0f,1.0f}},
        { {0.5f, 0.5f,0.5f,1.0f}, {1.0f, 0.0f,0.0f,0.0f}, {1.0f, 1.0f,1.0f,1.0f}},
        { {-0.5f, -0.5f,-0.5f,1.0f}, {1.0f, 0.0f,0.0f,0.0f}, {0.0f, 0.0f,0.0f,1.0f}},
        { {-0.5f, 0.5f,-0.5f,1.0f}, {1.0f, 0.0f,0.0f,0.0f}, {0.0f, 1.0f,0.0f,1.0f}},
        { {0.5f, -0.5f,-0.5f,1.0f}, {1.0f, 0.0f,0.0f,0.0f}, {1.0f, 0.0f,0.0f,1.0f}},
        { {0.5f, 0.5f,-0.5f,1.0f}, {1.0f, 0.0f,0.0f,0.0f}, {1.0f, 1.0f,0.0f,1.0f}}
    };

    uint32_t indices[] = {
        0,1,2,
        1,3,2,
        4,5,6,
        5,7,6,
        0,6,2,
        0,4,6,
        1,5,3,
        3,5,7,
        2,6,3,
        3,6,7,
        0,1,4,
        1,5,4
    };

    ConstantBuffer<ConstantBufferConstant> cb;


    GRLCDirectXBuffer* pVertexBuffer = new GRLCDirectXBuffer(&environment, sizeof(vertices));
    pVertexBuffer->SetBufferData(vertices, sizeof(vertices), 0);
    GRLCDirectXBuffer* pIndexBuffer= new GRLCDirectXBuffer(&environment, sizeof(indices));
    pIndexBuffer->SetBufferData(indices, sizeof(indices), 0);
    GRLCDirectXBuffer* pConstantBuffer = new GRLCDirectXBuffer(&environment, sizeof(cb));

    GRLCDirectXDepthMap* pDepthMap = new GRLCDirectXDepthMap(&environment, 1280, 720);


    //std::cout << (void*) & environment << std::endl;

    //ComPtr<ID3D12PipelineState> m_pipelineState;
    //ComPtr<ID3D12Device> m_device;
    //D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc = {};
    //pso_desc.DepthStencilState.DepthEnable = FALSE;
    //pso_desc.DepthStencilState.StencilEnable = FALSE;
    //m_device->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(&m_pipelineState));

    DirectX::XMFLOAT4 hsv(0.5, 1.0, 1.0, 1.0), rgba;
    float x = 0.0;
    GRLMat4 rot(1);
    while (!environment.PollEvents())
    {
        x += GRLRadian(0.5);
        //cb.content.offset = sinf(x);
        hsv.x += 0.001;
        DirectX::XMStoreFloat4(&rgba, DirectX::XMColorHSVToRGB(DirectX::XMLoadFloat4(&hsv)));
        //float clearcolor[4] = { rgba.x,rgba.y,rgba.z,1.0 };
        float clearcolor[4] = { 0.6,0.7,0.8,1.0 };
        ID3D12GraphicsCommandList * commandList = environment.StartDraw();
        commandList->SetPipelineState(psoAndRootSignature->GetPipelineState());
        commandList->SetGraphicsRootSignature(psoAndRootSignature->GetRootSignature());
        CD3DX12_RESOURCE_BARRIER resourceBarrier;

        uint32_t scr_width, scr_height;
        environment.GetWindowSize(&scr_width, &scr_height);
        std::cout << "[" << scr_width << ", " << scr_height << "]" << std::endl;
        CD3DX12_VIEWPORT viewPort(0.0f, 0.0f, (float)scr_width, (float)scr_height);
        CD3DX12_RECT scissorRect(0, 0, (LONG)scr_width, (LONG)scr_height);

        rot *= GRLTransformRotation(GRLRadian(0.1f), GRLRadian(0.2f), GRLRadian(0.3f));

        cb.content.mat =
            (GRLTransformProjection(GRLRadian(30.0f), (float)scr_width / (float)scr_height, 1.0f, 10.0f) *
                GRLTransformTranslate(0.0f, 0.0f, 5.0f) * rot).transpose();
        pConstantBuffer->SetBufferData(&cb, sizeof(cb), 0);

        commandList->RSSetScissorRects(1, &scissorRect);
        commandList->RSSetViewports(1, &viewPort);

        resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(environment.GetFrameResource(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
        commandList->ResourceBarrier(1, &resourceBarrier);

        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = environment.GetBackFrameRTVHandle();
        D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = pDepthMap->GetDSVHandle();

        commandList->OMSetRenderTargets(1, &rtvHandle, true, &dsvHandle);


        environment.ClearBackFrameColor(clearcolor);
        commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0, 0, 0, nullptr);

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        D3D12_VERTEX_BUFFER_VIEW vertexBufferView = pVertexBuffer->GetVertexBufferView(sizeof(Vertex));
        commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
        D3D12_INDEX_BUFFER_VIEW indexBufferView = pIndexBuffer->GetIndexBufferView();
        commandList->IASetIndexBuffer(&indexBufferView);

        commandList->SetGraphicsRootConstantBufferView(0, pConstantBuffer->GetResource()->GetGPUVirtualAddress());

        //commandList->DrawInstanced(3, 1, 0, 0);
        commandList->DrawIndexedInstanced(36, 1, 0, 0, 0);

        resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(environment.GetFrameResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
        commandList->ResourceBarrier(1, &resourceBarrier);

        environment.EndDraw();
    }

    pVertexBuffer->Release();
    psoAndRootSignature->Release();
    return 0;
}