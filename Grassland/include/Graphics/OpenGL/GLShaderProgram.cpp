#include "GLShaderProgram.h"

namespace Grassland
{
	namespace Graphics
	{
		namespace OpenGL
		{
			Program::Program()
			{
				__program_handle = NULL;
			}

			Program::~Program()
			{
				Clear();
			}

			GRL_RESULT Program::LinkShaders(Shader* vertex_shader, Shader* fragment_shader, Shader* geometry_shader)
			{
				uint32_t vertex_shader_handle = 0;
				uint32_t fragment_shader_handle = 0;
				uint32_t geometry_shader_handle = 0;
				if (vertex_shader) vertex_shader_handle = vertex_shader->GetShaderHandle();
				if (fragment_shader) fragment_shader_handle = fragment_shader->GetShaderHandle();
				return LinkShaders(
					vertex_shader_handle,
					fragment_shader_handle,
					geometry_shader_handle
				);
			}

			GRL_RESULT Program::LinkShaders(
				uint32_t __vertex_shader_handle,
				uint32_t __fragment_shader_handle,
				uint32_t __geometry_shader_handle
			)
			{
				Clear();
				if (!__vertex_shader_handle)
				{
					GRLSetErrorInfo("Vertex Shader must not be empty.");
					return GRL_TRUE;
				}
				if (!__fragment_shader_handle)
				{
					GRLSetErrorInfo("Fragment Shader must not be empty.");
					return GRL_TRUE;
				}
				__program_handle = glCreateProgram();
				glAttachShader(__program_handle, __vertex_shader_handle);
				glAttachShader(__program_handle, __fragment_shader_handle);
				if (__geometry_shader_handle)
					glAttachShader(__program_handle, __geometry_shader_handle);
				glLinkProgram(__program_handle);
				int32_t success;
				glGetProgramiv(__program_handle, GL_LINK_STATUS, &success);
				if (success != GL_TRUE)
				{
					//int32_t log_length = 0;
					//char message[1024];
					//glGetProgramInfoLog(__program_handle, 1000, &log_length, message);
					GRLSetErrorInfo("Program Link Failed.");
					Clear();
					return GRL_TRUE;
				}
				return GRL_FALSE;
			}

			void Program::Clear()
			{
				if (__program_handle) {
					glDeleteProgram(__program_handle);
					__program_handle = 0;
				}
				__uniform_location_map.clear();
			}

			int32_t Program::GetUniformLocation(const char* uniform_name)
			{
				if (!__program_handle)
				{
					GRLSetErrorInfo("Program is not ready to GetUniformLocation.");
					return 0;
				}
				if (__uniform_location_map.count(uniform_name))
					return __uniform_location_map.at(uniform_name);
				return __uniform_location_map[uniform_name]
					= glGetUniformLocation(__program_handle, uniform_name);
			}

			void Program::Use() const
			{
				if (!__program_handle)
				{
					GRLSetErrorInfo("Program is not ready to Use.");
					return;
				}
				glUseProgram(__program_handle);
			}

			void Program::SetFloat(int32_t location, float fvalue)
			{
				glProgramUniform1f(__program_handle, location, fvalue);
			}
			void Program::SetVec2(int32_t location, const GRLVec2& vec)
			{
				glProgramUniform2fv(__program_handle, location, 1, &vec[0]);
			}
			void Program::SetVec3(int32_t location, const GRLVec3& vec)
			{
				glProgramUniform3fv(__program_handle, location, 1, &vec[0]);
			}
			void Program::SetVec4(int32_t location, const GRLVec4& vec)
			{
				glProgramUniform4fv(__program_handle, location, 1, &vec[0]);
			}
			void Program::SetMat2(int32_t location, const GRLMat2& mat)
			{
				glProgramUniformMatrix2fv(__program_handle, location, 1, 1, &mat[0][0]);
			}
			void Program::SetMat3(int32_t location, const GRLMat3& mat)
			{
				glProgramUniformMatrix3fv(__program_handle, location, 1, 1, &mat[0][0]);
			}
			void Program::SetMat4(int32_t location, const GRLMat4& mat)
			{
				glProgramUniformMatrix4fv(__program_handle, location, 1, 1, &mat[0][0]);
			}
			void Program::SetFloat(const char* uname, float fvalue)
			{
				SetFloat(GetUniformLocation(uname), fvalue);
			}
			void Program::SetVec2(const char* uname, const GRLVec2& vec)
			{
				SetVec2(GetUniformLocation(uname), vec);
			}
			void Program::SetVec3(const char* uname, const GRLVec3& vec)
			{
				SetVec3(GetUniformLocation(uname), vec);
			}
			void Program::SetVec4(const char* uname, const GRLVec4& vec)
			{
				SetVec4(GetUniformLocation(uname), vec);
			}
			void Program::SetMat2(const char* uname, const GRLMat2& mat)
			{
				SetMat2(GetUniformLocation(uname), mat);
			}
			void Program::SetMat3(const char* uname, const GRLMat3& mat)
			{
				SetMat3(GetUniformLocation(uname), mat);
			}
			void Program::SetMat4(const char* uname, const GRLMat4& mat)
			{
				SetMat4(GetUniformLocation(uname), mat);
			}


