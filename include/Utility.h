#pragma once

#include <string>  // string


namespace SkyrimSoulsRE
{
	std::string boolToString(bool a_bool);
	std::string	numToHexString(UInt64 a_num, UInt64 a_bytes = 4);
}
