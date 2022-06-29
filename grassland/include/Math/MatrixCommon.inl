#pragma once
#include "MatrixCommon.h"

namespace Grassland
{
	namespace Math
	{
		template<int N, int M, int K, typename T, typename U>
		inline Matrix<N, K, T> matmul(const Matrix<N, M, T>& mat1, const Matrix<M, K, U>& mat2)
		{
			Matrix<N, K, T> res;
			for (int i = 0; i < N; i++)
			{
				for (int k = 0; k < K; k++)
				{
					res[i][k] = 0;
					for (int j = 0; j < M; j++)
					{
						res[i][k] += mat1[i][j] * mat2[j][k];
					}
				}
			}
			return res;
		}

		template<int N, int M, typename T, typename U>
		inline Vector<N, T> matmulvec(const Matrix<N, M, T>& mat, const Vector<M, U>& vec)
		{
			Vector<N, T> res;
			for (int i = 0; i < N; i++)
				res[i] = mat[i].dot(vec);
			return res;
		}

		template<int N, int M, typename T, typename U>
		inline Vector<M, T> vecmulmat(const Vector<N, U>& vec, const Matrix<N, M, T>& mat)
		{
			Vector<M, T> res;
			for (int i = 0; i < M; i++)
			{
				res[i] = 0;
				for (int j = 0; j < N; j++)
					res[i] += vec[j] * (mat[j][i]);
			}
			return res;
		}

		template<int N, typename T>
		Matrix<N, N, T> matinverse(Matrix<N, N, T> mat)
		{
			Matrix<N, N, T> res(1);
			for (int i = 0; i < N; i++)
			{
				T max_comp = 0; int max_index = i;
				for (int j = i; j < N; j++)
					if (abs(mat[j][i]) > max_comp)
					{
						max_comp = abs(mat[j][i]);
						max_index = j;
					}
				if (max_index != i)
				{
					Vector<N, T> buffer;
					buffer = mat[max_index];
					mat[max_index] = mat[i];
					mat[i] = buffer;
					buffer = res[max_index];
					res[max_index] = res[i];
					res[i] = buffer;
				}

				if (max_comp < EPS<T>()) return res;

				max_comp = mat[i][i];
				for (int j = i; j < N; j++)
					mat[i][j] /= max_comp;
				for (int j = 0; j < N; j++)
					res[i][j] /= max_comp;
				for (int j = i + 1; j < N; j++)
					if (abs(mat[j][i]) > EPS<T>())
						max_comp = mat[j][i],
						mat[j] -= mat[i] * max_comp,
						res[j] -= res[i] * max_comp;
			}
			for (int i = N - 1; i >= 0; i--)
			{
				T a;
				for (int j = i - 1; j >= 0; j--)
					if (abs(mat[j][i]) > EPS<T>())
						a = mat[j][i],
						mat[j] -= mat[i] * a,
						res[j] -= res[i] * a;
			}
			return res;
		}

		template<int N, typename T>
		T matdeterminant(Matrix<N, N, T> mat)
		{
			T res = 1.0;
			for (int i = 0; i < N; i++)
			{
				T max_comp = 0; int max_index = i;
				for (int j = i; j < N; j++)
					if (abs(mat[j][i]) > max_comp)
					{
						max_comp = abs(mat[j][i]);
						max_index = j;
					}
				if (max_index != i)
				{
					Vector<N, T> buffer;
					buffer = mat[max_index];
					mat[max_index] = mat[i];
					mat[i] = buffer;
					res = -res;
				}

				if (max_comp < EPS<T>()) return res;

				max_comp = mat[i][i];
				res *= max_comp;
				for (int j = i; j < N; j++)
					mat[i][j] /= max_comp;
				for (int j = i + 1; j < N; j++)
					if (abs(mat[j][i]) > EPS<T>())
						max_comp = mat[j][i],
						mat[j] -= mat[i] * max_comp;
			}
			return res;
		}
	}
}
