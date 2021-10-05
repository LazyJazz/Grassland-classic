#pragma once
#include "Matrix4x4.h"

namespace Grassland
{
	namespace Math
	{
		template<typename T>
		inline Matrix<4, 4, T>::Matrix()
		{
			m[0][0] = m[0][1] = m[0][2] = m[0][3] = 0;
			m[1][0] = m[1][1] = m[1][2] = m[1][3] = 0;
			m[2][0] = m[2][1] = m[2][2] = m[2][3] = 0;
			m[3][0] = m[3][1] = m[3][2] = m[3][3] = 0;
		}

		template<typename T>
		inline Matrix<4, 4, T>::Matrix(
			T m00, T m01, T m02, T m03,
			T m10, T m11, T m12, T m13,
			T m20, T m21, T m22, T m23,
			T m30, T m31, T m32, T m33
		)
		{
			m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
			m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
			m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
			m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
		}

		template<typename T>
		inline Matrix<4, 4, T>::Matrix(T scalar)
		{
			m[0][0] = scalar; m[0][1] = 0; m[0][2] = 0; m[0][3] = 0;
			m[1][0] = 0; m[1][1] = scalar; m[1][2] = 0; m[1][3] = 0;
			m[2][0] = 0; m[2][1] = 0; m[2][2] = scalar; m[2][3] = 0;
			m[3][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = scalar;
		}

		template<typename T>
		inline Vector<4, T>& Matrix<4, 4, T>::operator[](int index)
		{
			return m[index];
		}

		template<typename T>
		inline const Vector<4, T>& Matrix<4, 4, T>::operator[](int index) const
		{
			return m[index];
		}

		template<typename T>
		inline Matrix<4, 4, T> Matrix<4, 4, T>::inverse() const
		{
			return matinverse(*this);
		}

		template<typename T>
		inline T Matrix<4, 4, T>::determinant() const
		{
			return matdeterminant(*this);
		}

		template<typename T>
		template<typename U>
		inline Matrix<4, 4, T> Matrix<4, 4, T>::operator+(const Matrix<4, 4, U>& mat) const
		{
			return Matrix<4, 4, T>(
				m[0][0] + mat[0][0], m[0][1] + mat[0][1], m[0][2] + mat[0][2], m[0][3] + mat[0][3],
				m[1][0] + mat[1][0], m[1][1] + mat[1][1], m[1][2] + mat[1][2], m[1][3] + mat[1][3],
				m[2][0] + mat[2][0], m[2][1] + mat[2][1], m[2][2] + mat[2][2], m[2][3] + mat[2][3],
				m[3][0] + mat[3][0], m[3][1] + mat[3][1], m[3][2] + mat[3][2], m[3][3] + mat[3][3]
				);
		}
		template<typename T>
		template<typename U>
		inline Matrix<4, 4, T> Matrix<4, 4, T>::operator-(const Matrix<4, 4, U>& mat) const
		{
			return Matrix<4, 4, T>(
				m[0][0] - mat[0][0], m[0][1] - mat[0][1], m[0][2] - mat[0][2], m[0][3] - mat[0][3],
				m[1][0] - mat[1][0], m[1][1] - mat[1][1], m[1][2] - mat[1][2], m[1][3] - mat[1][3],
				m[2][0] - mat[2][0], m[2][1] - mat[2][1], m[2][2] - mat[2][2], m[2][3] - mat[2][3],
				m[3][0] - mat[3][0], m[3][1] - mat[3][1], m[3][2] - mat[3][2], m[3][3] - mat[3][3]
				);
		}
		template<typename T>
		template<typename U>
		inline Matrix<4, 4, T> Matrix<4, 4, T>::operator*(const Matrix<4, 4, U>& mat) const
		{
			return matmul(*this, mat);
		}

		template<typename T>
		template<typename U>
		inline Matrix<4, 4, T> Matrix<4, 4, T>::operator*(const U s) const
		{
			return Matrix<4, 4, T>(
				m[0][0] * s, m[0][1] * s, m[0][2] * s, m[0][3] * s,
				m[1][0] * s, m[1][1] * s, m[1][2] * s, m[1][3] * s,
				m[2][0] * s, m[2][1] * s, m[2][2] * s, m[2][3] * s,
				m[3][0] * s, m[3][1] * s, m[3][2] * s, m[3][3] * s
				);
		}

		template<typename T>
		template<typename U>
		inline Matrix<4, 4, T> Matrix<4, 4, T>::operator/(const U s) const
		{
			return *this * (T(1) / T(s));
		}

		template<typename T>
		template<typename U>
		inline Matrix<4, 4, T>& Matrix<4, 4, T>::operator+=(const Matrix<4, 4, U>& mat)
		{
			*this = *this + mat;
			return *this;
		}

		template<typename T>
		template<typename U>
		inline Matrix<4, 4, T>& Matrix<4, 4, T>::operator-=(const Matrix<4, 4, U>& mat)
		{
			*this = *this - mat;
			return *this;
		}

		template<typename T>
		template<typename U>
		inline Matrix<4, 4, T>& Matrix<4, 4, T>::operator*=(const Matrix<4, 4, U>& mat)
		{
			*this = *this * mat;
			return *this;
		}

		template<typename T>
		template<typename U>
		inline Matrix<4, 4, T>& Matrix<4, 4, T>::operator*=(const U s)
		{
			*this = *this * s;
			return *this;
		}

		template<typename T>
		template<typename U>
		inline Matrix<4, 4, T>& Matrix<4, 4, T>::operator/=(const U s)
		{
			*this = *this / s;
			return *this;
		}

		template<typename T>
		template<typename U>
		inline Vector<4, T> Matrix<4, 4, T>::operator*(const Vector<4, U>& vec) const
		{
			return matmulvec(*this, vec);
		}

		template<typename T>
		std::ostream& operator<<(std::ostream& os, const Matrix<4, 4, T>& mat)
		{
			os << "[[" << mat[0][0] << ", " << mat[0][1] << ", " << mat[0][2] << ", " << mat[0][3] << "]\n";
			os << " [" << mat[1][0] << ", " << mat[1][1] << ", " << mat[1][2] << ", " << mat[1][3] << "]\n";
			os << " [" << mat[2][0] << ", " << mat[2][1] << ", " << mat[2][2] << ", " << mat[2][3] << "]\n";
			os << " [" << mat[3][0] << ", " << mat[3][1] << ", " << mat[3][2] << ", " << mat[3][3] << "]]";
			return os;
		}
	}
}