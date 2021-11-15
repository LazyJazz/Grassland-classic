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
	GRL_RESULT GRLOpenGLSetDepthTestState(uint32_t enable)
	{
		if (enable)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
		return GRL_FALSE;
	}
	GRL_RESULT GRLOpenGLSetBlendState(uint32_t enable)
	{
		if (enable)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		else
			glDisable(GL_BLEND);
		return GRL_FALSE;
	}
	GRL_RESULT GRLOpenGLSetFaceCullState(uint32_t enable)
	{
		if (enable)
		{
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
		}
		else
			glDisable(GL_CULL_FACE);
		return GRL_FALSE;
	}
	GRL_RESULT GRLOpenGLSetVSyncState(uint32_t enable)
	{
		glfwSwapInterval(enable);
		return GRL_FALSE;
	}

	GRL_RESULT GRLOpenGLSetClearColor(GRLColor color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
		return GRL_FALSE;
	}
	GRL_RESULT GRLOpenGLClear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		return GRL_FALSE;
	}

	GRL_RESULT GRLOpenGLPollEvents()
	{
		glfwPollEvents();
		return GRL_FALSE;
	}
	bool GRLOpenGLShouldClose()
	{
		return glfwWindowShouldClose(GRLOpenGLGetWindow());
	}
	GRL_RESULT GRLOpenGLSwapBuffers()
	{
		glfwSwapBuffers(GRLOpenGLGetWindow());
		return GRL_FALSE;
	}
}
