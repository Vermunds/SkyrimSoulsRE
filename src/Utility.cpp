#include "Utility.h"

#include <cctype>  // toupper
#include <ios>  // hex
#include <sstream>  // stringstream
#include <string>  // string, strlen


namespace SkyrimSoulsRE
{
	std::string boolToString(bool a_bool)
	{
		return a_bool ? "True" : "False";
	}


	std::string numToHexString(UInt64 a_num, UInt64 a_bytes)
	{
		// Convert to hex
		std::stringstream sstream;
		sstream << std::hex << a_num;
		std::string hexStr = sstream.str();

		// Convert lowercase characters to uppercase
		for (auto& c : hexStr) {
			c = toupper(c);
		}

		// Sign extension
		while (std::strlen(hexStr.c_str()) < a_bytes * 2) {
			hexStr = '0' + hexStr;
		}

		return hexStr;
	}
}
