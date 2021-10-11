#pragma once
#include "GLHeader.h"

namespace Grassland
{
	namespace Graphics
	{
		namespace GL
		{
			class FrameBuffer
			{
			private:
				uint32_t hFrameBuffer;
				uint32_t hColorTex;
				uint32_t hDepthTex;
				int32_t height;
				int32_t width;
			public:
				FrameBuffer();
				~FrameBuffer();
				void Init(int32_t _width, int32_t _height, bool color_channel = 1, bool depth_channel = 1);
				void Resize(int32_t _width, int32_t _height);
				void Use() const;
				void Release();
				uint32_t GetColorTextureHandle() const;
				uint32_t GetDepthTextureHandle() const;
			};
			
			void UseScreenFrame();
		}
	}
}