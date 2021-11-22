#pragma once
#include "DXHeader.h"

namespace Grassland
{

	LRESULT WINAPI GRLDirectXWinProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

	GRL_RESULT GRLDirectXInit(int32_t width, int32_t height, const char* window_title, bool full_screen = false);

	GRL_RESULT GRLDirectXPollEvent();

	HWND GRLDirectXGetHWnd();
}
