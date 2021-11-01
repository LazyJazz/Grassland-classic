#pragma once
#include "PixelDecl.h"
namespace Grassland
{
	namespace Graphics
	{
		namespace Util
		{
			template<>
			struct Pixel<float, 1>
			{
				union
				{
					float r;
					float g;
					float b;
				};
				Pixel();
				Pixel(uint8_t color);
				Pixel(float color);
				Pixel(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
				Pixel(float r, float g, float b, float a = 1.0);
				uint8_t GetAValue() const;
				uint8_t GetRValue() const;
				uint8_t GetGValue() const;
				uint8_t GetBValue() const;
				float GetAValueF() const;
				float GetRValueF() const;
				float GetGValueF() const;
				float GetBValueF() const;
			};
			inline Pixel<float, 1>::Pixel()
			{
				r = 0;
			}
			inline Pixel<float, 1>::Pixel(uint8_t color)
			{
				r = Pixel_Byte_to_Float(color);
			}
			inline Pixel<float, 1>::Pixel(float color)
			{
				r = color;
			}
			inline Pixel<float, 1>::Pixel(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
			{
				this->r = Pixel_Byte_to_Float(((uint32_t)r * 19595 + (uint32_t)g * 38469 + (uint32_t)b * 7472) >> 16);
			}
			inline Pixel<float, 1>::Pixel(float r, float g, float b, float a)
			{
				this->r = r * 0.299 + g * 0.587 + b * 0.114;
			}
			inline uint8_t Pixel<float, 1>::GetAValue() const
			{
				return 255;
			}
			inline uint8_t Pixel<float, 1>::GetRValue() const
			{
				return Pixel_Float_to_Byte(r);
			}
			inline uint8_t Pixel<float, 1>::GetGValue() const
			{
				return Pixel_Float_to_Byte(g);
			}
			inline uint8_t Pixel<float, 1>::GetBValue() const
			{
				return Pixel_Float_to_Byte(b);
			}
			inline float Pixel<float, 1>::GetAValueF() const
			{
				return 1.0f;
			}
			inline float Pixel<float, 1>::GetRValueF() const
			{
				return r;
			}
			inline float Pixel<float, 1>::GetGValueF() const
			{
				return g;
			}
			inline float Pixel<float, 1>::GetBValueF() const
			{
				return b;
			}
		}
	}
}