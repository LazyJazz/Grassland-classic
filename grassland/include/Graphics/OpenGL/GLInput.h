#pragma once
#include "GLHeader.h"
#include "GLWindow.h"
#include "../Util/Message.h"

namespace Grassland
{
	uint32_t GRLOpenGLGetKey(uint32_t gl_key);

	uint32_t GRLOpenGLKey2GRLKey(uint32_t gl_key);

	uint32_t GRLGRLKey2OpenGLKey(uint32_t grl_key);

	uint32_t GRLOpenGLMouseButton2GRLKey(uint32_t gl_mb);

	void GRLOpenGLSetWindowProc(GRLWinProc WinProc, uint64_t param0 = 0, uint64_t param1 = 0);
}