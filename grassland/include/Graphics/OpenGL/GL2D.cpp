#include "GL2D.h"

namespace Grassland {
GRL_RESULT Grassland::GRLOpenGL2DInit() {
  if (GRLOpenGL2DBasicInit())
    return GRL_TRUE;
  if (GRLOpenGL2DImageInit())
    return GRL_TRUE;
  return GRL_FALSE;
}
}  // namespace Grassland
