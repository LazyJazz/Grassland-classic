#pragma once

#include "Matrix2x2.h"
#include "Matrix3x3.h"
#include "Matrix4x4.h"

namespace Grassland
{
	namespace Math
	{
		typedef Matrix<2, 2, float> Mat2x2;
		typedef Matrix<2, 2, double> Mat2x2d;
		typedef Matrix<2, 2, int> Mat2x2i;

		typedef Matrix<3, 3, float> Mat3x3;
		typedef Matrix<3, 3, double> Mat3x3d;
		typedef Matrix<3, 3, int> Mat3x3i;

		typedef Matrix<4, 4, float> Mat4x4;
		typedef Matrix<4, 4, double> Mat4x4d;
		typedef Matrix<4, 4, int> Mat4x4i;
	}
}