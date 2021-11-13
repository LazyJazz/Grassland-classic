#include "color.h"

namespace Grassland
{
	namespace Graphics
	{
		namespace Util
		{
			Color::Color()
			{
				r = g = b = a = 0.0;
			}

			Color::Color(int32_t r, int32_t g, int32_t b, int32_t a)
			{
				this->r = r * (1.0f / 255.0f);
				this->g = g * (1.0f / 255.0f);
				this->b = b * (1.0f / 255.0f);
				this->a = a * (1.0f / 255.0f);
			}

			Color::Color(float r, float g, float b, float a)
			{
				this->r = r;
				this->g = g;
				this->b = b;
				this->a = a;
			}

			Color::Color(double r, double g, double b, double a)
			{
				this->r = r;
				this->g = g;
				this->b = b;
				this->a = a;
			}

			Color RGB_to_Grey(float r, float g, float b, float a)
			{
				float color = r * 0.299 + g * 0.587 + b * 0.114;
				return Color(color, color, color, a);
			}

			Color HSV_to_RGB(float H, float S, float V, float A)
			{
				float R, G, B, C, X, m;
				int trigger = H * 6.0;
				trigger = trigger % 6;
				if (trigger < 0) trigger += 6;
				C = V * S;
				float Hmod2 = (H * 3.0f - floor(H * 3.0f)) * 2.0f;
				X = C * ( 1.0f - std::abs(Hmod2 - 1.0f));
				m = V - C;
				switch (trigger)
				{
				case 0:
					R = C; G = X; B = 0;
					break;
				case 1:
					R = X; G = C; B = 0;
					break;
				case 2:
					R = 0; G = C; B = X;
					break;
				case 3:
					R = 0; G = X; B = C;
					break;
				case 4:
					R = X; G = 0; B = C;
					break;
				case 5:
					R = C; G = 0; B = X;
					break;
				}
				return Color(R + m, G + m, B + m, A);
			}

			int32_t Color_Float_to_Int(float color)
			{
				int32_t res = color * 255.0f + 0.f;
				if (res > 255) res = 255;
				else if (res < 0) res = 0;
				return res;
			}
		}
	}
}