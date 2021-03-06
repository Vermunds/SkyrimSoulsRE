#include "Menus/CustomMenuEx.h"

namespace SkyrimSoulsRE
{
	RE::IMenu* CustomMenuEx::Creator()
	{
		return CreateMenu("CustomMenu");
	}
}
