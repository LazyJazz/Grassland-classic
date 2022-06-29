#pragma once
#include "GL2DHeader.h"

namespace Grassland
{
	GRL_RESULT GRLOpenGL2DImageInit();

	GRL_RESULT GRLOpenGL2DPutImage_s(float x, float y, float width, float height, GRLIOpenGLTexture * pTexture);

	GRL_RESULT GRLOpenGL2DPutImage(float x, float y, float width, float height, GRLIOpenGLTexture* pTexture);

	GRL_RESULT GRLOpenGL2DPutImage(float x, float y, GRLIOpenGLTexture* pTexture);
}
