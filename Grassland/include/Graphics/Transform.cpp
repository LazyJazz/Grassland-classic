#include "Transform.h"

namespace Grassland
{
	namespace Graphics
	{
		Math::Mat4x4 TransformProjection(float FOVangle, float aspect, float Near, float Far)
		{
			float INVtanFOVangle = 1.0 / tan(FOVangle * 0.5);
			return Math::Mat4x4(
				INVtanFOVangle / aspect, 0.0, 0.0, 0.0,
				0.0, INVtanFOVangle, 0.0, 0.0,
				0.0, 0.0, Far / (Far - Near), -(Near * Far) / (Far - Near),
				0.0, 0.0, 1.0, 0.0
			);
		}
		Math::Mat4x4 TransformTranslate(float x, float y, float z)
		{
			return Math::Mat4x4(
				1.0, 0.0, 0.0, x,
				0.0, 1.0, 0.0, y,
				0.0, 0.0, 1.0, z,
				0.0, 0.0, 0.0, 1.0
			);
		}
	}
}