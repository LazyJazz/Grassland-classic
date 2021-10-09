#include "GLShader.h"

namespace Grassland
{
	namespace Graphics
	{
		namespace GL
		{
			Shader::Shader()
			{
				hShader = 0;
			}

			GRASSLAND_RESULT Shader::CompileFromFile(const char* path, uint32_t type)
			{
				Release();
#ifdef _WIN32
				FILE* file = nullptr; fopen_s(&file, path, "r");
#else
				FILE* file = fopen(path, "r");
#endif
				if (!file)
					return GRASSLAND_FALSE;


				char readBuffer[2048] = {};
				int32_t length = 0, one_time_length;

				while (one_time_length = fread(readBuffer, 1, 2048, file))
					length += one_time_length;
				fseek(file, 0, SEEK_SET);

				char* source_code = new char [length + 1];
				memset(source_code, 0, length + 1);
				fread(source_code, 1, length, file);

				fclose(file);

				hShader = glCreateShader(type);
				glShaderSource(hShader, 1, &source_code, NULL);
				glCompileShader(hShader);
				delete[] source_code;

				int32_t success;
				glGetShaderiv(hShader, GL_COMPILE_STATUS, &success);
				if (success != GL_TRUE)
				{
					int32_t log_length = 0;
					char message[1024];
					glGetShaderInfoLog(hShader, 1000, &log_length, message);
					std::cout << "Shader Compilation Failed:" << std::endl;
					Release();
					return GRASSLAND_FALSE;
				}
				return GRASSLAND_TRUE;
			}
			uint32_t Shader::GetShaderHandle() const
			{
				return hShader;
			}
			void Shader::Release()
			{
				if (hShader) glDeleteShader(hShader);
				hShader = 0;
			}
			Program::Program()
			{
				uniformLocation.clear();
				hProgram = 0;
			}
			void Program::Init()
			{
				hProgram = glCreateProgram();
			}
			uint32_t Program::GetProgramHandle() const
			{
				return hProgram;
			}
			
			void Program::Attach(const Shader& shader) const
			{
				glAttachShader(hProgram, shader.GetShaderHandle());
			}

			GRASSLAND_RESULT Program::Link() const
			{
				glLinkProgram(hProgram);
				int32_t success;
				glGetProgramiv(hProgram, GL_LINK_STATUS, &success);
				if (success != GL_TRUE)
				{
					int32_t log_length = 0;
					char message[1024];
					glGetProgramInfoLog(hProgram, 1000, &log_length, message);
					std::cout << "Program Link Failed:" << std::endl;
					return GRASSLAND_FALSE;
				}
				return GRASSLAND_TRUE;
			}

			int32_t Program::GetUniformLocation(const char* uniformName)
			{
				if (uniformLocation.count(uniformName)) return uniformLocation.at(uniformName);
				return uniformLocation[uniformName] = glGetUniformLocation(hProgram, uniformName);
			}

			void Program::Release()
			{
				if (hProgram) glDeleteProgram(hProgram);
				hProgram = 0;
				uniformLocation.clear();
			}

			void Program::Use() const
			{
				if (!hProgram) puts("ERROR: Shader Program Not Initialized!!!!");
				glUseProgram(hProgram);
			}
			void Program::SetFloat(int32_t location, float fvalue)
			{
				glProgramUniform1f(hProgram, location, fvalue);
			}
			void Program::SetVec2(int32_t location, const Math::Vec2& vec)
			{
				glProgramUniform2fv(hProgram, location, 1, &vec[0]);
			}
			void Program::SetVec3(int32_t location, const Math::Vec3& vec)
			{
				glProgramUniform3fv(hProgram, location, 1, &vec[0]);
			}
			void Program::SetVec4(int32_t location, const Math::Vec4& vec)
			{
				glProgramUniform4fv(hProgram, location, 1, &vec[0]);
			}
			void Program::SetMat2(int32_t location, const Math::Mat2& mat)
			{
				glProgramUniformMatrix2fv(hProgram, location, 1, 1, &mat[0][0]);
			}
			void Program::SetMat3(int32_t location, const Math::Mat3& mat)
			{
				glProgramUniformMatrix3fv(hProgram, location, 1, 1, &mat[0][0]);
			}
			void Program::SetMat4(int32_t location, const Math::Mat4& mat)
			{
				glProgramUniformMatrix4fv(hProgram, location, 1, 1, &mat[0][0]);
			}
			void Program::SetFloat(const char* uname, float fvalue)
			{
				SetFloat(GetUniformLocation(uname), fvalue);
			}
			void Program::SetVec2(const char* uname, const Math::Vec2& vec)
			{
				SetVec2(GetUniformLocation(uname), vec);
			}
			void Program::SetVec3(const char* uname, const Math::Vec3& vec)
			{
				SetVec3(GetUniformLocation(uname), vec);
			}
			void Program::SetVec4(const char* uname, const Math::Vec4& vec)
			{
				SetVec4(GetUniformLocation(uname), vec);
			}
			void Program::SetMat2(const char* uname, const Math::Mat2& mat)
			{
				SetMat2(GetUniformLocation(uname), mat);
			}
			void Program::SetMat3(const char* uname, const Math::Mat3& mat)
			{
				SetMat3(GetUniformLocation(uname), mat);
			}
			void Program::SetMat4(const char* uname, const Math::Mat4& mat)
			{
				SetMat4(GetUniformLocation(uname), mat);
			}
		}
	}
}