#pragma once
#include "DXHeader.h"

using Microsoft::WRL::ComPtr;

namespace Grassland
{
	const int GRLFrameCount = 2;

	GRL_RESULT GRLDirectXSelectAdapter(IDXGIFactory * pFactory, IDXGIAdapter1 ** ppAdapter);

	class GRLCDirectXEnvironment: public GRLIBase
	{
	public:
		GRLCDirectXEnvironment(uint32_t screen_width, uint32_t screen_height, const char* window_title, bool full_screen);
		virtual GRL_RESULT PollEvents();
		virtual HWND GetHWnd();
		virtual ID3D12GraphicsCommandList* StartDraw();
		virtual void EndDraw();
		virtual ID3D12GraphicsCommandList* GetCommandList();
		virtual ID3D12Device* GetDevice();
		virtual void GetWindowSize(uint32_t* width, uint32_t* height);
		virtual void ClearBackFrameColor(float* color);
		virtual D3D12_CPU_DESCRIPTOR_HANDLE GetRTVHandle(int frameIndex);
		virtual D3D12_CPU_DESCRIPTOR_HANDLE GetBackFrameRTVHandle();
		virtual ID3D12Resource* GetFrameResource();
		virtual ID3D12Resource* GetFrameResource(int frameIndex);
		virtual GRL_RESULT AddRef();
		virtual GRL_RESULT Release();
	private:
		static LRESULT WINAPI GRLDirectXEnvironmentProcFunc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

		GRL_RESULT OnInit();
		GRL_RESULT LoadPipeline();

		GRL_RESULT __create_d3d12_device();
		GRL_RESULT __create_swapchain();
		GRL_RESULT __create_command_queue();
		GRL_RESULT __create_fence();

		void WaitForPreviousFrame();

		HWND m_hWnd;
		ComPtr<IDXGIFactory4> m_factory;
		ComPtr<ID3D12Device> m_device;
		ComPtr<IDXGISwapChain3> m_swapChain;
		ComPtr<ID3D12Resource> m_renderTargets[GRLFrameCount];
		ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
		ComPtr<ID3D12Fence> m_fence;
		CD3DX12_RECT m_scissorRect;
		CD3DX12_VIEWPORT m_viewport;
		ComPtr<ID3D12CommandQueue> m_commandQueue;
		ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		ComPtr<ID3D12GraphicsCommandList> m_commandList;
		HANDLE m_fenceEvent;
		uint32_t m_width, m_height;
		uint32_t m_frameIndex;
		uint64_t m_fenceValue;

		uint32_t __Ref_Cnt;
	};

	class GRLCBuffer : public GRLIBase
	{
	public:
		GRLCBuffer(GRLCDirectXEnvironment * pEnvironment, uint64_t size);
		virtual ID3D12Resource* GetResource();
		virtual D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView(uint32_t stride);
		virtual D3D12_INDEX_BUFFER_VIEW GetIndexBufferView();
		virtual uint64_t GetBufferSize();
		virtual void SetBufferData(void * pData, uint64_t data_size, uint64_t buffer_offset);
		virtual GRL_RESULT AddRef();
		virtual GRL_RESULT Release();
	private:
		ComPtr<ID3D12Resource> m_buffer;
		uint64_t m_size;
		uint32_t __Ref_Cnt;
	};

	class GRLCPipelineStateAndRootSignature : public GRLIBase
	{
	public:
		GRLCPipelineStateAndRootSignature(
			GRLCDirectXEnvironment * pEnvironment,
			const char * shaderFilePath,
			uint32_t numberRenderTarget, 
			DXGI_FORMAT * formats,
			uint32_t numberConstantBuffer,
			uint32_t numberTexture
			);
		virtual ID3D12RootSignature* GetRootSignature();
		virtual ID3D12PipelineState* GetPipelineState();
		virtual GRL_RESULT AddRef();
		virtual GRL_RESULT Release();
	private:
		ComPtr<ID3D12RootSignature> m_rootSignature;
		ComPtr<ID3D12PipelineState> m_pipelineState;
		uint32_t __Ref_Cnt;
	};
}
