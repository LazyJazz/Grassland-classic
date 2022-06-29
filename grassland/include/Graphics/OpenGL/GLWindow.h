#pragma once

#include "GLHeader.h"

namespace Grassland {
namespace Graphics {
namespace OpenGL {
GRL_RESULT Initilize(int32_t width,
                     int32_t height,
                     const char *window_title,
                     bool full_screen = false);

/*
@return Return the GLFW window handle, not suggested to use.
*/
GLFWwindow *GetGLFWWindow();

GRL_RESULT Terminate();

}  // namespace OpenGL
}  // namespace Graphics

GRL_RESULT GRLOpenGLInit(int32_t width,
                         int32_t height,
                         const char *window_title,
                         bool full_screen = false);

GRL_RESULT GRLOpenGLTerminate();

GRL_RESULT GRLOpenGLGetWindowSize(int *width, int *height);

GLFWwindow *GRLOpenGLGetWindow();

GRL_RESULT GRLOpenGLSetDepthTestState(uint32_t enable);

GRL_RESULT GRLOpenGLSetBlendState(uint32_t enable);

GRL_RESULT GRLOpenGLSetFaceCullState(uint32_t enable);

GRL_RESULT GRLOpenGLSetVSyncState(uint32_t enable);

GRL_RESULT GRLOpenGLSetClearColor(GRLColor color);

GRL_RESULT GRLOpenGLClear();

GRL_RESULT GRLOpenGLPollEvents();

bool GRLOpenGLShouldClose();

GRL_RESULT GRLOpenGLSwapBuffers();

}  // namespace Grassland
