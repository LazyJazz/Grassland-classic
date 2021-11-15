#pragma once
#include "GLHeader.h"
#include <ft2build.h>
#include FT_FREETYPE_H

namespace Grassland
{
	namespace Graphics
	{
		namespace OpenGL
		{
			;
		}
	}

	void GRLOpenGLTextSetFont(const char * font_path);

	void GRLOpenGLTextSetSize();
}