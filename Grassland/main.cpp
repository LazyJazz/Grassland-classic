#include <Grassland.h>

#include <Windows.h>
#include <d3d12.h>
#include <memory>

#include <iostream>
#include <thread>
#include <chrono>
#include <queue>

#include <DirectXMath.h>

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

void GRLGraphicsAPIFullTest(GRL_GRAPHICS_API);

int aplusb(int a, int b)
{
    return a + b;
}

template <class Func, class ... Args>
void SimulateFuncCall(Func && func, Args && ...args)
{
    std::cout << func(args...);
}

#define MacroFuncCall(func, ...) SimulateFuncCall(func, __VA_ARGS__)

int main()
{
    GRLGraphicsAPIFullTest(GRL_GRAPHICS_API::D3D12);
    return 0;
    std::thread th[2];
    th[0] = std::thread(GRLGraphicsAPIFullTest, GRL_GRAPHICS_API::D3D12);
    th[1] = std::thread(GRLGraphicsAPIFullTest, GRL_GRAPHICS_API::OPENGL);
    th[0].join();
    th[1].join();
    return 0;
}

void GRLGraphicsAPIFullTest(GRL_GRAPHICS_API graphics_api)
{
    Vertex vertices[24];
    uint32_t indices[36];
    ConstantBufferConstant constantBuffer;

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
    };
    ConstantBufferConstant constantBufferTex;

    SetProcessDPIAware();
    SetConsoleOutputCP(936);
    GRLPtr<GRLIGraphicsEnvironment> pEnvironment;
    GRLPtr<GRLIGraphicsPipelineState> pPipelineState;
    GRLPtr<GRLIGraphicsPipelineState> pPipelineStateTex;
    GRLPtr<GRLIGraphicsTexture> pTexture, pTextureBitmap;
    GRLPtr<GRLIGraphicsDepthMap> pDepthMapTex;


    GRLPtr<GRLIGraphicsBuffer> pVertexBufferTex;
    GRLPtr<GRLIGraphicsBuffer> pIndexBufferTex;
    GRLPtr<GRLIGraphicsBuffer> pConstantBufferTex;

    GRLPtr<GRLIGraphicsBuffer> pVertexBuffer;
    GRLPtr<GRLIGraphicsBuffer> pIndexBuffer;
    GRLPtr<GRLIGraphicsBuffer> pConstantBuffer;

    GRLCreateGraphicsEnvironment(1280, 720, "Grassland Graphics", graphics_api, &pEnvironment);

    GRL_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc;
    GRL_FORMAT inputElementLayout[] = { GRL_FORMAT::FLOAT4,GRL_FORMAT::FLOAT4 ,GRL_FORMAT::FLOAT4 };
    GRL_GRAPHICS_SAMPLER_DESC samplerDescs[] = {
        {GRL_GRAPHICS_SAMPLER_FILTER::POINT, GRL_GRAPHICS_TEXTURE_EXTENSION_MODE::CLAMP, GRL_GRAPHICS_TEXTURE_EXTENSION_MODE::CLAMP, GRL_GRAPHICS_TEXTURE_EXTENSION_MODE::CLAMP},
        {GRL_GRAPHICS_SAMPLER_FILTER::POINT, GRL_GRAPHICS_TEXTURE_EXTENSION_MODE::CLAMP, GRL_GRAPHICS_TEXTURE_EXTENSION_MODE::CLAMP, GRL_GRAPHICS_TEXTURE_EXTENSION_MODE::CLAMP}
    };
    pipelineStateDesc.enableBlend = 0;
    pipelineStateDesc.enableDepthTest = 1;
    pipelineStateDesc.cullFace = 0;
    pipelineStateDesc.inputElementLayout = inputElementLayout;
    pipelineStateDesc.numInputElement = 3;
    pipelineStateDesc.numConstantBuffer = 1;
    pipelineStateDesc.numRenderTargets = 1;
    pipelineStateDesc.numTexture = 2;
    pipelineStateDesc.samplerDesc = samplerDescs;
    pipelineStateDesc.renderTargetFormatsList[0] = GRL_FORMAT::BYTE4;
    pEnvironment->CreatePipelineState("shaders/cube_shader", &pipelineStateDesc, &pPipelineState);
    pipelineStateDesc.enableBlend = 0;
    pipelineStateDesc.enableDepthTest = 1;
    pipelineStateDesc.cullFace = 0;
    pipelineStateDesc.inputElementLayout = inputElementLayout;
    pipelineStateDesc.numInputElement = 3;
    pipelineStateDesc.numConstantBuffer = 1;
    pipelineStateDesc.numRenderTargets = 1;
    pipelineStateDesc.numTexture = 2;
    pipelineStateDesc.samplerDesc = samplerDescs;
    pipelineStateDesc.renderTargetFormatsList[0] = GRL_FORMAT::FLOAT4;
    pEnvironment->CreatePipelineState("shaders/cube_shader", &pipelineStateDesc, &pPipelineStateTex);
    //delete [] pipelineStateDesc.inputElementLayout;
    pEnvironment->CreateTexture(32, 32, GRL_FORMAT::FLOAT4, &pTexture);
    pEnvironment->CreateBuffer(sizeof(vertices), GRL_GRAPHICS_BUFFER_TYPE::VERTEX, &pVertexBuffer);
    pEnvironment->CreateBuffer(sizeof(indices), GRL_GRAPHICS_BUFFER_TYPE::INDEX, &pIndexBuffer);
    pEnvironment->CreateBuffer(sizeof(constantBuffer), GRL_GRAPHICS_BUFFER_TYPE::CONSTANT, &pConstantBuffer);
    pEnvironment->CreateBuffer(sizeof(verticesTex), GRL_GRAPHICS_BUFFER_TYPE::VERTEX, &pVertexBufferTex);
    pEnvironment->CreateBuffer(sizeof(indicesTex), GRL_GRAPHICS_BUFFER_TYPE::INDEX, &pIndexBufferTex);
    pEnvironment->CreateBuffer(sizeof(constantBuffer), GRL_GRAPHICS_BUFFER_TYPE::CONSTANT, &pConstantBufferTex);
    pEnvironment->CreateDepthMap(32, 32, &pDepthMapTex);

    pEnvironment->CreateTexture(256, 256, GRL_FORMAT::FLOAT4, &pTextureBitmap);
    GRLColor* pData = new GRLColor[256 * 256];
    for (int x = 0; x < 256; x++)
    {
        for (int y = 0; y < 256; y++)
        {
            pData[x + y * 256] = GRLColor(x, y, x ^ y);
        }
    }
    pTextureBitmap->WritePixels(pData);

    pVertexBuffer->WriteData(sizeof(vertices), 0, vertices);
    pIndexBuffer->WriteData(sizeof(indices), 0, indices);
    pVertexBufferTex->WriteData(sizeof(verticesTex), 0, verticesTex);
    pIndexBufferTex->WriteData(sizeof(indicesTex), 0, indicesTex);

    GRLMat4 rot(1.0), rotTex(1.0);

    std::queue<std::chrono::steady_clock::time_point> qts;

    while (!pEnvironment->PollEvents())
    {
        std::chrono::steady_clock::time_point tp = std::chrono::steady_clock::now();
        qts.push(tp);
        if (qts.size() == 1000)
        {
            std::chrono::steady_clock::time_point fp = qts.front();
            std::cout << (999e9f / (float)((tp - fp) / std::chrono::nanoseconds(1))) << std::endl;
            while (!qts.empty())
                qts.pop();
        }
        uint32_t scrWidth, scrHeight;
        pEnvironment->GetSize(&scrWidth, &scrHeight);

        rot *= GRLTransformRotation(GRLRadian(0.03f), GRLRadian(0.02f), GRLRadian(0.01f));
        rotTex *= GRLTransformRotation(GRLRadian(0.1f), GRLRadian(0.2f), GRLRadian(0.3f));

        constantBuffer.mat =
            (GRLTransformProjection(GRLRadian(30.0f), 1.0f, 1.0f, 10.0f) *
                GRLTransformTranslate(0.0f, 0.0f, 5.0f) * rotTex).transpose();
        constantBuffer.mode = 0;
        pConstantBufferTex->WriteData(sizeof(constantBuffer), 0, &constantBuffer);
        constantBuffer.mat =
            (GRLTransformProjection(GRLRadian(30.0f), (float)scrWidth / (float)scrHeight, 1.0f, 10.0f) *
                GRLTransformTranslate(0.0f, 0.0f, 5.0f) * rot).transpose();
        constantBuffer.mode = 1;
        pConstantBuffer->WriteData(sizeof(constantBuffer), 0, &constantBuffer);

        pEnvironment->BeginDraw();
        pEnvironment->ApplyPipelineState(pPipelineStateTex.Get());
        {
            GRLIGraphicsTexture* renderTargets[] = { pTexture.Get() };
            if (pEnvironment->SetRenderTargets(1, renderTargets, pDepthMapTex.Get()))
                GRLSetErrorInfo("Set RenderTargets Failed.");
        }
        pEnvironment->SetViewport(0, 0, 32, 32);
        pEnvironment->ClearRenderTargets(GRLColor(0.8, 0.7, 0.6, 1.0));
        pEnvironment->ClearDepthMap();
        pEnvironment->SetConstantBuffer(0, pConstantBufferTex.Get());
        pEnvironment->DrawIndexedInstance(pVertexBufferTex.Get(), pIndexBufferTex.Get(), 36, GRL_RENDER_TOPOLOGY::TRIANGLE);
        
        
        pEnvironment->ApplyPipelineState(pPipelineState.Get());
        pEnvironment->SetInternalRenderTarget();
        pEnvironment->SetViewport(0, 0, scrWidth, scrHeight);
        pEnvironment->ClearRenderTargets(GRLColor(0.6, 0.7, 0.8, 1.0));
        pEnvironment->ClearDepthMap();
        GRLIGraphicsTexture* bindTextures[] = { pTexture.Get(), pTextureBitmap.Get() };
        pEnvironment->SetTextures(2, bindTextures);
        pEnvironment->SetConstantBuffer(0, pConstantBuffer.Get());
        pEnvironment->DrawIndexedInstance(pVertexBuffer.Get(), pIndexBuffer.Get(), 36, GRL_RENDER_TOPOLOGY::TRIANGLE);
        pEnvironment->EndDraw();
        pEnvironment->Present(0);
        /*
        {
            GRLIImage * pImage;
            GRLCreateImage(32, 32, &pImage);
            GRLColor* pColor;
            pImage->GetImageBuffer(&pColor);
            pTexture->ReadPixels(pColor);
            pColor = nullptr;
            pImage->StoreBMP("texture.bmp");
            pImage->Release();
        }//*/
    }
    return;
}