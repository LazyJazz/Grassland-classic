#pragma once

#include "MathDecl.h"

namespace Grassland {
namespace Math {
template <typename T>
struct Vector<4, T> {
 private:
  T v[4];

 public:
  Vector();
  Vector(T v0, T v1, T v2, T v3);
  Vector(const T *pv);
  Vector(T scalar);

  T &operator[](int index);
  const T &operator[](int index) const;

  template <typename U>
  Vector<4, T> operator+(const Vector<4, U> &vec) const;
  template <typename U>
  Vector<4, T> operator-(const Vector<4, U> &vec) const;
  template <typename U>
  Vector<4, T> operator*(const Vector<4, U> &vec) const;

  template <typename U>
  Vector<4, T> operator+(const U &s) const;
  template <typename U>
  Vector<4, T> operator-(const U &s) const;
  template <typename U>
  Vector<4, T> operator*(const U &s) const;
  template <typename U>
  Vector<4, T> operator/(const U &s) const;

  template <typename U>
  Vector<4, U> operator*(const Matrix<4, 4, U> &mat) const;

  template <typename U>
  Vector<4, T> &operator+=(const U &s);
  template <typename U>
  Vector<4, T> &operator+=(const Vector<4, U> &s);
  template <typename U>
  Vector<4, T> &operator-=(const U &s);
  template <typename U>
  Vector<4, T> &operator-=(const Vector<4, U> &s);
  template <typename U>
  Vector<4, T> &operator*=(const U &s);
  template <typename U>
  Vector<4, T> &operator*=(const Vector<4, U> &s);
  template <typename U>
  Vector<4, T> &operator/=(const U &s);

  Vector<4, T> &operator*=(const Matrix<4, 4, T> &mat);

  T norm_sqr() const;
  T norm() const;

  template <typename U>
  T dot(const Vector<4, U> &s) const;
};

template <typename T>
std::ostream &operator<<(std::ostream &os, const Vector<4, T> &vec);
}  // namespace Math
}  // namespace Grassland

#include "Vector4.inl"
