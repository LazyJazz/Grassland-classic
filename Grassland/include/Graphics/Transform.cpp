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
	}
}