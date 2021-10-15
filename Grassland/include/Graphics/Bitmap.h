#pragma once
#ifdef _WIN32
#define NOMINMAX
#include <Windows.h>
#endif
#include <iostream>
#include <cstdio>
#include <cstdint>

namespace Grassland
{
	namespace Graphics
	{
#pragma pack (1)
		struct BitmapPixel
		{
			uint8_t b, g, r;
			BitmapPixel();
			BitmapPixel(uint8_t _r, uint8_t _g, uint8_t _b);
		};
#pragma pack ()

		class Bitmap
		{
			uint32_t width, height;
			BitmapPixel* pixels;
		public:
			Bitmap();
			Bitmap(uint32_t width, uint32_t height);
			~Bitmap();
			void Resize(uint32_t width, uint32_t height);
			uint32_t GetWidth() const;
			uint32_t GetHeight() const;
			BitmapPixel& operator () (uint32_t x, uint32_t y);
			BitmapPixel GetColor(uint32_t x, uint32_t y) const;
			void SetColor(uint32_t x, uint32_t y, BitmapPixel color);
			int SaveToFile(const char* _path) const;
			int LoadFromFile(const char* _path);
			void SaveBitmapToFile(const char* _path) const;
			int LoadBitmapFromFile(const char* _path);
			BitmapPixel* GetColorPtr() const;
			void Clear(BitmapPixel bkgroundColor = BitmapPixel(0, 0, 0));
		};


#ifndef _WINDOWS_
#pragma pack (1)
		struct BITMAPFILEHEADER {
			uint16_t    bfType;
			uint32_t   bfSize;
			uint16_t    bfReserved1;
			uint16_t    bfReserved2;
			uint32_t   bfOffBits;
		};

		struct BITMAPINFOHEADER {
			uint32_t      biSize;
			int32_t       biWidth;
			int32_t       biHeight;
			uint16_t       biPlanes;
			uint16_t       biBitCount;
			uint32_t      biCompression;
			uint32_t      biSizeImage;
			int32_t       biXPelsPerMeter;
			int32_t       biYPelsPerMeter;
			uint32_t      biClrUsed;
			uint32_t      biClrImportant;
		};
#pragma pack ()
#endif
	}
}