#include "GLWindow.h"

namespace Grassland
{
	namespace Graphics
	{
		namespace OpenGL
		{
			GLFWwindow* g_pGLFWwindow = nullptr;
			;
			GRL_RESULT Initilize(int32_t width, int32_t height, const char * window_title, bool full_screen)
			{
				if (!glfwInit())
				{
					GRLSetErrorInfo("glfwInit Failed");
					return GRL_TRUE;
				}
				g_pGLFWwindow = glfwCreateWindow(width, height, window_title, full_screen ? glfwGetPrimaryMonitor() : nullptr, nullptr);
				if (!g_pGLFWwindow)
				{
					glfwTerminate();
					GRLSetErrorInfo("glfwCreateWindow Failed");
					return GRL_TRUE;
				}

				glfwMakeContextCurrent(g_pGLFWwindow);
				if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
				{
					glfwTerminate();
					GRLSetErrorInfo("glfwCreateWindow Failed");
					return GRL_TRUE;
				}

				glViewport(0, 0, width, height);

				return GRL_FALSE;
			}

			GLFWwindow* GetGLFWWindow()
			{
				return g_pGLFWwindow;
			}

			GRL_RESULT Terminate()
			{
				glfwTerminate();
				return GRL_FALSE;
			}
		}
	}

	GRL_RESULT GRLOpenGLInit(int32_t width, int32_t height, const char* window_title, bool full_screen)
	{
		return Graphics::OpenGL::Initilize(width, height, window_title, full_screen);
	}

	GRL_RESULT GRLOpenGLTerminate()
	{
		return Graphics::OpenGL::Terminate();
	}
	GRL_RESULT GRLOpenGLGetWindowSize(int* width, int* height)
	{
		glfwGetWindowSize(Graphics::OpenGL::GetGLFWWindow(), width, height);
		return GRL_FALSE;
	}

	GLFWwindow* GRLOpenGLGetWindow()
	{
		return Graphics::OpenGL::GetGLFWWindow();
	}
}