			Shader::Shader()
			{
				__shader_handle = 0;
				__shader_type = GRL_OPENGL_SHADER_TYPE_NONE;
			}

			Shader::Shader(const char* shader_file_path, GRL_OPENGL_SHADER_TYPE shader_type)
			{
				__shader_handle = 0;
				__shader_type = GRL_OPENGL_SHADER_TYPE_NONE;
				CompileShaderFromFile(shader_file_path, shader_type);
			}

			Shader::~Shader()
			{
				Clear();
			}

			uint32_t __OpenGLShaderTypeEnumToValue(GRL_OPENGL_SHADER_TYPE __shader_type)
			{
				switch (__shader_type)
				{
				case GRL_OPENGL_SHADER_TYPE_FRAGMENT:
					return GL_FRAGMENT_SHADER;
				case GRL_OPENGL_SHADER_TYPE_VERTEX:
					return GL_VERTEX_SHADER;
				case GRL_OPENGL_SHADER_TYPE_GEOMETRY:
					return GL_GEOMETRY_SHADER;
				default:
					return NULL;
				}
			}

			GRL_RESULT Shader::CompileShaderFromFile(const char* shader_file_path, GRL_OPENGL_SHADER_TYPE shader_type)
			{
				__shader_type = shader_type;
				FILE* file = nullptr;
#ifdef _WIN32
				fopen_s(&file, shader_file_path, "rb");
#else
				file = fopen(shader_file_path, "rb");
#endif
				if (!file)
				{
					GRLSetErrorInfo("CompileShaderFromFile failed on opening file.");
					return GRL_TRUE;
				}
				fseek(file, 0, SEEK_END);
				int32_t length = ftell(file);
				//std::cout << "Code length:" << length << std::endl;
				char* source_code = new char[length + 1];
				memset(source_code, 0, length + 1);
				fseek(file, 0, SEEK_SET);
				fread(source_code, 1, length, file);
				//std::cout << source_code << std::endl;
				fclose(file);

				__shader_handle = glCreateShader(
					__OpenGLShaderTypeEnumToValue(shader_type)
				);
				glShaderSource(__shader_handle, 1, &source_code, NULL);
				glCompileShader(__shader_handle);
				delete[] source_code;

				int32_t success;
				glGetShaderiv(__shader_handle, GL_COMPILE_STATUS, &success);
				if (success != GL_TRUE)
				{
					int32_t log_length = 0;
					char message[1024];
					glGetShaderInfoLog(__shader_handle, 1000, &log_length, message);
					GRLSetErrorInfo("Shader Compilation Failed.");
					std::cout << "Error Info:\n" << message << std::endl;
					Clear();
					return GRL_TRUE;
				}
				return GRL_FALSE;
			}

			GRL_OPENGL_SHADER_TYPE Shader::GetShaderType() const
			{
				return __shader_type;
			}

			uint32_t Shader::GetShaderHandle() const
			{
				return __shader_handle;
			}

			uint32_t Shader::Clear()
			{
				if (__shader_handle)
					glDeleteShader(__shader_handle);
				__shader_handle = 0;
				__shader_type = GRL_OPENGL_SHADER_TYPE_NONE;
				return GRL_FALSE;
			}
		}
	}

	class GRLCOpenGLShader : public GRLIOpenGLShader
	{
	public:
		GRLCOpenGLShader(Graphics::OpenGL::Shader* pShader = nullptr);
		~GRLCOpenGLShader();
		virtual GRL_RESULT GetShaderType(GRL_OPENGL_SHADER_TYPE* type) const;
		virtual GRL_RESULT GetShaderHandle(uint32_t* __shader_handle_pnter) const;
		virtual GRL_RESULT AddRef();
		virtual GRL_RESULT Release();
	private:
		Graphics::OpenGL::Shader* __shader_pnter;
		int32_t __Ref_Cnt;
	};

