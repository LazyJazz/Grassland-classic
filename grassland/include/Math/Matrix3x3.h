#pragma once
#include "MatrixCommon.h"
#include "Vector.h"

namespace Grassland {
namespace Math {
template <typename T>
struct Matrix<3, 3, T> {
 private:
  Vector<3, T> m[3];

 public:
  Matrix();
  Matrix(T m00, T m01, T m02, T m10, T m11, T m12, T m20, T m21, T m22);
  Matrix(T scalar);
  Matrix(const Matrix<4, 4, T> &mat4);

  Vector<3, T> &operator[](int index);
  const Vector<3, T> &operator[](int index) const;

  template <typename U>
  Matrix<3, 3, T> operator+(const Matrix<3, 3, U> &mat) const;
  template <typename U>
  Matrix<3, 3, T> operator-(const Matrix<3, 3, U> &mat) const;
  template <typename U>
  Matrix<3, 3, T> operator*(const Matrix<3, 3, U> &mat) const;
  template <typename U>
  Matrix<3, 3, T> operator*(const U s) const;
  template <typename U>
  Matrix<3, 3, T> operator/(const U s) const;

  template <typename U>
  Matrix<3, 3, T> &operator+=(const Matrix<3, 3, U> &mat);
  template <typename U>
  Matrix<3, 3, T> &operator-=(const Matrix<3, 3, U> &mat);
  template <typename U>
  Matrix<3, 3, T> &operator*=(const Matrix<3, 3, U> &mat);
  template <typename U>
  Matrix<3, 3, T> &operator*=(const U s);
  template <typename U>
  Matrix<3, 3, T> &operator/=(const U s);

  template <typename U>
  Vector<3, T> operator*(const Vector<3, U> &vec) const;

  Matrix<3, 3, T> inverse() const;
  T determinant() const;
};

template <typename T>
std::ostream &operator<<(std::ostream &os, const Matrix<3, 3, T> &vec);
}  // namespace Math
}  // namespace Grassland

#include "Matrix3x3.inl"
