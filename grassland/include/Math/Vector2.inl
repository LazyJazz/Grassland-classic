#pragma once
#include "Vector2.h"

namespace Grassland
{
	namespace Math
	{
		template<typename T>
		inline Vector<2, T>::Vector()
		{
			v[0] = v[1] = (T)0;
		}

		template<typename T>
		inline Vector<2, T>::Vector(T v0, T v1)
		{
			v[0] = v0;
			v[1] = v1;
		}

		template<typename T>
		inline Vector<2, T>::Vector(const T* pv)
		{
			v[0] = pv[0];
			v[1] = pv[1];
		}

		template<typename T>
		inline Vector<2, T>::Vector(T scalar)
		{
			v[0] = v[1] = scalar;
		}

		template<typename T>
		inline T& Vector<2, T>::operator[](int index)
		{
			return v[index];
		}

		template<typename T>
		inline const T& Vector<2, T>::operator[](int index) const
		{
			return v[index];
		}

		template<typename T>
		inline T Vector<2, T>::norm_sqr() const
		{
			return v[0] * v[0] + v[1] * v[1];
		}

		template<typename T>
		inline T Vector<2, T>::norm() const
		{
			return T(sqrt(norm_sqr()));
		}

		template<typename T>
		template<typename U>
		inline Vector<2, T> Vector<2, T>::operator+(const Vector<2, U>& vec) const
		{
			return Vector<2, T>(v[0] + vec[0], v[1] + vec[1]);
		}

		template<typename T>
		template<typename U>
		inline Vector<2, T> Vector<2, T>::operator-(const Vector<2, U>& vec) const
		{
			return Vector<2, T>(v[0] - vec[0], v[1] - vec[1]);
		}

		template<typename T>
		template<typename U>
		inline Vector<2, T> Vector<2, T>::operator*(const Vector<2, U>& vec) const
		{
			return Vector<2, T>(v[0] * vec[0], v[1] * vec[1]);
		}

		template<typename T>
		template<typename U>
		inline Vector<2, T> Vector<2, T>::operator+(const U& s) const
		{
			return Vector<2, T>(v[0] + s, v[1] + s);
		}

		template<typename T>
		template<typename U>
		inline Vector<2, T> Vector<2, T>::operator-(const U& s) const
		{
			return Vector<2, T>(v[0] - s, v[1] - s);
		}

		template<typename T>
		template<typename U>
		inline Vector<2, T> Vector<2, T>::operator*(const U& s) const
		{
			return Vector<2, T>(v[0] * s, v[1] * s);
		}

		template<typename T>
		template<typename U>
		inline Vector<2, T> Vector<2, T>::operator/(const U& s) const
		{
			return *this * (T(1) / T(s));
		}

		template<typename T>
		template<typename U>
		inline Vector<2, U> Vector<2, T>::operator*(const Matrix<2, 2, U>& mat) const
		{
			return vecmulmat(*this, mat);
		}

		template<typename T>
		template<typename U>
		inline Vector<2, T>& Vector<2, T>::operator+=(const U& s)
		{
			*this = *this + s;
			return *this;
		}

		template<typename T>
		template<typename U>
		inline Vector<2, T>& Vector<2, T>::operator+=(const Vector<2, U>& s)
		{
			*this = *this + s;
			return *this;
		}

		template<typename T>
		template<typename U>
		inline Vector<2, T>& Vector<2, T>::operator-=(const U& s)
		{
			*this = *this - s;
			return *this;
		}

		template<typename T>
		template<typename U>
		inline Vector<2, T>& Vector<2, T>::operator-=(const Vector<2, U>& s)
		{
			*this = *this - s;
			return *this;
		}

		template<typename T>
		template<typename U>
		inline Vector<2, T>& Vector<2, T>::operator*=(const U& s)
		{
			*this = *this * s;
			return *this;
		}

		template<typename T>
		template<typename U>
		inline Vector<2, T>& Vector<2, T>::operator*=(const Vector<2, U>& s)
		{
			*this = *this * s;
			return *this;
		}

		template<typename T>
		template<typename U>
		inline Vector<2, T>& Vector<2, T>::operator/=(const U& s)
		{
			*this = *this / s;
			return *this;
		}

		template<typename T>
		inline Vector<2, T>& Vector<2, T>::operator*=(const Matrix<2, 2, T>& mat) const
		{
			*this = *this * mat;
			return *this;
		}

		template<typename T>
		template<typename U>
		inline T Vector<2, T>::dot(const Vector<2, U>& s) const
		{
			return T(v[0] * s[0] + v[1] * s[1]);
		}

		template<typename T>
		template<typename U>
		inline T Vector<2, T>::operator^(const Vector<2, U>& s)
		{
			return v[0] * s[1] - v[1] * s[0];
		}

		template<typename T>
		std::ostream& operator<<(std::ostream& os, const Vector<2, T>& vec)
		{
			os << '[' << vec[0] << ", " << vec[1] << ']';
			return os;
		}
	}
}