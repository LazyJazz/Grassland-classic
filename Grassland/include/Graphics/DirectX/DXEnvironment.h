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
		virtual GRL_RESULT AddRef();
		virtual GRL_RESULT Release();
		virtual HWND GetHWnd();
		virtual ID3D12CommandList* StartDraw();
		virtual void EndDraw();
		void ClearBackFrameColor(float* color);
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
		uint32_t m_fenceValue;

		uint32_t __Ref_Cnt;
	};
}