	class GRLCOpenGLProgram : public GRLIOpenGLProgram
	{
	public:
		GRLCOpenGLProgram(Graphics::OpenGL::Program* pProgram = nullptr);
		~GRLCOpenGLProgram();

		virtual GRL_RESULT AddRef();
		virtual GRL_RESULT Release();
		virtual int32_t GetUniformLocation(const char* uniform_name) const;
		virtual GRL_RESULT Use() const;
		virtual GRL_RESULT SetFloat(int32_t location, float fvalue) const;
		virtual GRL_RESULT SetVec2(int32_t location, const GRLVec2& vec) const;
		virtual GRL_RESULT SetVec3(int32_t location, const GRLVec3& vec) const;
		virtual GRL_RESULT SetVec4(int32_t location, const GRLVec4& vec) const;
		virtual GRL_RESULT SetMat2(int32_t location, const GRLMat2& mat) const;
		virtual GRL_RESULT SetMat3(int32_t location, const GRLMat3& mat) const;
		virtual GRL_RESULT SetMat4(int32_t location, const GRLMat4& mat) const;
		virtual GRL_RESULT SetFloat(const char* uniform_name, float fvalue) const;
		virtual GRL_RESULT SetVec2(const char* uniform_name, const GRLVec2& vec) const;
		virtual GRL_RESULT SetVec3(const char* uniform_name, const GRLVec3& vec) const;
		virtual GRL_RESULT SetVec4(const char* uniform_name, const GRLVec4& vec) const;
		virtual GRL_RESULT SetMat2(const char* uniform_name, const GRLMat2& mat) const;
		virtual GRL_RESULT SetMat3(const char* uniform_name, const GRLMat3& mat) const;
		virtual GRL_RESULT SetMat4(const char* uniform_name, const GRLMat4& mat) const;
	private:
		Graphics::OpenGL::Program* __program_pnter;
		int32_t __Ref_Cnt;
	};

