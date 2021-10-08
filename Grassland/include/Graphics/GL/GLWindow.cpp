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

			GRASSLAND_RESULT Initialize(int32_t width, int32_t height, const char * title, Window* window)
			{
				if (!glfwInit())
					return GRASSLAND_FALSE;
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
		}
	}
}