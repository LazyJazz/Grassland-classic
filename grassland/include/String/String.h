#pragma once
#include "../GrasslandDecl.h"
#include <string>
#include <cstring>

namespace Grassland
{
	int32_t GRLStringUTF8toUnicode(const char* utf8_str, wchar_t* unicode_str);

	std::wstring GRLStringUTF8toUnicode(std::string utf8_str);

	std::string GRLStringUnicodetoUTF8(std::wstring unicode_str);

}