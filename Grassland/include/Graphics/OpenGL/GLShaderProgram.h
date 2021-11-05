#pragma once

#include "GLHeader.h"

namespace Grassland
{
	namespace Graphics
	{
		namespace OpenGL
		{
			enum GRL_OPENGL_SHADER_TYPE
			{
				GRL_OPENGL_SHADER_TYPE_VERTEX_SHADER = 0,
				GRL_OPENGL_SHADER_TYPE_FRAGMENT,
				GRL_OPENGL_SHADER_TYPE_GEOMETRY
			};

			class Shader
			{
			public:
				Shader();
				Shader(const char * shader_file_path, GRL_OPENGL_SHADER_TYPE shader_type);
				GRL_RESULT LoadShaderFromFile(const char* shader_file_path, GRL_OPENGL_SHADER_TYPE shader_type);
				GRL_OPENGL_SHADER_TYPE GetShaderType() const;
				uint32_t GetShaderHandle() const;
			private:
				uint32_t __shader_handle;
				GRL_OPENGL_SHADER_TYPE __shader_type;
			};

			class Program
			{
				;
			};
		}
	}

	class GLRIOpenGLShader : public GRLIBase
	{
	public:
	private:

	};

	class GLRIOpenGLProgram: public GRLIBase
	{
	public:
	private:
	};
}