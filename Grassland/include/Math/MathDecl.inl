#include"MathDecl.h"

namespace Grassland
{
	namespace Math
	{
		template<typename T>
		T radian(T deg)
		{
			return deg * Pi<T>() / 180.0;
		}

		template<typename T>
		T degree(T rad)
		{
			return rad / Pi<T>() * 180.0;
		}
	}
}