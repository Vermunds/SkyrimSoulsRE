#include "Menus/Hooks_ModManagerMenu.h"

namespace SkyrimSoulsRE
{
	RE::IMenu* ModManagerMenuEx::Creator()
	{
		return CreateMenu(RE::ModManagerMenu::MENU_NAME);
	}

	void ModManagerMenuEx::InstallHook()
	{

	}
}
