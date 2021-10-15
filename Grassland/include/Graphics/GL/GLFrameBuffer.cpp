#include "GLFrameBuffer.h"

namespace Grassland
{
	namespace Graphics
	{
		namespace GL
		{
			FrameBuffer::FrameBuffer()
			{
				hFrameBuffer = 0;
				hColorTex = 0;
				hDepthTex = 0;
				height = width = 0;
			}
			FrameBuffer::~FrameBuffer()
			{
				Release();
			}
			void FrameBuffer::Init(int32_t _width, int32_t _height, bool color_channel, bool depth_channel)
			{
				Release();
				width = _width;
				height = _height;
				glGenFramebuffers(1, &hFrameBuffer);
				glBindFramebuffer(GL_FRAMEBUFFER, hFrameBuffer);
				if (color_channel)
				{
					glGenTextures(1, &hColorTex);
					glBindTexture(GL_TEXTURE_2D, hColorTex);
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
						width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hColorTex, 0);
					glBindTexture(GL_TEXTURE_2D, 0);
				}
				if (depth_channel)
				{
					glGenTextures(1, &hDepthTex);
					glBindTexture(GL_TEXTURE_2D, hDepthTex);
					glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
						width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, hDepthTex, 0);
					glBindTexture(GL_TEXTURE_2D, 0);
				}
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}
			void FrameBuffer::Resize(int32_t _width, int32_t _height)
			{
				Init(_width, _height, hColorTex, hDepthTex);
			}
			void FrameBuffer::Use() const
			{
				glBindFramebuffer(GL_FRAMEBUFFER, hFrameBuffer);
				if (hDepthTex) glEnable(GL_DEPTH_TEST);
				else glDisable(GL_DEPTH_TEST);
				glViewport(0, 0, width, height);
			}
			void FrameBuffer::Release()
			{
				width = height = 0;
				if (hFrameBuffer) glDeleteFramebuffers(1, &hFrameBuffer);
				if (hColorTex) glDeleteTextures(1, &hColorTex);
				if (hDepthTex) glDeleteTextures(1, &hDepthTex);
				hFrameBuffer = 0;
				hColorTex = 0;
				hDepthTex = 0;
			}
			uint32_t FrameBuffer::GetColorTextureHandle() const
			{
				return hColorTex;
			}
			uint32_t FrameBuffer::GetDepthTextureHandle() const
			{
				return hDepthTex;
			}
			void UseScreenFrame()
			{
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				int32_t width, height;
				glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);
				glViewport(0, 0, width, height);
				glEnable(GL_DEPTH_TEST);
			}
		}
	}
}