#pragma once
#include "../../Math/Math.h"

namespace Grassland {
template <typename __Ty>
Math::Matrix<4, 4, __Ty> GRLTransformProjection(__Ty FOVangle,
                                                __Ty aspect,
                                                __Ty Near,
                                                __Ty Far);

template <typename __Ty>
Math::Matrix<4, 4, __Ty> GRLTransformTranslate(__Ty x, __Ty y, __Ty z);

template <typename __Ty>
Math::Matrix<4, 4, __Ty> GRLTransformRotation(__Ty pitch, __Ty yaw, __Ty roll);

template <typename __Ty>
Math::Matrix<4, 4, __Ty> GRLTransformLookAt(Math::Vector<3, __Ty> from,
                                            Math::Vector<3, __Ty> look_at);
}  // namespace Grassland

#include "Transform.inl"
