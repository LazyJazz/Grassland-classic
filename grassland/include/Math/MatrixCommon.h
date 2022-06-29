#pragma once
#include "MathDecl.h"

namespace Grassland {
namespace Math {
template <int N, int M, int K, typename T, typename U>
Matrix<N, K, T> matmul(const Matrix<N, M, T> &mat1,
                       const Matrix<M, K, U> &mat2);

template <int N, int M, typename T, typename U>
Vector<N, T> matmulvec(const Matrix<N, M, T> &mat, const Vector<M, U> &vec);

template <int N, int M, typename T, typename U>
Vector<M, T> vecmulmat(const Vector<N, U> &vec, const Matrix<N, M, T> &mat);

template <int N, typename T>
Matrix<N, N, T> matinverse(Matrix<N, N, T> mat);

template <int N, typename T>
T matdeterminant(Matrix<N, N, T> mat);

}  // namespace Math
}  // namespace Grassland

#include "MatrixCommon.inl"
