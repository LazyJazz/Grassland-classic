#pragma once
#include "PixelDecl.h"

namespace Grassland
{
	namespace Graphics
	{
		namespace Util
		{
			template<>
			struct Pixel<uint8_t, 3>
			{
				uint8_t r;
				uint8_t g;
				uint8_t b;
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
			inline Pixel<uint8_t, 3>::Pixel()
			{
				r = 0;
				g = 0;
				b = 0;
			}
			inline Pixel<uint8_t, 3>::Pixel(uint8_t color)
			{
				r = color;
				g = color;
				b = color;
			}
			inline Pixel<uint8_t, 3>::Pixel(float color)
			{
				r = g = b = Pixel_Float_to_Byte(color);
			}
			inline Pixel<uint8_t, 3>::Pixel(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
			{
				this->r = r;
				this->g = g;
				this->b = b;
			}
			inline Pixel<uint8_t, 3>::Pixel(float r, float g, float b, float a)
			{
				this->r = Pixel_Float_to_Byte(r);
				this->g = Pixel_Float_to_Byte(g);
				this->b = Pixel_Float_to_Byte(b);
			}
			inline uint8_t Pixel<uint8_t, 3>::GetAValue() const
			{
				return 255;
			}
			inline uint8_t Pixel<uint8_t, 3>::GetRValue() const
			{
				return r;
			}
			inline uint8_t Pixel<uint8_t, 3>::GetGValue() const
			{
				return g;
			}
			inline uint8_t Pixel<uint8_t, 3>::GetBValue() const
			{
				return b;
			}
			inline float Pixel<uint8_t, 3>::GetAValueF() const
			{
				return 1.0f;
			}
			inline float Pixel<uint8_t, 3>::GetRValueF() const
			{
				return Pixel_Byte_to_Float(r);
			}
			inline float Pixel<uint8_t, 3>::GetGValueF() const
			{
				return Pixel_Byte_to_Float(g);
			}
			inline float Pixel<uint8_t, 3>::GetBValueF() const
			{
				return Pixel_Byte_to_Float(b);
			}
		}
	}
}
