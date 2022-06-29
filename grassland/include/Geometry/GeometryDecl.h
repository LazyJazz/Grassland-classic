#pragma once
#include "../Math/Math.h"

namespace Grassland {
namespace Geometry {
template <int dimension, typename __Ty>
struct Ray {
  Math::Vector<dimension, __Ty> o;
  Math::Vector<dimension, __Ty> d;
  Ray() {
    o = Math::Vector<dimension, __Ty>();
    d = Math::Vector<dimension, __Ty>();
  }
  Ray(Math::Vector<dimension, __Ty> _o, Math::Vector<dimension, __Ty> _d) {
    o = _o;
    d = _d;
  }
};
}  // namespace Geometry
typedef Geometry::Ray<3, float> GRLRay3;
typedef Geometry::Ray<4, float> GRLRay4;
typedef Geometry::Ray<3, double> GRLRay3d;
typedef Geometry::Ray<4, double> GRLRay4d;
typedef GRLRay3 GRLRay;
typedef GRLRay3d GRLRayd;
}  // namespace Grassland
