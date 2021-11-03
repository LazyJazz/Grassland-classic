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

			Color RGB_to_Grey(float r, float g, float b, float a)
			{
				float color = r * 0.299 + g * 0.587 + b * 0.114;
				return Color(color, color, color, a);
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