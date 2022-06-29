#pragma once

#include "MathDecl.h"
#include "MatrixCommon.h"

namespace Grassland {
namespace Math {
template <typename T>
struct Vector<2, T> {
 private:
  T v[2];

 public:
  Vector();
  Vector(T v0, T v1);
  Vector(const T *pv);
  Vector(T scalar);

  T &operator[](int index);
  const T &operator[](int index) const;

  template <typename U>
  Vector<2, T> operator+(const Vector<2, U> &vec) const;
  template <typename U>
  Vector<2, T> operator-(const Vector<2, U> &vec) const;
  template <typename U>
  Vector<2, T> operator*(const Vector<2, U> &vec) const;

  template <typename U>
  Vector<2, T> operator+(const U &s) const;
  template <typename U>
  Vector<2, T> operator-(const U &s) const;
  template <typename U>
  Vector<2, T> operator*(const U &s) const;
  template <typename U>
  Vector<2, T> operator/(const U &s) const;
  template <typename U>
  Vector<2, U> operator*(const Matrix<2, 2, U> &mat) const;

  template <typename U>
  Vector<2, T> &operator+=(const U &s);
  template <typename U>
  Vector<2, T> &operator+=(const Vector<2, U> &s);
  template <typename U>
  Vector<2, T> &operator-=(const U &s);
  template <typename U>
  Vector<2, T> &operator-=(const Vector<2, U> &s);
  template <typename U>
  Vector<2, T> &operator*=(const U &s);
  template <typename U>
  Vector<2, T> &operator*=(const Vector<2, U> &s);
  template <typename U>
  Vector<2, T> &operator/=(const U &s);

  Vector<2, T> &operator*=(const Matrix<2, 2, T> &mat) const;

  T norm_sqr() const;
  T norm() const;

  template <typename U>
  T dot(const Vector<2, U> &s) const;

  template <typename U>
  T operator^(const Vector<2, U> &s);
};

template <typename T>
std::ostream &operator<<(std::ostream &os, const Vector<2, T> &vec);
}  // namespace Math
}  // namespace Grassland

#include "Vector2.inl"
