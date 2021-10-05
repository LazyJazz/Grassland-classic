#pragma once
#include <cmath>
#include <iostream>

namespace Grassland
{
	namespace Math
	{
		template<typename T> T Pi() { return 3.14159265358979323846264338327950288419716939937510; };
		template<typename T> T EPS();
		
		template<>
		double EPS() { return 1e-8; }

		template<>
		float EPS() { return 1e-4; }

		template<>
		int EPS() { return 1; }

		template<int n, typename T> struct Vector;
		template<int R, int C, typename T> struct Matrix;
	}
}