	GRL_RESULT GRLCompileOpenGLShaderFromFile(
		const char* shader_file_path,
		GRL_OPENGL_SHADER_TYPE shader_type,
		GRLIOpenGLShader** ppShader
	)
	{
		Graphics::OpenGL::Shader* pShader = new Graphics::OpenGL::Shader();
		if (pShader->CompileShaderFromFile(shader_file_path, shader_type))
		{
			delete pShader;
			*ppShader = NULL;
			return GRL_TRUE;
		}
		*ppShader = new GRLCOpenGLShader(pShader);
		return GRL_FALSE;
	}
	GRL_RESULT GRLCreateLinkedProgram(GRLIOpenGLShader* pVertexShader, GRLIOpenGLShader* pFragmentShader, GRLIOpenGLShader* pGeometryShader, GRLIOpenGLProgram** ppProgram)
	{
		Graphics::OpenGL::Program* pProgram = new Graphics::OpenGL::Program();
		uint32_t vertex_shader = 0;
		uint32_t fragment_shader = 0;
		uint32_t geometry_shader = 0;
		if (pVertexShader)
			pVertexShader->GetShaderHandle(&vertex_shader);
		if (pFragmentShader)
			pFragmentShader->GetShaderHandle(&fragment_shader);
		if (pGeometryShader)
			pGeometryShader->GetShaderHandle(&geometry_shader);

		if (pProgram->LinkShaders(
			vertex_shader,
			fragment_shader,
			geometry_shader
		))
		{
			delete pProgram;
			*ppProgram = NULL;
			return GRL_TRUE;
		}

		*ppProgram = new GRLCOpenGLProgram(pProgram);
		return GRL_FALSE;
	}
	GRL_RESULT GRLCreateProgramFromSourceFile(
		const char* vertex_shader_path,
		const char* fragment_shader_path,
		const char* geometry_shader_path,
		GRLIOpenGLProgram** ppProgram)
	{
		GRLIOpenGLShader* pVertexShader = nullptr;
		GRLIOpenGLShader* pFragmentShader = nullptr;
		GRLIOpenGLShader* pGeometryShader = nullptr;
		if (vertex_shader_path)
		{
			if (GRLCompileOpenGLShaderFromFile(
				vertex_shader_path,
				GRL_OPENGL_SHADER_TYPE_VERTEX,
				&pVertexShader))
			{
				if (pVertexShader) pVertexShader->Release();
				return GRL_TRUE;
			}
		}
		if (fragment_shader_path)
		{
			if (GRLCompileOpenGLShaderFromFile(
				fragment_shader_path,
				GRL_OPENGL_SHADER_TYPE_FRAGMENT,
				&pFragmentShader))
			{
				if (pVertexShader) pVertexShader->Release();
				if (pFragmentShader) pFragmentShader->Release();
				return GRL_TRUE;
			}
		}
		if (geometry_shader_path)
		{
			if (GRLCompileOpenGLShaderFromFile(
				geometry_shader_path,
				GRL_OPENGL_SHADER_TYPE_GEOMETRY,
				&pGeometryShader))
			{
				if (pVertexShader) pVertexShader->Release();
				if (pFragmentShader) pFragmentShader->Release();
				if (pGeometryShader) pGeometryShader->Release();
				return GRL_TRUE;
			}
		}
		if (GRLCreateLinkedProgram(
			pVertexShader, 
			pFragmentShader,
			pGeometryShader,
			ppProgram))
		{
			if (pVertexShader) pVertexShader->Release();
			if (pFragmentShader) pFragmentShader->Release();
			if (pGeometryShader) pGeometryShader->Release();
			return GRL_TRUE;
		}
		if (pVertexShader) pVertexShader->Release();
		if (pFragmentShader) pFragmentShader->Release();
		if (pGeometryShader) pGeometryShader->Release();
		return GRL_FALSE;
	}
	GRLCOpenGLShader::GRLCOpenGLShader(Graphics::OpenGL::Shader* pShader)
	{
		__shader_pnter = pShader;
		__Ref_Cnt = 1;
	}
	GRLCOpenGLShader::~GRLCOpenGLShader()
	{
		if (__shader_pnter) delete __shader_pnter;
	}
	GRL_RESULT GRLCOpenGLShader::GetShaderType(GRL_OPENGL_SHADER_TYPE* type) const
	{
		if (!__shader_pnter)
		{
			GRLSetErrorInfo("OpenGL Shader Object not initialized. ");
			return GRL_TRUE;
		}
		if (type)
			*type = __shader_pnter->GetShaderType();
		return GRL_FALSE;
	}
	GRL_RESULT GRLCOpenGLShader::GetShaderHandle(uint32_t* __shader_handle_pnter) const
	{
		if (!__shader_pnter)
		{
			GRLSetErrorInfo("OpenGL Shader Object not initialized. ");
			return GRL_TRUE;
		}
		if (__shader_handle_pnter)
			*__shader_handle_pnter = __shader_pnter->GetShaderHandle();
		return GRL_FALSE;
	}
	GRL_RESULT GRLCOpenGLShader::AddRef()
	{
		__Ref_Cnt++;
		return GRL_FALSE;
	}
	GRL_RESULT GRLCOpenGLShader::Release()
	{
		__Ref_Cnt--;
		if (!__Ref_Cnt)
			delete this;
		return GRL_FALSE;
	}
	GRLCOpenGLProgram::GRLCOpenGLProgram(Graphics::OpenGL::Program* pProgram)
	{
		__Ref_Cnt = 1;
		__program_pnter = pProgram;
	}
	GRLCOpenGLProgram::~GRLCOpenGLProgram()
	{
		if (__program_pnter) delete __program_pnter;
	}
	GRL_RESULT GRLCOpenGLProgram::AddRef()
	{
		__Ref_Cnt++;
		return GRL_FALSE;
	}
	GRL_RESULT GRLCOpenGLProgram::Release()
	{
		__Ref_Cnt--;
		if (!__Ref_Cnt) delete this;
		return GRL_FALSE;
	}
	int32_t GRLCOpenGLProgram::GetUniformLocation(const char* uniform_name) const
	{
		if (!__program_pnter)
		{
			GRLSetErrorInfo("OpenGL Program Object not initialized. ");
			return 0;
		}
		return __program_pnter->GetUniformLocation(uniform_name);
	}
	GRL_RESULT GRLCOpenGLProgram::Use() const
	{
		if (!__program_pnter)
		{
			GRLSetErrorInfo("OpenGL Program Object not initialized. ");
			return GRL_TRUE;
		}
		__program_pnter->Use();
		return GRL_FALSE;
	}
	GRL_RESULT GRLCOpenGLProgram::SetFloat(int32_t location, float fvalue) const
	{
		if (!__program_pnter)
		{
			GRLSetErrorInfo("OpenGL Program Object not initialized. ");
			return GRL_TRUE;
		}
		__program_pnter->SetFloat(location, fvalue);
		return GRL_FALSE;
	}
	GRL_RESULT GRLCOpenGLProgram::SetVec2(int32_t location, const GRLVec2& vec) const
	{
		if (!__program_pnter)
		{
			GRLSetErrorInfo("OpenGL Program Object not initialized. ");
			return GRL_TRUE;
		}
		__program_pnter->SetVec2(location, vec);
		return GRL_FALSE;
	}
	GRL_RESULT GRLCOpenGLProgram::SetVec3(int32_t location, const GRLVec3& vec) const
	{
		if (!__program_pnter)
		{
			GRLSetErrorInfo("OpenGL Program Object not initialized. ");
			return GRL_TRUE;
		}
		__program_pnter->SetVec3(location, vec);
		return GRL_FALSE;
	}
	GRL_RESULT GRLCOpenGLProgram::SetVec4(int32_t location, const GRLVec4& vec) const
	{
		if (!__program_pnter)
		{
			GRLSetErrorInfo("OpenGL Program Object not initialized. ");
			return GRL_TRUE;
		}
		__program_pnter->SetVec4(location, vec);
		return GRL_FALSE;
	}
	GRL_RESULT GRLCOpenGLProgram::SetMat2(int32_t location, const GRLMat2& mat) const
	{
		if (!__program_pnter)
		{
			GRLSetErrorInfo("OpenGL Program Object not initialized. ");
			return GRL_TRUE;
		}
		__program_pnter->SetMat2(location, mat);
		return GRL_FALSE;
	}
	GRL_RESULT GRLCOpenGLProgram::SetMat3(int32_t location, const GRLMat3& mat) const
	{
		if (!__program_pnter)
		{
			GRLSetErrorInfo("OpenGL Program Object not initialized. ");
			return GRL_TRUE;
		}
		__program_pnter->SetMat3(location, mat);
		return GRL_FALSE;
	}
	GRL_RESULT GRLCOpenGLProgram::SetMat4(int32_t location, const GRLMat4& mat) const
	{
		if (!__program_pnter)
		{
			GRLSetErrorInfo("OpenGL Program Object not initialized. ");
			return GRL_TRUE;
		}
		__program_pnter->SetMat4(location, mat);
		return GRL_FALSE;
	}
	GRL_RESULT GRLCOpenGLProgram::SetFloat(const char* uniform_name, float fvalue) const
	{
		if (!__program_pnter)
		{
			GRLSetErrorInfo("OpenGL Program Object not initialized. ");
			return GRL_TRUE;
		}
		__program_pnter->SetFloat(uniform_name, fvalue);
		return GRL_FALSE;
	}
	GRL_RESULT GRLCOpenGLProgram::SetVec2(const char* uniform_name, const GRLVec2& vec) const
	{
		if (!__program_pnter)
		{
			GRLSetErrorInfo("OpenGL Program Object not initialized. ");
			return GRL_TRUE;
		}
		__program_pnter->SetVec2(uniform_name, vec);
		return GRL_FALSE;
	}
	GRL_RESULT GRLCOpenGLProgram::SetVec3(const char* uniform_name, const GRLVec3& vec) const
	{
		if (!__program_pnter)
		{
			GRLSetErrorInfo("OpenGL Program Object not initialized. ");
			return GRL_TRUE;
		}
		__program_pnter->SetVec3(uniform_name, vec);
		return GRL_FALSE;
	}
	GRL_RESULT GRLCOpenGLProgram::SetVec4(const char* uniform_name, const GRLVec4& vec) const
	{
		if (!__program_pnter)
		{
			GRLSetErrorInfo("OpenGL Program Object not initialized. ");
			return GRL_TRUE;
		}
		__program_pnter->SetVec4(uniform_name, vec);
		return GRL_FALSE;
	}
	GRL_RESULT GRLCOpenGLProgram::SetMat2(const char* uniform_name, const GRLMat2& mat) const
	{
		if (!__program_pnter)
		{
			GRLSetErrorInfo("OpenGL Program Object not initialized. ");
			return GRL_TRUE;
		}
		__program_pnter->SetMat2(uniform_name, mat);
		return GRL_FALSE;
	}
	GRL_RESULT GRLCOpenGLProgram::SetMat3(const char* uniform_name, const GRLMat3& mat) const
	{
		if (!__program_pnter)
		{
			GRLSetErrorInfo("OpenGL Program Object not initialized. ");
			return GRL_TRUE;
		}
		__program_pnter->SetMat3(uniform_name, mat);
		return GRL_FALSE;
	}
	GRL_RESULT GRLCOpenGLProgram::SetMat4(const char* uniform_name, const GRLMat4& mat) const
	{
		if (!__program_pnter)
		{
			GRLSetErrorInfo("OpenGL Program Object not initialized. ");
			return GRL_TRUE;
		}
		__program_pnter->SetMat4(uniform_name, mat);
		return GRL_FALSE;
	}
}

