#pragma once
#include "GLHeader.h"
#include "../Util/Image.h"

namespace Grassland
{
	typedef uint32_t GRL_OPENGL_TEXTURE_FORMAT;
#define GRL_OPENGL_TEXTURE_FORMAT_RGB 0
#define GRL_OPENGL_TEXTURE_FORMAT_RGBA 1
#define GRL_OPENGL_TEXTURE_FORMAT_R 2
#define GRL_OPENGL_TEXTURE_FORMAT_DEPTH 3
	namespace Graphics
	{
		namespace OpenGL
		{
			class Texture
			{
			public:
				Texture(int32_t width, int32_t height, GRL_OPENGL_TEXTURE_FORMAT format = GRL_OPENGL_TEXTURE_FORMAT_RGBA, void* data = nullptr);
				~Texture();
				int32_t GetWidth() const;
				int32_t GetHeight() const;
				uint32_t GetHandle() const;
				GRL_OPENGL_TEXTURE_FORMAT GetFormat() const;
				GRL_RESULT GetImage(Grassland::Graphics::Util::Image * img) const;
				void Resize(int32_t width, int32_t height);
			private:
				uint32_t __texture_handle;
				GRL_OPENGL_TEXTURE_FORMAT __texture_format;
				int32_t _width, _height;
			};
		}
	}

	class GRLIOpenGLTexture : public GRLIBase
	{
	public:
		virtual int32_t GetWidth() const = 0;
		virtual int32_t GetHeight() const = 0;
		virtual uint32_t GetHandle() const = 0;
		virtual GRL_OPENGL_TEXTURE_FORMAT GetFormat() const = 0;
		virtual GRL_RESULT GetImage(GRLIImage* pImg) const = 0;
		virtual GRL_RESULT BindTexture(int32_t slot) const = 0;
		virtual GRL_RESULT Resize(int32_t width, int32_t height) const = 0;
	};

	GRL_RESULT GRLCreateOpenGLTexture(int32_t width, int32_t height, GRL_OPENGL_TEXTURE_FORMAT format, void* init_data, GRLIOpenGLTexture** ppTexture);

	GRL_RESULT GRLCreateOpenGLTextureFromImage(GRLIImage* pImg, GRLIOpenGLTexture** ppTexture);

	//GRL_RESULT GRLCreateOpenGLTextureFromImage(GRLIImage * pImg);
}