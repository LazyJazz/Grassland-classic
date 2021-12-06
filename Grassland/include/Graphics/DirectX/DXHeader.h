#pragma once
#include "d3dx12.h"
#include <dxgi1_6.h>
#include <d3d12shader.h>
#include <d3d12sdklayers.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include "../../GrasslandDecl.h"
#include "../../String/String.h"
#include <map>

#include <iostream>

namespace Grassland
{
	HRESULT GRLDirectXErrorReport(HRESULT hr, const char * code, const char * file, int line);

#define GRLComCall(x) GRLDirectXErrorReport(x, #x, __FILE__, __LINE__)
}