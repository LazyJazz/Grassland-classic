#include "DXWindow.h"

namespace Grassland
{
    LRESULT __stdcall GRLDirectXWinProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
    {
        UINT FileCount;
        switch (Msg)
        {
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
    GRL_RESULT GRLDirectXInit(int32_t width, int32_t height, const char* window_title, bool full_screen)
	{
        HINSTANCE hInstance = GetModuleHandle(nullptr);
        WNDCLASSEX windowClass = { 0 };
        windowClass.cbSize = sizeof(WNDCLASSEX);
        windowClass.style = CS_HREDRAW | CS_VREDRAW;
        windowClass.lpfnWndProc = GRLDirectXWinProc;
        windowClass.hInstance = hInstance;
        windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        windowClass.lpszClassName = L"GrasslandWindowClass";
        RegisterClassEx(&windowClass);

        RECT windowRect = { 0, 0, static_cast<LONG>(800), static_cast<LONG>(600) };
        AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

        // Create the window and store a handle to it.
        HWND hWnd = CreateWindow(
            windowClass.lpszClassName,
            L"Grassland Window",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            windowRect.right - windowRect.left,
            windowRect.bottom - windowRect.top,
            nullptr,        // We have no parent window.
            nullptr,        // We aren't using menus.
            hInstance,
            nullptr);

        ShowWindow(hWnd, SW_SHOW);
        DragAcceptFiles(hWnd, TRUE);
        return GRL_FALSE;
	}



	GRL_RESULT GRLDirectXPollEvent()
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
}