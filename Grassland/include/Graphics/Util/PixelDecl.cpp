#include "PixelDecl.h"

uint8_t Grassland::Graphics::Util::Pixel_Float_to_Byte(float color)
{
	int res = color * 255;
	if (res > 255) res = 255;
	if (res < 0) res = 0;
	return uint8_t(res);
}

float Grassland::Graphics::Util::Pixel_Byte_to_Float(uint8_t color)
{
	return color * (1.0f / 255.0f);
}
