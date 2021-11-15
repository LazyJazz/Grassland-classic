#pragma once
#include "GLHeader.h"
#include <ft2build.h>
#include FT_FREETYPE_H

namespace Grassland
{
	GRL_RESULT GRLOpenGLTextInit();

	GRL_RESULT GRLOpenGLTextSetFont(const char* font_path, int32_t width, int32_t height);
}