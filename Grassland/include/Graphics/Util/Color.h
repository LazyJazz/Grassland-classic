#pragma once

#include <cstdint>
#include <algorithm>


namespace Grassland
{
	namespace Graphics
	{
		namespace Util
		{
			struct Color
			{
				float r, g, b, a;
				Color();
				Color(int32_t r, int32_t g, int32_t b, int32_t = 255);
				Color(float r, float g, float b, float a = 1.0);
			};

			Color RGB_to_Grey(float r, float g, float b, float a = 1.0);

			Color HSV_to_RGB(float H, float S, float V);

			int32_t Color_Float_to_Int(float color);
		}
	}
}