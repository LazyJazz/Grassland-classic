#pragma once
#include "GLHeader.h"

namespace Grassland
{
	namespace Graphics
	{
		namespace GL
		{
			class Window
			{
			private:
				GLFWwindow* pWnd;
			public:
				Window();
				GLFWwindow*& GetGLFWWindowHandle();
				int ShouldClose() const;
			};

			/*! @brief Initialize OpenGL environment.
			* This function initializes OpenGL environment through GLFW and GLAD libraries,
			* it will also start a main window with given settings since OpenGL context needs a window.
			* 
			* @param[in] width The main window width.
			* 
			* @param[in] height The main window height.
			* 
			* @param[in] title The main window title.
			* 
			* @param[out] window The pointer to object that contains created window.
			* 
			* @return If succeed, return GRASSLAND_TRUE, otherwise, return GRASSLAND_FALSE
			*/
			GRASSLAND_RESULT Initialize(int32_t width, int32_t height, const char* title, Window* window);
		}
	}
}