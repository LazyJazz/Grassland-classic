#pragma once
#include "Vector4.h"

namespace Grassland
{
	namespace Math
	{
		template<typename T>
		inline Vector<4, T>::Vector()
		{
			v[0] = v[1] = v[2] = v[3] = (T)0;
		}

		template<typename T>
		inline Vector<4, T>::Vector(T v0, T v1, T v2, T v3)
		{
			v[0] = v0;
			v[1] = v1;
			v[2] = v2;
			v[3] = v3;
		}

		template<typename T>
		inline Vector<4, T>::Vector(const T* pv)
		{
			v[0] = pv[0];
			v[1] = pv[1];
			v[2] = pv[2];
			v[3] = pv[3];
		}

		template<typename T>
		inline Vector<4, T>::Vector(T scalar)
		{
			v[0] = v[1] = v[2] = v[3] = scalar;
		}

		template<typename T>
		inline T& Vector<4, T>::operator[](int index)
		{
			return v[index];
		}

		template<typename T>
		inline const T& Vector<4, T>::operator[](int index) const
		{
			return v[index];
		}

		template<typename T>
		inline T Vector<4, T>::norm_sqr() const
		{
			return v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
		}

		template<typename T>
		inline T Vector<4, T>::norm() const
		{
			return T(sqrt(norm_sqr()));
		}

		template<typename T>
		template<typename U>
		inline Vector<4, T> Vector<4, T>::operator+(const Vector<4, U>& vec) const
		{
			return Vector<4, T>(v[0] + vec[0], v[1] + vec[1], v[2] + vec[2], v[3] + vec[3]);
		}

		template<typename T>
		template<typename U>
		inline Vector<4, T> Vector<4, T>::operator-(const Vector<4, U>& vec) const
		{
			return Vector<4, T>(v[0] - vec[0], v[1] - vec[1], v[2] - vec[2], v[3] - vec[3]);
		}

		template<typename T>
		template<typename U>
		inline Vector<4, T> Vector<4, T>::operator*(const Vector<4, U>& vec) const
		{
			return Vector<4, T>(v[0] * vec[0], v[1] * vec[1], v[2] * vec[2], v[3] * vec[3]);
		}

		template<typename T>
		template<typename U>
		inline Vector<4, T> Vector<4, T>::operator+(const U& s) const
		{
			return Vector<4, T>(v[0] + s, v[1] + s, v[2] + s, v[3] + s);
		}

		template<typename T>
		template<typename U>
		inline Vector<4, T> Vector<4, T>::operator-(const U& s) const
		{
			return Vector<4, T>(v[0] - s, v[1] - s, v[2] - s, v[3] - s);
		}

		template<typename T>
		template<typename U>
		inline Vector<4, T> Vector<4, T>::operator*(const U& s) const
		{
			return Vector<4, T>(v[0] * s, v[1] * s, v[2] * s, v[3] * s);
		}

		template<typename T>
		template<typename U>
		inline Vector<4, T> Vector<4, T>::operator/(const U& s) const
		{
			return *this * (T(1) / T(s));
		}

		template<typename T>
		template<typename U>
		inline Vector<4, U> Vector<4, T>::operator*(const Matrix<4, 4, U>& mat) const
		{
			return vecmulmat(*this, mat);
		}

		template<typename T>
		template<typename U>
		inline Vector<4, T>& Vector<4, T>::operator+=(const U& s)
		{
			*this = *this + s;
			return *this;
		}

		template<typename T>
		template<typename U>
		inline Vector<4, T>& Vector<4, T>::operator+=(const Vector<4, U>& s)
		{
			*this = *this + s;
			return *this;
		}

		template<typename T>
		template<typename U>
		inline Vector<4, T>& Vector<4, T>::operator-=(const U& s)
		{
			*this = *this - s;
			return *this;
		}

		template<typename T>
		template<typename U>
		inline Vector<4, T>& Vector<4, T>::operator-=(const Vector<4, U>& s)
		{
			*this = *this - s;
			return *this;
		}

		template<typename T>
		template<typename U>
		inline Vector<4, T>& Vector<4, T>::operator*=(const U& s)
		{
			*this = *this * s;
			return *this;
		}

		template<typename T>
		template<typename U>
		inline Vector<4, T>& Vector<4, T>::operator*=(const Vector<4, U>& s)
		{
			*this = *this * s;
			return *this;
		}

		template<typename T>
		template<typename U>
		inline Vector<4, T>& Vector<4, T>::operator/=(const U& s)
		{
			*this = *this / s;
			return *this;
		}

		template<typename T>
		inline Vector<4, T>& Vector<4, T>::operator*=(const Matrix<4, 4, T>& mat)
		{
			*this = *this * mat;
			return *this;
		}

		template<typename T>
		template<typename U>
		inline T Vector<4, T>::dot(const Vector<4, U>& s) const
		{
			return T(v[0] * s[0] + v[1] * s[1] + v[2] * s[2] + v[3] * s[3]);
		}

		template<typename T>
		std::ostream& operator<<(std::ostream& os, const Vector<4, T>& vec)
		{
			os << '[' << vec[0] << ", " << vec[1] << ", " << vec[2] << ", " << vec[3] << ']';
			return os;
		}
	}
}
