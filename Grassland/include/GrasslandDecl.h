#pragma once

#include <cstdint>

namespace Grassland
{
#define GRASSLAND_TRUE 1
#define GRASSLAND_FALSE 0

	class GrIBase
	{
		virtual uint32_t AddRef() = 0;
		virtual uint32_t Release() = 0;
		virtual uint32_t QueryInterface() = 0;
	};

	typedef uint8_t Byte;
	typedef float float32;
	typedef double float64;
}