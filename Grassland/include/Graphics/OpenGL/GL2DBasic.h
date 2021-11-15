#pragma once

#include "GL2DHeader.h"

namespace Grassland
{
	GRL_RESULT GRLOpenGL2DBasicInit();

	GRL_RESULT GRLOpenGL2DSetDrawColor(GRLColor color);

	GRLColor GRLOpenGL2DGetDrawColor();

	GRL_RESULT GRLOpenGL2DDrawCircle(float originx, float originy, float radius);

	GRL_RESULT GRLOpenGL2DDrawRectangle(float left_down_x, float left_down_y, float right_down_x, float right_down_y);

	GRL_RESULT GRLOpenGL2DDrawTriangle(float x0, float y0, float x1, float y1, float x2, float y2);
}