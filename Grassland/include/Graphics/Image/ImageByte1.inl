#pragma once
#include "ImageByte1.h"

namespace Grassland
{
	namespace Graphics
	{
		inline Image<uint8_t, 1>::Image()
		{
			width = height = 0;
			pixels = nullptr;
		}

		inline Image<uint8_t, 1>::Image(uint32_t width, uint32_t height)
		{
			this->width = width;
			this->height = height;
			pixels = new Pixel[width * height];
			memset(pixels, 0, sizeof(Pixel) * width * height);
		}
		inline Image<uint8_t, 1>::~Image()
		{
			if (pixels) delete[] pixels;
		}
		inline void Image<uint8_t, 1>::Resize(uint32_t width, uint32_t height)
		{
			Pixel* new_pixels = new Pixel [width * height];
			uint32_t copy_width = std::min(width, this->width), copy_height = std::min(height, this->height);
			for (int32_t y = 0; y < copy_height; y++)
				memcpy(new_pixels + y * width, pixels + y * this->width, sizeof(Pixel) * copy_width);
			if (pixels)
				delete[] pixels;
			this->pixels = new_pixels;
			this->width = width;
			this->height = height;
			
		}
		inline uint32_t Image<uint8_t, 1>::GetWidth() const
		{
			return width;
		}
		inline uint32_t Image<uint8_t, 1>::GetHeight() const
		{
			return height;
		}
		inline Image<uint8_t, 1>::Pixel& Image<uint8_t, 1>::operator()(uint32_t x, uint32_t y)
		{
			return pixels[y * width + x];
		}
		inline const Image<uint8_t, 1>::Pixel& Image<uint8_t, 1>::operator()(uint32_t x, uint32_t y) const
		{
			pixels[y * width + x];
			// TODO: 在此处插入 return 语句
		}
	}
}