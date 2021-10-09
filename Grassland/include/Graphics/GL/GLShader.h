#pragma once
#include "GLHeader.h"
#include <map>

namespace Grassland
{
	namespace Graphics
	{
		namespace GL
		{
			class Shader
			{
			private:
				uint32_t hShader;
			public:
				Shader();
				GRASSLAND_RESULT CompileFromFile(const char * path, uint32_t type);
				uint32_t GetShaderHandle() const;
				void Release();
			};

			class Program
			{
			private:
				uint32_t hProgram;
				std::map<std::string, int32_t> uniformLocation;
			public:
				Program();
				void Init();
				uint32_t GetProgramHandle() const;
				void Attach(const Shader& shader) const;
				GRASSLAND_RESULT Link() const;
				int32_t GetUniformLocation(const char * uniformName);
				void Release();
				void Use() const;
				void SetFloat(int32_t location, float fvalue);
				void SetVec2(int32_t location, const Math::Vec2& vec);
				void SetVec3(int32_t location, const Math::Vec3& vec);
				void SetVec4(int32_t location, const Math::Vec4& vec);
				void SetMat2(int32_t location, const Math::Mat2& mat);
				void SetMat3(int32_t location, const Math::Mat3& mat);
				void SetMat4(int32_t location, const Math::Mat4& mat);
				void SetFloat(const char* uname, float fvalue);
				void SetVec2(const char* uname, const Math::Vec2& vec);
				void SetVec3(const char* uname, const Math::Vec3& vec);
				void SetVec4(const char* uname, const Math::Vec4& vec);
				void SetMat2(const char* uname, const Math::Mat2& mat);
				void SetMat3(const char* uname, const Math::Mat3& mat);
				void SetMat4(const char* uname, const Math::Mat4& mat);
			};
		}
	}
}