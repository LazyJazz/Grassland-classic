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
				Clear();
				if (!vertex_shader || !vertex_shader->GetShaderHandle())
				{
					GRLSetErrorInfo("Vertex Shader must not be empty.");
					return GRL_TRUE;
				}
				if (!fragment_shader || !fragment_shader->GetShaderHandle())
				{
					GRLSetErrorInfo("Fragment Shader must not be empty.");
					return GRL_TRUE;
				}
				__program_handle = glCreateProgram();
				glAttachShader(__program_handle, vertex_shader->GetShaderHandle());
				glAttachShader(__program_handle, fragment_shader->GetShaderHandle());
				if (geometry_shader && geometry_shader->GetShaderHandle())
					glAttachShader(__program_handle, geometry_shader->GetShaderHandle());
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
				char* source_code = new char[length + 1];
				fseek(file, 0, SEEK_SET);
				fread(source_code, 1, length, file);
				delete[] source_code;
				fclose(file);

				__shader_handle = glCreateShader(__OpenGLShaderTypeEnumToValue(__shader_type));
				glShaderSource(__shader_handle, 1, &source_code, NULL);
				glCompileShader(__shader_handle);
				delete[] source_code;

				int32_t success;
				glGetShaderiv(__shader_handle, GL_COMPILE_STATUS, &success);
				if (success != GL_TRUE)
				{
					//int32_t log_length = 0;
					//char message[1024];
					//glGetShaderInfoLog(__shader_handle, 1000, &log_length, message);
					GRLSetErrorInfo("Shader Compilation Failed.");
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
}

