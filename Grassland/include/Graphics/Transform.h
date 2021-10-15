#pragma once
#include "../Math/Math.h"

namespace Grassland
{
	namespace Graphics
	{
		Math::Mat4x4 TransformProjection(float tanFOVangle, float aspect, float Near, float Far);

		Math::Mat4x4 TransformTranslate(float x, float y, float z);
	}
}