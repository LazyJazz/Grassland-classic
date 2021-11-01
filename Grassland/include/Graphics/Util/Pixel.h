#pragma once
#include "PixelByte.h"
#include "PixelByte3.h"
#include "PixelByte4.h"
#include "PixelFloat.h"
#include "PixelFloat3.h"
#include "PixelFloat4.h"

namespace Grassland
{
	namespace Graphics
	{
		namespace Util
		{
			typedef Pixel<uint8_t, 1> PixelB;
			typedef Pixel<uint8_t, 3> Pixel3B;
			typedef Pixel<uint8_t, 4> Pixel4B;
			typedef Pixel<float, 1> Pixelf;
			typedef Pixel<float, 3> Pixel3f;
			typedef Pixel<float, 4> Pixel4f;
		}
	}
}