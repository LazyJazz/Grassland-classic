#pragma once
#include "../../GrasslandDecl.h"
namespace Grassland
{
	namespace Graphics
	{
		namespace Util
		{
			template<typename __Ty, int channel>
			struct Pixel;

			uint8_t Pixel_Float_to_Byte(float color);

			float Pixel_Byte_to_Float(uint8_t color);
		}
	}
}