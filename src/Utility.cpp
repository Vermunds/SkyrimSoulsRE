#include "Utility.h"

#include <cctype>  // toupper
#include <ios>  // hex
#include <sstream>  // stringstream
#include <string>  // string, strlen


namespace SkyrimSoulsRE
{

	bool StrToInt(SInt32& i, char const* s)
	{
		char              c;
		std::stringstream ss(s);
		ss >> i;
		if (ss.fail() || ss.get(c)) {
			// not an integer
			return false;
		}
		return true;
	}
}
