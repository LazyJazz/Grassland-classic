#pragma once
#include "PixelDecl.h"
namespace Grassland
{
	namespace Graphics
	{
		namespace Util
		{
			template<>
			struct Pixel<float, 3>
			{
				float r;
				float g;
				float b;
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
			inline Pixel<float, 3>::Pixel()
			{
				r = g = b = 0;
			}
			inline Pixel<float, 3>::Pixel(uint8_t color)
			{
				r = g = b = Pixel_Byte_to_Float(color);
			}
			inline Pixel<float, 3>::Pixel(float color)
			{
				r = g = b = color;
			}
			inline Pixel<float, 3>::Pixel(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
			{
				this->r = Pixel_Byte_to_Float(r);
				this->g = Pixel_Byte_to_Float(g);
				this->b = Pixel_Byte_to_Float(b);
				//this->a = Pixel_Byte_to_Float(a);
			}
			inline Pixel<float, 3>::Pixel(float r, float g, float b, float a)
			{
				this->r = r;
				this->g = g;
				this->b = b;
			}
			inline uint8_t Pixel<float, 3>::GetAValue() const
			{
				return 255;
			}
			inline uint8_t Pixel<float, 3>::GetRValue() const
			{
				return Pixel_Float_to_Byte(r);
			}
			inline uint8_t Pixel<float, 3>::GetGValue() const
			{
				return Pixel_Float_to_Byte(g);
			}
			inline uint8_t Pixel<float, 3>::GetBValue() const
			{
				return Pixel_Float_to_Byte(b);
			}
			inline float Pixel<float, 3>::GetAValueF() const
			{
				return 1.0f;
			}
			inline float Pixel<float, 3>::GetRValueF() const
			{
				return r;
			}
			inline float Pixel<float, 3>::GetGValueF() const
			{
				return g;
			}
			inline float Pixel<float, 3>::GetBValueF() const
			{
				return b;
			}
		}
	}
}