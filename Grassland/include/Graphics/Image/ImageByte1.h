#pragma once
#include "ImageDef.h"

namespace Grassland
{
	namespace Graphics
	{
		class Image<uint8_t, 1>
		{
#pragma pack(1)
			struct Pixel
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
#pragma pack()
			uint32_t width, height;
			Pixel* pixels;
		public:
			Image();
			Image(uint32_t width, uint32_t height);
			~Image();
			void Resize(uint32_t width, uint32_t height);
			uint32_t GetWidth() const;
			uint32_t GetHeight() const;
			Pixel& operator () (uint32_t x, uint32_t y);
			const Pixel& operator () (uint32_t x, uint32_t y) const;
			Pixel GetColor(uint32_t x, uint32_t y) const;
			void SetColor(uint32_t x, uint32_t y, Pixel color);
			void SaveBitmapToFile(const char* _path) const;
			int LoadBitmapFromFile(const char* _path);
			Pixel* GetColorPtr() const;
			void Clear(Pixel bkgroundColor = Pixel(0));
		};
	}
}

#include "ImageByte1.inl"