#include "GLFrameBuffer.h"

namespace Grassland
{
	uint32_t __OpenGLSlotTranslate(GRL_OPENGL_FRAMEBUFFER_SLOT slot)
	{
		if (slot == GRL_OPENGL_FRAMEBUFFER_SLOT_DEPTH)
			return GL_DEPTH_ATTACHMENT;
		if (slot >= GRL_OPENGL_FRAMEBUFFER_SLOT_COLOR0 && slot <= GRL_OPENGL_FRAMEBUFFER_SLOT_COLOR_LAST)
			return GL_COLOR_ATTACHMENT0 + (slot - GRL_OPENGL_FRAMEBUFFER_SLOT_COLOR0);
		return NULL;
	}

	class GRLCOpenGLFrameBuffer : public GRLIOpenGLFrameBuffer
	{
	public:
		GRLCOpenGLFrameBuffer(int32_t width, int32_t height);
		~GRLCOpenGLFrameBuffer();
		virtual uint32_t GetHandle() const;
		virtual int32_t GetWidth() const;
		virtual int32_t GetHeight() const;
		virtual GRL_RESULT CheckStatus() const;
		virtual GRL_RESULT BindTexture(GRLIOpenGLTexture* pTexture, GRL_OPENGL_FRAMEBUFFER_SLOT slot = GRL_OPENGL_FRAMEBUFFER_SLOT_COLOR0);
		virtual GRL_RESULT Resize(int32_t width, int32_t height);
		virtual GRL_RESULT AddRef();
		virtual GRL_RESULT Release();
	private:
		int32_t _width, _height;
		uint32_t __framebuffer_handle;
		int32_t __Ref_Cnt;
		GRLPtr<GRLIOpenGLTexture> __color_textures[32], __depth_texture;
	};

	GRL_RESULT GRLCreateOpenGLFrameBuffer(int32_t width, int32_t height, GRLIOpenGLFrameBuffer** ppFrameBuffer)
	{
		*ppFrameBuffer = new GRLCOpenGLFrameBuffer(width, height);
		return GRL_TRUE;
	}

	GRLCOpenGLFrameBuffer::GRLCOpenGLFrameBuffer(int32_t width, int32_t height)
	{
		_width = width;
		_height = height;
		__Ref_Cnt = 1;
		glGenFramebuffers(1, &__framebuffer_handle);
		for (int i = 0; i < 32; i++)
			__color_textures[i].Reset();
		__depth_texture.Reset();
	}
	
	GRLCOpenGLFrameBuffer::~GRLCOpenGLFrameBuffer()
	{
		glDeleteFramebuffers(1, &__framebuffer_handle);
		for (int i = 0; i < 32; i++)
			__color_textures[i].Reset();
		__depth_texture.Reset();
	}
	
	uint32_t GRLCOpenGLFrameBuffer::GetHandle() const
	{
		return __framebuffer_handle;
	}

	int32_t GRLCOpenGLFrameBuffer::GetWidth() const
	{
		return _width;
	}

	int32_t GRLCOpenGLFrameBuffer::GetHeight() const
	{
		return _height;
	}
	
	GRL_RESULT GRLCOpenGLFrameBuffer::CheckStatus() const
	{
		if (glCheckNamedFramebufferStatus(__framebuffer_handle, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
			return GRL_FALSE;
		return GRL_TRUE;
	}

	GRL_RESULT GRLCOpenGLFrameBuffer::BindTexture(GRLIOpenGLTexture* _pTexture, GRL_OPENGL_FRAMEBUFFER_SLOT slot)
	{
		GRLPtr<GRLIOpenGLTexture> pTexture(_pTexture);
		glBindFramebuffer(GL_FRAMEBUFFER, __framebuffer_handle);
		if (slot == GRL_OPENGL_FRAMEBUFFER_SLOT_DEPTH)
		{
			if (pTexture->GetFormat() == GRL_OPENGL_TEXTURE_FORMAT_DEPTH)
			{
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, pTexture->GetHandle(), 0);
				pTexture->Resize(_width, _height);
				__depth_texture.Set(pTexture.Get());
				//__color_textures[slot]->Resize(_width, _height);
			}
			else
			{
				GRLSetErrorInfo("You should not attach a color texture to depth frame.");
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				return GRL_TRUE;
			}
		}
		else if (slot >= GRL_OPENGL_FRAMEBUFFER_SLOT_COLOR0 && slot <= GRL_OPENGL_FRAMEBUFFER_SLOT_COLOR_LAST)
		{
			slot -= GRL_OPENGL_FRAMEBUFFER_SLOT_COLOR0;
			if (pTexture->GetFormat() != GRL_OPENGL_TEXTURE_FORMAT_DEPTH)
			{
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + slot, GL_TEXTURE_2D, pTexture->GetHandle(), 0);
				pTexture->Resize(_width, _height);
				__color_textures[slot].Set(pTexture.Get());
				//__color_textures[slot]->Resize(_width, _height);
			}
			else
			{
				GRLSetErrorInfo("You should not attach a depthmap to color frame.");
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				return GRL_TRUE;
			}
		}
		uint32_t colors[32] = {};
		int32_t color_cnt = 0;
		for (int i = 0; i < 32; i++)
		{
			if (__color_textures[i].Get())
			{
				colors[color_cnt] = GL_COLOR_ATTACHMENT0 + i;
				color_cnt++;
			}
		}
		glDrawBuffers(color_cnt, colors);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return GRL_FALSE;
	}

	GRL_RESULT GRLCOpenGLFrameBuffer::Resize(int32_t width, int32_t height)
	{
		_width = width;
		_height = height;
		for (int i = 0; i < 32; i++)
			if (__color_textures[i].Get())
				__color_textures[i]->Resize(width, height);
		__depth_texture->Resize(width, height);
		return GRL_FALSE;
	}
	GRL_RESULT GRLCOpenGLFrameBuffer::AddRef()
	{
		__Ref_Cnt++;
		return GRL_FALSE;
	}
	GRL_RESULT GRLCOpenGLFrameBuffer::Release()
	{
		__Ref_Cnt--;
		if (!__Ref_Cnt)
			delete this;
		return GRL_FALSE;
	}

	GRL_RESULT GRLOpenGLBindFrameBuffer(GRLIOpenGLFrameBuffer* pFramebuffer)
	{
		int width, height;
		if (!pFramebuffer)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			GRLOpenGLGetWindowSize(&width, &height);
		}
		else
		{
			width = pFramebuffer->GetWidth(), height = pFramebuffer->GetHeight();
			glBindFramebuffer(GL_FRAMEBUFFER, pFramebuffer->GetHandle());
		}
		glViewport(0, 0, width, height);
		return GRL_FALSE;
	}
}