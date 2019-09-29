#include "Utility.h"

#include <sstream>  // stringstream
#include <string>  // string

namespace SkyrimSoulsRE
{

	bool StrToInt(SInt32& i, const char* s)
	{
		char c;
		std::stringstream ss(s);
		ss >> i;
		if (ss.fail() || ss.get(c)) {
			// not an integer
			return false;
		}
		return true;
	}
}
