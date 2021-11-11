#pragma once

#include "GLHeader.h"
#include <map>
#include <string>

namespace Grassland
{
	typedef int32_t GRL_OPENGL_SHADER_TYPE;
#define GRL_OPENGL_SHADER_TYPE_NONE 0
#define GRL_OPENGL_SHADER_TYPE_VERTEX 1
#define GRL_OPENGL_SHADER_TYPE_FRAGMENT 2
#define GRL_OPENGL_SHADER_TYPE_GEOMETRY 3

	namespace Graphics
	{
		namespace OpenGL
		{

			class Shader
			{
			public:
				Shader();
				Shader(const char * shader_file_path, GRL_OPENGL_SHADER_TYPE shader_type);
				~Shader();
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
				GRL_RESULT LinkShaders(
					uint32_t __vertex_shader_handle,
					uint32_t __fragment_shader_handle,
					uint32_t __geometry_shader_handle = 0
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

	class GRLIOpenGLShader : public GRLIBase
	{
	public:
		virtual GRL_RESULT GetShaderType(GRL_OPENGL_SHADER_TYPE *type) const = 0;
		virtual GRL_RESULT GetShaderHandle(uint32_t* __shader_handle_pnter) const = 0;
	};

	class GRLIOpenGLProgram: public GRLIBase
	{
	public:
		virtual int32_t GetUniformLocation(const char* uniform_name) const = 0;
		virtual GRL_RESULT Use() const = 0;
		virtual GRL_RESULT SetFloat(int32_t location, float fvalue) const = 0;
		virtual GRL_RESULT SetVec2(int32_t location, const GRLVec2& vec) const = 0;
		virtual GRL_RESULT SetVec3(int32_t location, const GRLVec3& vec) const = 0;
		virtual GRL_RESULT SetVec4(int32_t location, const GRLVec4& vec) const = 0;
		virtual GRL_RESULT SetMat2(int32_t location, const GRLMat2& mat) const = 0;
		virtual GRL_RESULT SetMat3(int32_t location, const GRLMat3& mat) const = 0;
		virtual GRL_RESULT SetMat4(int32_t location, const GRLMat4& mat) const = 0;
		virtual GRL_RESULT SetFloat(const char* uniform_name, float fvalue) const = 0;
		virtual GRL_RESULT SetVec2(const char* uniform_name, const GRLVec2& vec) const = 0;
		virtual GRL_RESULT SetVec3(const char* uniform_name, const GRLVec3& vec) const = 0;
		virtual GRL_RESULT SetVec4(const char* uniform_name, const GRLVec4& vec) const = 0;
		virtual GRL_RESULT SetMat2(const char* uniform_name, const GRLMat2& mat) const = 0;
		virtual GRL_RESULT SetMat3(const char* uniform_name, const GRLMat3& mat) const = 0;
		virtual GRL_RESULT SetMat4(const char* uniform_name, const GRLMat4& mat) const = 0;
	};

	GRL_RESULT GRLCompileOpenGLShaderFromFile(
		const char* shader_file_path,
		GRL_OPENGL_SHADER_TYPE shader_type,
		GRLIOpenGLShader ** ppShader
	);

	GRL_RESULT GRLCreateOpenGLLinkedProgram(
		GRLIOpenGLShader* pVertexShader,
		GRLIOpenGLShader* pFragmentShader,
		GRLIOpenGLShader* pGeometryShader,
		GRLIOpenGLProgram **ppProgram
	);

	GRL_RESULT GRLCreateOpenGLProgramFromSourceFile(
		const char* vertex_shader_path,
		const char* fragment_shader_path,
		const char* geometry_shader_path,
		GRLIOpenGLProgram **ppProgram
	);
}