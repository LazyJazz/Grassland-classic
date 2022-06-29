#include "GLInput.h"

namespace Grassland {
uint32_t GRLOpenGLGetKey(uint32_t gl_key) {
  return glfwGetKey(GRLOpenGLGetWindow(), gl_key);
}

uint32_t GRLOpenGLKey2GRLKey(uint32_t gl_key) {
  if (gl_key >= GLFW_KEY_A && gl_key <= GLFW_KEY_Z)
    return GRL_KEY_A + gl_key - GLFW_KEY_A;
  if (gl_key >= GLFW_KEY_0 && gl_key <= GLFW_KEY_9)
    return GRL_KEY_0 + gl_key - GLFW_KEY_0;
  switch (gl_key) {
    case GLFW_KEY_ESCAPE:
      return GRL_KEY_ESCAPE;
    case GLFW_KEY_LEFT_SHIFT:
      return GRL_KEY_LSHIFT;
    case GLFW_KEY_RIGHT_SHIFT:
      return GRL_KEY_RSHIFT;
    case GLFW_KEY_LEFT_CONTROL:
      return GRL_KEY_LCONTROL;
    case GLFW_KEY_RIGHT_CONTROL:
      return GRL_KEY_RCONTROL;
    case GLFW_KEY_LEFT_ALT:
      return GRL_KEY_LALT;
    case GLFW_KEY_RIGHT_ALT:
      return GRL_KEY_RALT;
    case GLFW_KEY_LEFT_SUPER:
      return GRL_KEY_LWINDOWS;
    case GLFW_KEY_RIGHT_SUPER:
      return GRL_KEY_RWINDOWS;
    case GLFW_KEY_TAB:
      return GRL_KEY_TAB;
    case GLFW_KEY_ENTER:
      return GRL_KEY_ENTER;
    case GLFW_KEY_LEFT:
      return GRL_KEY_LEFT;
    case GLFW_KEY_UP:
      return GRL_KEY_UP;
    case GLFW_KEY_RIGHT:
      return GRL_KEY_RIGHT;
    case GLFW_KEY_DOWN:
      return GRL_KEY_DOWN;
  }
  return 0;
}

uint32_t GRLGRLKey2OpenGLKey(uint32_t grl_key) {
  if (grl_key >= GRL_KEY_A && grl_key <= GRL_KEY_Z)
    return GLFW_KEY_A + grl_key - GRL_KEY_A;
  if (grl_key >= GRL_KEY_0 && grl_key <= GRL_KEY_9)
    return GLFW_KEY_0 + grl_key - GRL_KEY_0;
  switch (grl_key) {
    case GRL_KEY_ESCAPE:
      return GLFW_KEY_ESCAPE;
    case GRL_KEY_LSHIFT:
      return GLFW_KEY_LEFT_SHIFT;
    case GRL_KEY_RSHIFT:
      return GLFW_KEY_RIGHT_SHIFT;
    case GRL_KEY_LCONTROL:
      return GLFW_KEY_LEFT_CONTROL;
    case GRL_KEY_RCONTROL:
      return GLFW_KEY_RIGHT_CONTROL;
    case GRL_KEY_LALT:
      return GLFW_KEY_LEFT_ALT;
    case GRL_KEY_RALT:
      return GLFW_KEY_RIGHT_ALT;
    case GRL_KEY_LWINDOWS:
      return GLFW_KEY_LEFT_SUPER;
    case GRL_KEY_RWINDOWS:
      return GLFW_KEY_RIGHT_SUPER;
    case GRL_KEY_TAB:
      return GLFW_KEY_TAB;
    case GRL_KEY_ENTER:
      return GLFW_KEY_ENTER;
    case GRL_KEY_LEFT:
      return GLFW_KEY_LEFT;
    case GRL_KEY_UP:
      return GLFW_KEY_UP;
    case GRL_KEY_RIGHT:
      return GLFW_KEY_RIGHT;
    case GRL_KEY_DOWN:
      return GLFW_KEY_DOWN;
  }
  return 0;
}

uint32_t GRLOpenGLMouseButton2GRLKey(uint32_t gl_mb) {
  switch (gl_mb) {
    case GLFW_MOUSE_BUTTON_LEFT:
      return GRL_KEY_LBUTTON;
    case GLFW_MOUSE_BUTTON_RIGHT:
      return GRL_KEY_RBUTTON;
    case GLFW_MOUSE_BUTTON_MIDDLE:
      return GRL_KEY_MBUTTON;
  }
  return 0;
}

GRLWinProc g_GRLWinProc;

void __GRLOpenGLCursorPosCallback(GLFWwindow *window,
                                  double xpos,
                                  double ypos) {
  g_GRLWinProc((uint64_t)window, GRL_WM_MOUSEMOVE, xpos, ypos);
}
void __GRLOpenGLWindowSizeCallback(GLFWwindow *window, int width, int height) {
  g_GRLWinProc((uint64_t)window, GRL_WM_SIZE, width, height);
}

void __GRLOpenGLKeyCallback(GLFWwindow *window,
                            int gl_key,
                            int scannode,
                            int action,
                            int mods) {
  switch (action) {
    case GLFW_PRESS:
      g_GRLWinProc((uint64_t)window, GRL_WM_KEYDOWN,
                   GRLOpenGLKey2GRLKey(gl_key), NULL);
      break;
    case GLFW_RELEASE:
      g_GRLWinProc((uint64_t)window, GRL_WM_KEYUP, GRLOpenGLKey2GRLKey(gl_key),
                   NULL);
      break;
    case GLFW_REPEAT:
      g_GRLWinProc((uint64_t)window, GRL_WM_KEYLAST,
                   GRLOpenGLKey2GRLKey(gl_key), NULL);
      break;
  }
}

void __GRLOpenGLMouseButtonCallback(GLFWwindow *window,
                                    int gl_mb,
                                    int action,
                                    int mods) {
  switch (action) {
    case GLFW_PRESS:
      g_GRLWinProc((uint64_t)window, GRL_WM_KEYDOWN,
                   GRLOpenGLMouseButton2GRLKey(gl_mb), NULL);
      break;
    case GLFW_RELEASE:
      g_GRLWinProc((uint64_t)window, GRL_WM_KEYUP,
                   GRLOpenGLMouseButton2GRLKey(gl_mb), NULL);
      break;
    case GLFW_REPEAT:
      g_GRLWinProc((uint64_t)window, GRL_WM_KEYLAST,
                   GRLOpenGLMouseButton2GRLKey(gl_mb), NULL);
      break;
  }
}

void GRLOpenGLSetWindowProc(GRLWinProc WinProc,
                            uint64_t param0,
                            uint64_t param1) {
  g_GRLWinProc = WinProc;
  glfwSetCursorPosCallback(GRLOpenGLGetWindow(), __GRLOpenGLCursorPosCallback);
  glfwSetWindowSizeCallback(GRLOpenGLGetWindow(),
                            __GRLOpenGLWindowSizeCallback);
  glfwSetKeyCallback(GRLOpenGLGetWindow(), __GRLOpenGLKeyCallback);
  glfwSetMouseButtonCallback(GRLOpenGLGetWindow(),
                             __GRLOpenGLMouseButtonCallback);
  WinProc((uint64_t)GRLOpenGLGetWindow(), GRL_WM_CREATE, param0, param1);
}
}  // namespace Grassland
