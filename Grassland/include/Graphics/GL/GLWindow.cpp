#include "GLWindow.h"

namespace Grassland
{
	namespace Graphics
	{
		namespace GL
		{

			Window::Window()
			{
				pWnd = NULL;
			}

			GLFWwindow*& Window::GetGLFWWindowHandle()
			{
				return pWnd;
			}

			int32_t Window::ShouldClose() const
			{
				return glfwWindowShouldClose(pWnd);
			}

			void Window::Present() const
			{
				glfwSwapBuffers(pWnd);
			}

			GRASSLAND_RESULT Initialize(int32_t width, int32_t height, const char * title, Window* window, int32_t MSScale)
			{
				if (!glfwInit())
					return GRASSLAND_FALSE;
				glfwWindowHint(GLFW_SAMPLES, MSScale);
				window->GetGLFWWindowHandle() = glfwCreateWindow(width, height, title, nullptr, nullptr);
				if (!window->GetGLFWWindowHandle())
				{
					glfwTerminate();
					return GRASSLAND_FALSE;
				}

				glfwMakeContextCurrent(window->GetGLFWWindowHandle());

				if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
				{
					glfwTerminate();
					return GRASSLAND_FALSE;
				}

				glViewport(0, 0, width, height);

				return GRASSLAND_TRUE;
			}
			void SetClearColor(float red, float green, float blue, float alpha)
			{
				glClearColor(red, green, blue, alpha);
			}
			void SetDepthTestState(bool enable)
			{
				if (enable) glEnable(GL_DEPTH_TEST);
				else glDisable(GL_DEPTH_TEST);
			}
			void ClearColorBuffer()
			{
				glClear(GL_COLOR_BUFFER_BIT);
			}
			void ClearDepthBuffer()
			{
				glClear(GL_DEPTH_BUFFER_BIT);
			}
			void SetMultiSampleState(bool enable)
			{
				if (enable)glEnable(GL_MULTISAMPLE);
				else glDisable(GL_MULTISAMPLE);
			}
			void SetBlendState(bool enable)
			{
				if (enable) glEnable(GL_BLEND);
				else glDisable(GL_BLEND);
			}
		}
	}
}