#pragma once

#include "../Graphics.h"

#include "d3dx12.h"
#include <dxgi1_6.h>
#include <d3d12shader.h>
#include <d3d12sdklayers.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include "../../GrasslandDecl.h"
#include "../../String/String.h"
#include <map>

#include <iostream>

namespace Grassland
{
	const int GRLD3D12FrameCount = 2;

	class GRLCD3D12Environment;
	class GRLCD3D12Buffer;
	class GRLCD3D12Texture;
	class GRLCD3D12DepthMap;
	class GRLCD3D12PipelineState;

	GRLMakeObjectUUIDAssociate(GRLCD3D12Environment, GRLID_CD3D12Environment, "877751a2-5308-416e-9fee-c981d70964d5");
	GRLMakeObjectUUIDAssociate(GRLCD3D12Buffer, GRLID_CD3D12Buffer, "fc0f8ae9-520d-44c5-9d85-54c4066dd6f7");
	GRLMakeObjectUUIDAssociate(GRLCD3D12Texture, GRLID_CD3D12Texture, "7bb45f0c-5e72-459e-82a7-20c3160720d8");
	GRLMakeObjectUUIDAssociate(GRLCD3D12DepthMap, GRLID_CD3D12DepthMap, "ea8ca3e4-1203-4aa3-b86f-7a60f3e5ad3f");
	GRLMakeObjectUUIDAssociate(GRLCD3D12PipelineState, GRLID_CD3D12PipelineState, "70519a5b-9f40-491d-9bc4-ecd2676d966b");

	GRL_RESULT GRLD3D12SelectAdapter(IDXGIFactory* pFactory, IDXGIAdapter1** ppAdapter);

	DXGI_FORMAT GRLFormatToDXGIFormat(GRL_FORMAT grl_format);

	class GRLCD3D12Environment : public GRLIGraphicsEnvironment
	{
	public:
		GRLCD3D12Environment(uint32_t width, uint32_t height, const char* window_title);
		virtual GRL_RESULT PollEvents() override;
		virtual GRL_RESULT Resize(uint32_t width, uint32_t height) override;
		virtual GRL_RESULT CreateTexture(uint32_t width, uint32_t height, GRL_FORMAT format, GRLIGraphicsTexture** ppTexture) override;
		virtual GRL_RESULT CreateDepthMap(uint32_t width, uint32_t height, GRLIGraphicsDepthMap** ppDepthMap) override;
		virtual GRL_RESULT CreateBuffer(uint64_t size, GRLIGraphicsBuffer** ppBuffer) override;
		virtual GRL_RESULT CreatePipelineState(
			const char* shader_path,
			GRL_GRAPHICS_PIPELINE_STATE_DESC* desc,
			GRLIGraphicsPipelineState** ppPipelineState) override;
		virtual GRL_RESULT BeginDraw() override;
		virtual GRL_RESULT ApplyPipelineState(GRLIGraphicsPipelineState* pPipelineState) override;
		virtual GRL_RESULT SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		virtual GRL_RESULT SetConstantBuffer(uint32_t constantBufferIndex, GRLIGraphicsBuffer* constantBuffer) override;
		virtual GRL_RESULT SetTextures(uint32_t textureIndex, GRLIGraphicsTexture* pTexture) override;
		virtual GRL_RESULT SetRenderTargets(GRLIGraphicsTexture* pRenderTargetList, GRLIGraphicsDepthMap* pDepthMap) override;
		virtual GRL_RESULT SetInternalRenderTarget() override;
		virtual GRL_RESULT ClearRenderTargets(GRLColor color) override;
		virtual GRL_RESULT ClearDepthMap() override;
		virtual GRL_RESULT DrawInstance(
			GRLIGraphicsBuffer* pVertexBuffer,
			uint32_t numInstance,
			GRL_RENDER_TOPOLOGY renderTopology) override;
		virtual GRL_RESULT DrawIndexedInstance(
			GRLIGraphicsBuffer* pVertexBuffer,
			GRLIGraphicsBuffer* pIndexBuffer,
			uint32_t numInstance,
			GRL_RENDER_TOPOLOGY renderTopology) override;
		virtual GRL_RESULT EndDraw() override;
		virtual void WaitForGpu() override;
		virtual void Present(uint32_t enableInterval) override;
		virtual GRL_RESULT AddRef() override;
		virtual GRL_RESULT Release() override;
		virtual GRL_RESULT QueryInterface(GRLUUID Uuid, void** ppObject) override;

		ID3D12Device* GetDevice();
		ID3D12GraphicsCommandList* GetCommandList();
		ID3D12Resource* GetSwapChainFrameResource(int frameIndex);
		ID3D12Resource* GetSwapChainFrameResource();
		D3D12_CPU_DESCRIPTOR_HANDLE GetSwapChainRenderTargetViewHandle(int frameIndex);
		D3D12_CPU_DESCRIPTOR_HANDLE GetSwapChainRenderTargetViewHandle();

