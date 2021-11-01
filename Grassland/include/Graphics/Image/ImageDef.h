#pragma once
#include <cstdint>
#include <cstring>
#include <iostream>

namespace Grassland
{
	namespace Graphics
	{
		template<typename T, int32_t channel>
		class Image;

		template<typename T, int32_t channel>
		struct Pixel;

		uint8_t pixel_float_to_byte(float color);

#pragma pack(1)
		struct Pixel<uint8_t, 1>
		{
			union
			{
				uint8_t r;
				uint8_t g;
				uint8_t b;
				uint8_t a;
			};
			Pixel(uint8_t color = 0) : r(color) {};
		};

		struct Pixel<uint8_t, 3>
		{
			uint8_t r;
			uint8_t g;
			uint8_t b;
			Pixel(uint8_t grey = 0) : r(grey), g(grey), b(grey) {};
			Pixel(uint8_t _r, uint8_t _g, uint8_t _b) : r(_r), g(_g), b(_b) {};
		};
#pragma pack()
	}
}