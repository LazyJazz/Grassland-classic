#include "DXHeader.h"

namespace Grassland
{
	HRESULT GRLDirectXErrorReport(HRESULT hr, const char* code, const char* file, int line)
	{
		if (FAILED(hr))
		{
			std::cout << "[ComCall Error!] code: " << std::hex << hr << std::endl
				<< '(' << file << ":" << line << ')' << code << std::endl;
		}
		return hr;
	}
}