#pragma once
#include "MatrixCommon.h"
#include "Vector.h"

namespace Grassland {
namespace Math {
template <typename T>
struct Matrix<2, 2, T> {
 private:
  Vector<2, T> m[2];

 public:
  Matrix();
  Matrix(T m00, T m01, T m10, T m11);
  Matrix(T scalar);

  Vector<2, T> &operator[](int index);
  const Vector<2, T> &operator[](int index) const;

  template <typename U>
  Matrix<2, 2, T> operator+(const Matrix<2, 2, U> &mat) const;
  template <typename U>
  Matrix<2, 2, T> operator-(const Matrix<2, 2, U> &mat) const;
  template <typename U>
  Matrix<2, 2, T> operator*(const Matrix<2, 2, U> &mat) const;
  template <typename U>
  Matrix<2, 2, T> operator*(const U s) const;
  template <typename U>
  Matrix<2, 2, T> operator/(const U s) const;

  template <typename U>
  Matrix<2, 2, T> &operator+=(const Matrix<2, 2, U> &mat);
  template <typename U>
  Matrix<2, 2, T> &operator-=(const Matrix<2, 2, U> &mat);
  template <typename U>
  Matrix<2, 2, T> &operator*=(const Matrix<2, 2, U> &mat);
  template <typename U>
  Matrix<2, 2, T> &operator*=(const U s);
  template <typename U>
  Matrix<2, 2, T> &operator/=(const U s);

  template <typename U>
  Vector<2, T> operator*(const Vector<2, U> &vec) const;

  Matrix<2, 2, T> inverse() const;
  T determinant() const;
};

template <typename T>
std::ostream &operator<<(std::ostream &os, const Matrix<2, 2, T> &vec);
}  // namespace Math
}  // namespace Grassland

#include "Matrix2x2.inl"
