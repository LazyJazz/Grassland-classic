#pragma once
#include "GLHeader.h"
#include "GLFrameBuffer.h"
#include "GLShaderProgram.h"
#include "GLVertexArray.h"
#include "GLWindow.h"
#include "../../String/String.h"
#include <ft2build.h>
#include <map>
#include FT_FREETYPE_H

namespace Grassland
{
	GRL_RESULT GRLOpenGLTextInit();

	GRL_RESULT GRLOpenGLTextInit(const char * ascii_font, const char * non_ascii_font, int32_t pixel_width, int32_t pixel_height);

	GRL_RESULT GRLOpenGLTextSetASCIIFont(const char* font_path);

	GRL_RESULT GRLOpenGLTextSetNonASCIIFont(const char* font_path);

	GRL_RESULT GRLOpenGLTextSetFont(const char* font_path);

	GRL_RESULT GRLOpenGLTextSetSize(uint32_t pixel_width, uint32_t pixel_height);

	GRL_RESULT GRLOpenGLTextRender(float x, float y, const wchar_t* wtext, int32_t length = -1);

	GRL_RESULT GRLOpenGLTextRender(float x, float y, std::wstring wtext);

	GRL_RESULT GRLOpenGLTextRender(float x, float y, std::string text);

	GRL_RESULT GRLOpenGLTextRender(float x, float y, const char* text);

	void GRLOpenGLTextSetColor(GRLColor color);

	GRLColor GRLOpenGLTextGetColor();

}