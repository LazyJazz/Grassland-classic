#pragma once
#include "Matrix3x3.h"

namespace Grassland
{
	namespace Math
	{
		template<typename T>
		inline Matrix<3, 3, T>::Matrix()
		{
			m[0][0] = m[0][1] = m[0][2] = 0;
			m[1][0] = m[1][1] = m[1][2] = 0;
			m[2][0] = m[2][1] = m[2][2] = 0;
		}

		template<typename T>
		inline Matrix<3, 3, T>::Matrix(
			T m00, T m01, T m02,
			T m10, T m11, T m12,
			T m20, T m21, T m22
		)
		{
			m[0][0] = m00; m[0][1] = m01; m[0][2] = m02;
			m[1][0] = m10; m[1][1] = m11; m[1][2] = m12;
			m[2][0] = m20; m[2][1] = m21; m[2][2] = m22;
		}

		template<typename T>
		inline Matrix<3, 3, T>::Matrix(T scalar)
		{
			m[0][0] = scalar; m[0][1] = 0; m[0][2] = 0;
			m[1][0] = 0; m[1][1] = scalar; m[1][2] = 0;
			m[2][0] = 0; m[2][1] = 0; m[2][2] = scalar;
		}

		template<typename T>
		inline Matrix<3, 3, T>::Matrix(const Matrix<4, 4, T>& mat4)
		{
			for (int i = 0; i < 3; i++)
				for (int j = 0; j < 3; j++)
					m[i][j] = mat4[i][j];
		}

		template<typename T>
		inline Vector<3, T>& Matrix<3, 3, T>::operator[](int index)
		{
			return m[index];
		}

		template<typename T>
		inline const Vector<3, T>& Matrix<3, 3, T>::operator[](int index) const
		{
			return m[index];
		}

		template<typename T>
		inline Matrix<3, 3, T> Matrix<3, 3, T>::inverse() const
		{
			return Matrix<3, 3, T>(
				m[1][1] * m[2][2] - m[2][1] * m[1][2], -m[0][1] * m[2][2] + m[2][1] * m[0][2], m[0][1] * m[1][2] - m[1][1] * m[0][2],
				-m[1][0] * m[2][2] + m[2][0] * m[1][2], m[0][0] * m[2][2] - m[0][2] * m[2][0], -m[0][0] * m[1][2] + m[1][0] * m[0][2],
				m[1][0] * m[2][1] - m[2][0] * m[1][1], -m[0][0] * m[2][1] + m[2][0] * m[0][1], m[0][0] * m[1][1] - m[0][1] * m[1][0]
				) / determinant();
		}

		template<typename T>
		inline T Matrix<3, 3, T>::determinant() const
		{
			return
				m[0][0] * m[1][1] * m[2][2] +
				m[0][1] * m[1][2] * m[2][0] +
				m[0][2] * m[1][0] * m[2][1] -
				m[0][0] * m[1][2] * m[2][1] -
				m[0][2] * m[1][1] * m[2][0] -
				m[0][1] * m[1][0] * m[2][2];
		}

		template<typename T>
		template<typename U>
		inline Matrix<3, 3, T> Matrix<3, 3, T>::operator+(const Matrix<3, 3, U>& mat) const
		{
			return Matrix<3, 3, T>(
				m[0][0] + mat[0][0], m[0][1] + mat[0][1], m[0][2] + mat[0][2],
				m[1][0] + mat[1][0], m[1][1] + mat[1][1], m[1][2] + mat[1][2],
				m[2][0] + mat[2][0], m[2][1] + mat[2][1], m[2][2] + mat[2][2]
				);
		}
		template<typename T>
		template<typename U>
		inline Matrix<3, 3, T> Matrix<3, 3, T>::operator-(const Matrix<3, 3, U>& mat) const
		{
			return Matrix<3, 3, T>(
				m[0][0] - mat[0][0], m[0][1] - mat[0][1], m[0][2] - mat[0][2],
				m[1][0] - mat[1][0], m[1][1] - mat[1][1], m[1][2] - mat[1][2],
				m[2][0] - mat[2][0], m[2][1] - mat[2][1], m[2][2] - mat[2][2]
				);
		}
		template<typename T>
		template<typename U>
		inline Matrix<3, 3, T> Matrix<3, 3, T>::operator*(const Matrix<3, 3, U>& mat) const
		{
			return matmul(*this, mat);
		}

		template<typename T>
		template<typename U>
		inline Matrix<3, 3, T> Matrix<3, 3, T>::operator*(const U s) const
		{
			return Matrix<3, 3, T>(
				m[0][0] * s, m[0][1] * s, m[0][2] * s,
				m[1][0] * s, m[1][1] * s, m[1][2] * s,
				m[2][0] * s, m[2][1] * s, m[2][2] * s
				);
		}

		template<typename T>
		template<typename U>
		inline Matrix<3, 3, T> Matrix<3, 3, T>::operator/(const U s) const
		{
			return *this * (T(1) / T(s));
		}

		template<typename T>
		template<typename U>
		inline Matrix<3, 3, T>& Matrix<3, 3, T>::operator+=(const Matrix<3, 3, U>& mat)
		{
			*this = *this + mat;
			return *this;
		}

		template<typename T>
		template<typename U>
		inline Matrix<3, 3, T>& Matrix<3, 3, T>::operator-=(const Matrix<3, 3, U>& mat)
		{
			*this = *this - mat;
			return *this;
		}

		template<typename T>
		template<typename U>
		inline Matrix<3, 3, T>& Matrix<3, 3, T>::operator*=(const Matrix<3, 3, U>& mat)
		{
			*this = *this * mat;
			return *this;
		}

		template<typename T>
		template<typename U>
		inline Matrix<3, 3, T>& Matrix<3, 3, T>::operator*=(const U s)
		{
			*this = *this * s;
			return *this;
		}

		template<typename T>
		template<typename U>
		inline Matrix<3, 3, T>& Matrix<3, 3, T>::operator/=(const U s)
		{
			*this = *this / s;
			return *this;
		}

		template<typename T>
		template<typename U>
		inline Vector<3, T> Matrix<3, 3, T>::operator*(const Vector<3, U>& vec) const
		{
			return matmulvec(*this, vec);
		}

		template<typename T>
		std::ostream& operator<<(std::ostream& os, const Matrix<3, 3, T>& mat)
		{
			os << "[[" << mat[0][0] << ", " << mat[0][1] << ", " << mat[0][2] << "]\n";
			os << " [" << mat[1][0] << ", " << mat[1][1] << ", " << mat[1][2] << "]\n";
			os << " [" << mat[2][0] << ", " << mat[2][1] << ", " << mat[2][2] << "]]";
			return os;
		}
	}
}