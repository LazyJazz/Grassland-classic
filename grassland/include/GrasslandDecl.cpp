#include "GrasslandDecl.h"

namespace Grassland
{
	GRLUUID::GRLUUID()
	{
		for (int i = 0; i < 16; i++)
			__uuid_bytes[i] = 0;
	}
	GRLUUID::GRLUUID(const char* uuid_str)
	{
		for (int i = 0; i < 16; i++)
			__uuid_bytes[i] = 0;
		int index = 0;
		int len = 0;
		while (uuid_str[len]) len++;
		for (int i = 0; i < len; i++)
		{
			int c = uuid_str[i], digit;
			if (isalnum(c))
			{
				if (index < 32)
				{
					if (isdigit(c)) digit = c - '0';
					else if (islower(c)) digit = c - 'a' + 10;
					else digit = c - 'A' + 10;
					__uuid_bytes[index >> 1] |= (digit << (4 * ((index & 1) ^ 1)));
				}
				index++;
			}
		}
	}
	bool GRLUUID::operator==(const GRLUUID& __another_uuid) const
	{
		return ((__uuid_int64[0] == __another_uuid.__uuid_int64[0]) && (__uuid_int64[1] == __another_uuid.__uuid_int64[1]));
	}
	std::ostream& operator<<(std::ostream& os, const GRLUUID& uuid)
	{
		os << std::hex << std::setw(2) << std::setfill('0') << (uint32_t)uuid.__uuid_bytes[0];
		os << std::hex << std::setw(2) << std::setfill('0') << (uint32_t)uuid.__uuid_bytes[1];
		os << std::hex << std::setw(2) << std::setfill('0') << (uint32_t)uuid.__uuid_bytes[2];
		os << std::hex << std::setw(2) << std::setfill('0') << (uint32_t)uuid.__uuid_bytes[3];
		os << '-';
		os << std::hex << std::setw(2) << std::setfill('0') << (uint32_t)uuid.__uuid_bytes[4];
		os << std::hex << std::setw(2) << std::setfill('0') << (uint32_t)uuid.__uuid_bytes[5];
		os << '-';
		os << std::hex << std::setw(2) << std::setfill('0') << (uint32_t)uuid.__uuid_bytes[6];
		os << std::hex << std::setw(2) << std::setfill('0') << (uint32_t)uuid.__uuid_bytes[7];
		os << '-';
		os << std::hex << std::setw(2) << std::setfill('0') << (uint32_t)uuid.__uuid_bytes[8];
		os << std::hex << std::setw(2) << std::setfill('0') << (uint32_t)uuid.__uuid_bytes[9];
		os << '-';
		os << std::hex << std::setw(2) << std::setfill('0') << (uint32_t)uuid.__uuid_bytes[10];
		os << std::hex << std::setw(2) << std::setfill('0') << (uint32_t)uuid.__uuid_bytes[11];
		os << std::hex << std::setw(2) << std::setfill('0') << (uint32_t)uuid.__uuid_bytes[12];
		os << std::hex << std::setw(2) << std::setfill('0') << (uint32_t)uuid.__uuid_bytes[13];
		os << std::hex << std::setw(2) << std::setfill('0') << (uint32_t)uuid.__uuid_bytes[14];
		os << std::hex << std::setw(2) << std::setfill('0') << (uint32_t)uuid.__uuid_bytes[15];
		return os;
	}
}