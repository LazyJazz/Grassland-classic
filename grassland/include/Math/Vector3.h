#pragma once

#include "MathDecl.h"
#include "MatrixCommon.h"

namespace Grassland {
namespace Math {
template <typename T>
struct Vector<3, T> {
 private:
  T v[3];

 public:
  Vector();
  Vector(T v0, T v1, T v2);
  Vector(const T *pv);
  Vector(T scalar);

  T &operator[](int index);
  const T &operator[](int index) const;

  template <typename U>
  Vector<3, T> operator+(const Vector<3, U> &vec) const;
  template <typename U>
  Vector<3, T> operator-(const Vector<3, U> &vec) const;
  template <typename U>
  Vector<3, T> operator*(const Vector<3, U> &vec) const;

  template <typename U>
  Vector<3, T> operator+(const U &s) const;
  template <typename U>
  Vector<3, T> operator-(const U &s) const;
  template <typename U>
  Vector<3, T> operator*(const U &s) const;
  template <typename U>
  Vector<3, T> operator/(const U &s) const;

  template <typename U>
  Vector<3, U> operator*(const Matrix<3, 3, U> &mat) const;

  template <typename U>
  Vector<3, T> &operator+=(const U &s);
  template <typename U>
  Vector<3, T> &operator+=(const Vector<3, U> &s);
  template <typename U>
  Vector<3, T> &operator-=(const U &s);
  template <typename U>
  Vector<3, T> &operator-=(const Vector<3, U> &s);
  template <typename U>
  Vector<3, T> &operator*=(const U &s);
  template <typename U>
  Vector<3, T> &operator*=(const Vector<3, U> &s);
  template <typename U>
  Vector<3, T> &operator/=(const U &s);

  Vector<3, T> &operator*=(const Matrix<3, 3, T> &mat);

  T norm_sqr() const;
  T norm() const;

  template <typename U>
  T dot(const Vector<3, U> &s) const;

  template <typename U>
  Vector<3, T> operator^(const Vector<3, U> &s);

  template <typename U>
  Vector<3, T> &operator^=(const Vector<3, U> &s);
};

template <typename T>
std::ostream &operator<<(std::ostream &os, const Vector<3, T> &vec);
}  // namespace Math
}  // namespace Grassland

#include "Vector3.inl"
