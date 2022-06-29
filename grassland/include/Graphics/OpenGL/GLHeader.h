#pragma once
#include "GLAD/glad.h"
#include "GLFW/glfw3.h"

#include "../Util/Util.h"
#include "../../GrasslandDecl.h"
#include "../../Math/Math.h"

#define GRLASSERT(x) if (!(x)) __debugbreak();
#define GRLGLCall(x) GRLOpenGLClearError();\
	x;\
	GRLASSERT(GRLOpenGLLogCall(#x, __FILE__, __LINE__))

static void GRLOpenGLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

static bool GRLOpenGLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[ OpenGL Error ] (" << error << ") " << function <<
			" " << file << ":" << line << std::endl;
		return false;
	}
	return true;
}