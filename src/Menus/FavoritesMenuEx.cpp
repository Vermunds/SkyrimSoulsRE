#include "Menus/FavoritesMenuEx.h"

namespace SkyrimSoulsRE
{
	RE::IMenu* FavoritesMenuEx::Creator()
	{
		return CreateMenu(RE::FavoritesMenu::MENU_NAME);
	}

	void FavoritesMenuEx::InstallHook()
	{
	}
}
