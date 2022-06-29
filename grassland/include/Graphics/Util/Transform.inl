#include "Transform.h"

namespace Grassland
{
	template<typename __Ty>
	Math::Matrix<4, 4, __Ty> GRLTransformProjection(__Ty FOVangle, __Ty aspect, __Ty Near, __Ty Far)
	{
		__Ty INVtanFOVangle = 1.0 / tan(FOVangle * 0.5);
		return Math::Matrix<4, 4, __Ty>(
			INVtanFOVangle / aspect, 0.0, 0.0, 0.0,
			0.0, INVtanFOVangle, 0.0, 0.0,
			0.0, 0.0, Far / (Far - Near), -(Near * Far) / (Far - Near),
			0.0, 0.0, 1.0, 0.0
			);
	}

	template<typename __Ty>
	Math::Matrix<4, 4, __Ty> GRLTransformTranslate(__Ty x, __Ty y, __Ty z)
	{
		return Math::Matrix<4, 4, __Ty>(
			1.0, 0.0, 0.0, x,
			0.0, 1.0, 0.0, y,
			0.0, 0.0, 1.0, z,
			0.0, 0.0, 0.0, 1.0
			);
	}

	template<typename __Ty>
	Math::Matrix<4, 4, __Ty> GRLTransformRotation(__Ty pitch, __Ty yaw, __Ty roll)
	{
		return
			//Math::Matrix<4, 4, __Ty>(1.0);
			Math::Matrix<4, 4, __Ty>(
				cos(yaw), 0.0, -sin(yaw), 0.0,
				0.0, 1.0, 0.0, 0.0,
				sin(yaw), 0.0,  cos(yaw), 0.0,
				0.0, 0.0, 0.0, 1.0
				) *
			Math::Matrix<4, 4, __Ty>(
				1.0, 0.0, 0.0, 0.0,
				0.0, cos(pitch), -sin(pitch), 0.0,
				0.0, sin(pitch),  cos(pitch), 0.0,
				0.0, 0.0, 0.0, 1.0
				) *
			Math::Matrix<4, 4, __Ty>(
				cos(roll), sin(roll), 0.0, 0.0,
				-sin(roll),  cos(roll), 0.0, 0.0,
				0.0, 0.0, 1.0, 0.0,
				0.0, 0.0, 0.0, 1.0
				);
	}

	template<typename __Ty>
	Math::Matrix<4, 4, __Ty> GRLTransformLookAt(Math::Vector<3, __Ty> from, Math::Vector<3, __Ty> look_at)
	{
		Math::Vector<3, __Ty> zaxis = look_at - from;
		zaxis = zaxis / zaxis.norm();
		Math::Vector<3, __Ty> xaxis = Math::Vector<3, __Ty>(0.0, 1.0, 0.0) ^ zaxis;
		Math::Vector<3, __Ty> yaxis;
		if (xaxis.norm_sqr() < Math::EPS<__Ty>())
			xaxis = Math::Vector<3, __Ty>(1.0, 0.0, 0.0);
		else
			xaxis /= xaxis.norm();
		yaxis = zaxis ^ xaxis;
		return
			Math::Matrix<4, 4, __Ty>(
				xaxis[0], yaxis[0], zaxis[0], from[0],
				xaxis[1], yaxis[1], zaxis[1], from[1],
				xaxis[2], yaxis[2], zaxis[2], from[2],
				0.0, 0.0, 0.0, 1.0
				);
	}
}
