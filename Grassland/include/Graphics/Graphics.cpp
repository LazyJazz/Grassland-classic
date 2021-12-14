#include "Graphics.h"
#include "DirectX/GraphicsDirectX.h"

namespace Grassland
{
	uint32_t GRLFormatSizeInByte(GRL_FORMAT format)
	{
		switch (format)
		{
		case GRL_FORMAT::BYTE: return 1;
		case GRL_FORMAT::BYTE2: return 2;
		case GRL_FORMAT::BYTE4: return 4;
		case GRL_FORMAT::FLOAT: return 4;
		case GRL_FORMAT::FLOAT2: return 8;
		case GRL_FORMAT::FLOAT3: return 12;
		case GRL_FORMAT::FLOAT4: return 16;
		case GRL_FORMAT::INT: return 4;
		case GRL_FORMAT::INT2: return 8;
		case GRL_FORMAT::INT3: return 12;
		case GRL_FORMAT::INT4: return 16;
		case GRL_FORMAT::UINT: return 4;
		case GRL_FORMAT::UINT2: return 8;
		case GRL_FORMAT::UINT3: return 12;
		case GRL_FORMAT::UINT4: return 16;
		}
		return 0;
	}
	GRL_RESULT GRLCreateGraphicsEnvironment(uint32_t width, uint32_t height, const char* window_title, GRL_GRAPHICS_API graphicsAPI, GRLIGraphicsEnvironment** ppEnvironment)
	{
		switch (graphicsAPI)
		{
#if defined(_WIN32)
		case GRL_GRAPHICS_API::D3D12:
			return GRLCreateD3D12Environment(width, height, window_title, ppEnvironment);
#endif
		case GRL_GRAPHICS_API::OPENGL:
			return GRL_FALSE;
		}
		return GRL_FALSE;
	};
}