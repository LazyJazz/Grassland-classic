#pragma once

#include "GLHeader.h"
#include <map>
#include <string>

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
				GRL_RESULT CompileShaderFromFile(const char* shader_file_path, GRL_OPENGL_SHADER_TYPE shader_type);
				GRL_OPENGL_SHADER_TYPE GetShaderType() const;
				uint32_t GetShaderHandle() const;
				uint32_t Clear();
			private:
				uint32_t __shader_handle;
				GRL_OPENGL_SHADER_TYPE __shader_type;
			};

			class Program
			{
			public:
				Program();
				~Program();
				GRL_RESULT LinkShaders(
					Shader* vertex_shader,
					Shader* fragment_shader,
					Shader* geometry_shader = NULL
				);
				void Clear();
				int32_t GetUniformLocation(const char* uniform_name);
				void Use() const;
				void SetFloat(int32_t location, float fvalue);
				void SetVec2(int32_t location, const GRLVec2& vec);
				void SetVec3(int32_t location, const GRLVec3& vec);
				void SetVec4(int32_t location, const GRLVec4& vec);
				void SetMat2(int32_t location, const GRLMat2& mat);
				void SetMat3(int32_t location, const GRLMat3& mat);
				void SetMat4(int32_t location, const GRLMat4& mat);
				void SetFloat(const char* uniform_name, float fvalue);
				void SetVec2(const char* uniform_name, const GRLVec2& vec);
				void SetVec3(const char* uniform_name, const GRLVec3& vec);
				void SetVec4(const char* uniform_name, const GRLVec4& vec);
				void SetMat2(const char* uniform_name, const GRLMat2& mat);
				void SetMat3(const char* uniform_name, const GRLMat3& mat);
				void SetMat4(const char* uniform_name, const GRLMat4& mat);
			private:
				uint32_t __program_handle;
				std::map<std::string, int32_t> __uniform_location_map;
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