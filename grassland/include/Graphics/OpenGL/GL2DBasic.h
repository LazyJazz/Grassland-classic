#pragma once

#include "GL2DHeader.h"

namespace Grassland {
GRL_RESULT GRLOpenGL2DBasicInit();

GRL_RESULT GRLOpenGL2DSetDrawColor(GRLColor color);

GRLColor GRLOpenGL2DGetDrawColor();

GRL_RESULT GRLOpenGL2DDrawEllipse_s(float originx,
                                    float originy,
                                    float x_axis,
                                    float y_axis);

GRL_RESULT GRLOpenGL2DDrawEllipse(float originx,
                                  float originy,
                                  float x_axis,
                                  float y_axis);

GRL_RESULT GRLOpenGL2DDrawCircle(float originx, float originy, float radius);

GRL_RESULT GRLOpenGL2DDrawTriangle_s(float x0,
                                     float y0,
                                     float x1,
                                     float y1,
                                     float x2,
                                     float y2);

GRL_RESULT GRLOpenGL2DDrawTriangle(float x0,
                                   float y0,
                                   float x1,
                                   float y1,
                                   float x2,
                                   float y2);

GRL_RESULT GRLOpenGL2DDrawRectangle_s(float x0, float y0, float x1, float y1);

GRL_RESULT GRLOpenGL2DDrawRectangle(float x0, float y0, float x1, float y1);

}  // namespace Grassland
