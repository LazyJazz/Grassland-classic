#include "MathDecl.h"

template <>
double Grassland::Math::EPS() {
  return 1e-8;
}

template <>
float Grassland::Math::EPS() {
  return 1e-4f;
}

template <>
int Grassland::Math::EPS() {
  return 1;
}
