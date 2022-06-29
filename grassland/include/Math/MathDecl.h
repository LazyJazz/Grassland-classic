#pragma once
#include <cmath>
#include <iostream>

namespace Grassland {
namespace Math {
const double pi = 3.14159265358979323846264338327950288419716939937510;
const double epsd = 1e-8;
const double eps = 1e-4;

template <typename T>
T Pi() {
  return 3.14159265358979323846264338327950288419716939937510;
};
template <typename T>
T EPS();

template <>
double EPS();

template <>
float EPS();

template <>
int EPS();

template <typename T>
T radian(T deg);

template <typename T>
T degree(T rad);

template <int n, typename T>
struct Vector;
template <int R, int C, typename T>
struct Matrix;
}  // namespace Math

template <typename T>
T GRLRadian(T deg) {
  return Math::radian<T>(deg);
}

template <typename T>
T GRLDegree(T rad) {
  return Math::degree<T>(rad);
}
}  // namespace Grassland

#include "MathDecl.inl"
