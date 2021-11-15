#pragma once
#include "GL2DHeader.h"

namespace Grassland
{
	GRL_RESULT GRLOpenGL2DImageInit();

	GRL_RESULT GRLOpenGL2DPutImage(float x, float y, float width, float height, GRLIOpenGLTexture * pTexture);

	GRL_RESULT GRLOpenGL2DPutImage(int32_t x, int32_t y, int32_t width, int32_t height, GRLIOpenGLTexture* pTexture);

	GRL_RESULT GRLOpenGL2DPutImage(int32_t x, int32_t y, GRLIOpenGLTexture* pTexture);
}
