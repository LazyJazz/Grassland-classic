#pragma once
#include "Vector.h"
#include "MatrixCommon.h"

namespace Grassland
{
	namespace Math
	{
		template<typename T>
		struct Matrix<4, 4, T>
		{
		private:
			Vector<4, T> m[4];
		public:
			Matrix();
			Matrix(
				T m00, T m01, T m02, T m03,
				T m10, T m11, T m12, T m13,
				T m20, T m21, T m22, T m23,
				T m30, T m31, T m32, T m33
			);
			Matrix(T scalar);
			Matrix(const Matrix<3, 3, T>& mat3);

			Vector<4, T>& operator[] (int index);
			const Vector<4, T>& operator[] (int index) const;

			template<typename U>
			Matrix<4, 4, T> operator + (const Matrix<4, 4, U>& mat) const;
			template<typename U>
			Matrix<4, 4, T> operator - (const Matrix<4, 4, U>& mat) const;
			template<typename U>
			Matrix<4, 4, T> operator * (const Matrix<4, 4, U>& mat) const;
			template<typename U>
			Matrix<4, 4, T> operator * (const U s) const;
			template<typename U>
			Matrix<4, 4, T> operator / (const U s) const;

			template<typename U>
			Matrix<4, 4, T>& operator += (const Matrix<4, 4, U>& mat);
			template<typename U>
			Matrix<4, 4, T>& operator -= (const Matrix<4, 4, U>& mat);
			template<typename U>
			Matrix<4, 4, T>& operator *= (const Matrix<4, 4, U>& mat);
			template<typename U>
			Matrix<4, 4, T>& operator *= (const U s);
			template<typename U>
			Matrix<4, 4, T>& operator /= (const U s);

			template<typename U>
			Vector<4, T> operator * (const Vector<4, U>& vec) const;

			Matrix<4, 4, T> inverse() const;
			Matrix<4, 4, T> transpose() const;
			T determinant() const;
		};


		template<typename T>
		std::ostream& operator << (std::ostream& os, const Matrix<4, 4, T>& vec);
	}
}

#include "Matrix4x4.inl"