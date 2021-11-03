#pragma once

#include <cstdint>

namespace Grassland
{
	typedef int32_t GRL_RESULT;
#define GRL_TRUE 1
#define GRL_FALSE 0

	class GRLIBase
	{
		virtual uint32_t AddRef() = 0;
		virtual uint32_t Release() = 0;
		virtual uint32_t QueryInterface() = 0;
	};

	enum GRL_TYPE
	{
		GRL_TYPE_INT8 = 0,
		GRL_TYPE_UNSIGNED_INT8,
		GRL_TYPE_INT32,
		GRL_TYPE_UNSIGNED_INT32,
		GRL_TYPE_INT64,
		GRL_TYPE_UNSIGNED_INT64,
		GRL_TYPE_FLOAT,
		GRL_TYPE_DOUBLE
	};

	typedef uint8_t Byte;
	typedef float float32;
	typedef double float64;
}