		virtual void MoveToNextFrame();
	private:

		static LRESULT WINAPI GRLD3D12EnvironmentProcFunc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

		GRL_RESULT OnInit();
		GRL_RESULT OnResize(uint32_t width, uint32_t height);
		GRL_RESULT LoadPipeline();

		GRL_RESULT __create_d3d12_device();
		GRL_RESULT __create_swapchain();
		GRL_RESULT __create_command_queue();
		GRL_RESULT __create_fence();
		GRL_RESULT __build_rtvResources();

		void WaitForPreviousFrame();

		HWND m_hWnd;
		ComPtr<IDXGIFactory4> m_factory;
		ComPtr<ID3D12Device> m_device;
		ComPtr<IDXGISwapChain3> m_swapChain;
		ComPtr<ID3D12Resource> m_renderTargets[GRLD3D12FrameCount];
		ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
		ComPtr<ID3D12Fence> m_fence;
		ComPtr<ID3D12CommandQueue> m_commandQueue;
		ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		ComPtr<ID3D12GraphicsCommandList> m_commandList;
		HANDLE m_fenceEvent;
		uint32_t m_width, m_height;
		uint32_t m_frameIndex;
		uint64_t m_fenceValues[GRLD3D12FrameCount];

		bool m_duringDraw;

		D3D12_CPU_DESCRIPTOR_HANDLE m_workingRTVHandleList[8];
		D3D12_CPU_DESCRIPTOR_HANDLE m_workingDSVHandle;
		uint32_t m_numWorkingRTV;
		bool m_isWorkingDSV;

		uint32_t __Ref_Cnt;
	};

	GRL_RESULT GRLCreateD3D12Environment(uint32_t width, uint32_t height, const char* window_title, GRLIGraphicsEnvironment ** ppEnvironment);

	class GRLCD3D12Buffer : public GRLIGraphicsBuffer
	{
	public:
		virtual GRL_RESULT WriteData(uint64_t size, uint64_t offset, void* pData) override;
		virtual GRL_RESULT AddRef() override;
		virtual GRL_RESULT Release() override;
		virtual GRL_RESULT QueryInterface(GRLUUID uuid, void** ppObject) override;
	private:
		uint32_t __Ref_Cnt;
	};

	class GRLCD3D12Texture : public GRLIGraphicsTexture
	{
	public:
		GRLCD3D12Texture(uint32_t width, uint32_t height, GRL_FORMAT format, GRLCD3D12Environment * pEnvironment);
		virtual GRL_RESULT WritePixels(void* pData) override;
		virtual GRL_RESULT ReadPixels(void* pData) override;
		virtual GRL_RESULT GetSize(uint32_t* pWidth, uint32_t* pHeight) override;
		virtual GRL_RESULT AddRef() override;
		virtual GRL_RESULT Release() override;
		virtual GRL_RESULT QueryInterface(GRLUUID uuid, void** ppObject) override;
	private:
		uint32_t __Ref_Cnt;
		uint32_t m_width, m_height;
		ComPtr<ID3D12Resource> m_texture;
		ComPtr<ID3D12Resource> m_uploadBuffer;
		ComPtr<ID3D12Resource> m_downloadBuffer;
		GRL_FORMAT m_format;
		DXGI_FORMAT m_dxgi_format;
	};

	class GRLCD3D12DepthMap : public GRLIGraphicsDepthMap
	{
	public:
		virtual GRL_RESULT GetSize(uint32_t* pWidth, uint32_t* pHeight) override;
		virtual GRL_RESULT AddRef() override;
		virtual GRL_RESULT Release() override;
		virtual GRL_RESULT QueryInterface(GRLUUID uuid, void** ppObject) override;
	private:
		uint32_t __Ref_Cnt;
	};

	class GRLCD3D12PipelineState : public GRLIGraphicsPipelineState
	{
	public:
		GRLCD3D12PipelineState(
			GRLCD3D12Environment * pEnvironment,
			const char* shader_path,
			GRL_GRAPHICS_PIPELINE_STATE_DESC* desc);
		virtual GRL_RESULT AddRef() override;
		virtual GRL_RESULT Release() override;
		virtual GRL_RESULT QueryInterface(GRLUUID Uuid, void** ppObject) override;
		virtual ID3D12RootSignature* GetRootSignature();
		virtual ID3D12PipelineState* GetPipelineState();
	private:
		ComPtr<ID3D12RootSignature> m_rootSignature;
		ComPtr<ID3D12PipelineState> m_pipelineState;
		uint32_t __Ref_Cnt;
	};
}