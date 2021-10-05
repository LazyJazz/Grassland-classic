#pragma once
#include "Matrix2x2.h"

namespace Grassland
{
	namespace Math
	{
		template<typename T>
		inline Matrix<2, 2, T>::Matrix()
		{
			m[0][0] = m[0][1] = 0;
			m[1][0] = m[1][1] = 0;
		}

		template<typename T>
		inline Matrix<2, 2, T>::Matrix(T m00, T m01, T m10, T m11)
		{
			m[0][0] = m00; m[0][1] = m01;
			m[1][0] = m10; m[1][1] = m11;
		}

		template<typename T>
		inline Matrix<2, 2, T>::Matrix(T scalar)
		{
			m[0][0] = scalar; m[0][1] = 0;
			m[1][0] = 0; m[1][1] = scalar;
		}

		template<typename T>
		inline Vector<2, T>& Matrix<2, 2, T>::operator[](int index)
		{
			return m[index];
		}

		template<typename T>
		inline const Vector<2, T>& Matrix<2, 2, T>::operator[](int index) const
		{
			return m[index];
		}

		template<typename T>
		inline Matrix<2, 2, T> Matrix<2, 2, T>::inverse() const
		{
			return Matrix<2, 2, T>(
				m[1][1], -m[0][1],
				-m[1][0], m[0][0]
				) / determinant();
		}

		template<typename T>
		inline T Matrix<2, 2, T>::determinant() const
		{
			return m[0][0] * m[1][1] - m[0][1] * m[1][0];
		}

		template<typename T>
		template<typename U>
		inline Matrix<2, 2, T> Matrix<2, 2, T>::operator+(const Matrix<2, 2, U>& mat) const
		{
			return Matrix<2, 2, T>(
				m[0][0] + mat[0][0], m[0][1] + mat[0][1],
				m[1][0] + mat[1][0], m[1][1] + mat[1][1]
				);
		}
		template<typename T>
		template<typename U>
		inline Matrix<2, 2, T> Matrix<2, 2, T>::operator-(const Matrix<2, 2, U>& mat) const
		{
			return Matrix<2, 2, T>(
				m[0][0] - mat[0][0], m[0][1] - mat[0][1],
				m[1][0] - mat[1][0], m[1][1] - mat[1][1]
				);
		}
		template<typename T>
		template<typename U>
		inline Matrix<2, 2, T> Matrix<2, 2, T>::operator*(const Matrix<2, 2, U>& mat) const
		{
			return matmul(*this, mat);
		}

		template<typename T>
		template<typename U>
		inline Matrix<2, 2, T> Matrix<2, 2, T>::operator*(const U s) const
		{
			return Matrix<2, 2, T>(
				m[0][0] * s, m[0][1] * s,
				m[1][0] * s, m[1][1] * s
				);
		}

		template<typename T>
		template<typename U>
		inline Matrix<2, 2, T> Matrix<2, 2, T>::operator/(const U s) const
		{
			return *this * (T(1) / T(s));
		}

		template<typename T>
		template<typename U>
		inline Matrix<2, 2, T>& Matrix<2, 2, T>::operator+=(const Matrix<2, 2, U>& mat)
		{
			*this = *this + mat;
			return *this;
		}

		template<typename T>
		template<typename U>
		inline Matrix<2, 2, T>& Matrix<2, 2, T>::operator-=(const Matrix<2, 2, U>& mat)
		{
			*this = *this - mat;
			return *this;
		}

		template<typename T>
		template<typename U>
		inline Matrix<2, 2, T>& Matrix<2, 2, T>::operator*=(const Matrix<2, 2, U>& mat)
		{
			*this = *this * mat;
			return *this;
		}

		template<typename T>
		template<typename U>
		inline Matrix<2, 2, T>& Matrix<2, 2, T>::operator*=(const U s)
		{
			*this = *this * s;
			return *this;
		}

		template<typename T>
		template<typename U>
		inline Matrix<2, 2, T>& Matrix<2, 2, T>::operator/=(const U s)
		{
			*this = *this / s;
			return *this;
		}

		template<typename T>
		template<typename U>
		inline Vector<2, T> Matrix<2, 2, T>::operator*(const Vector<2, U>& vec) const
		{
			return matmulvec(*this, vec);
		}

		template<typename T>
		std::ostream& operator<<(std::ostream& os, const Matrix<2, 2, T>& mat)
		{
			os << "[[" << mat[0][0] << ", " << mat[0][1] << "]\n";
			os << " [" << mat[1][0] << ", " << mat[1][1] << "]]";
			return os;
		}
	}
}