#pragma once
#include "Pixel.h"

namespace Grassland
{
	namespace Graphics
	{
		namespace Util
		{
			template<typename T, int channel>
			class Image
			{
				;
			};

		}

		class GrIImage : public GrIBase
		{
			virtual uint32_t Resize(int32_t width, int32_t height) = 0;
			virtual int32_t GetChannel() = 0;
			virtual int32_t GetWidth() = 0;
			virtual int32_t GetHeight() = 0;
			virtual int32_t GetType() = 0;
			virtual uint32_t Map(void** ppBuffer) = 0;
		};

		struct GRIMAGEINFO
		{
			int32_t width;
			int32_t height;
			uint32_t type;
			uint32_t channel;
		};

		void CreateGrImage(GRIMAGEINFO *imgInfo, GrIImage** ppImage);

		void LoadImageFromFile()
	}
}