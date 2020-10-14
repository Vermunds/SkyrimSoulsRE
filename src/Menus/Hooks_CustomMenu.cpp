#include "Menus/Hooks_CustomMenu.h"

namespace SkyrimSoulsRE
{
	RE::IMenu* CustomMenuEx::Creator()
	{
		return CreateMenu("CustomMenu");
	}
}

