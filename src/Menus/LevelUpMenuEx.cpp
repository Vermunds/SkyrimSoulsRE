#include "Menus/LevelUpMenuEx.h"

namespace SkyrimSoulsRE
{
	RE::IMenu* LevelUpMenuEx::Creator()
	{
		RE::LevelUpMenu* menu = static_cast<RE::LevelUpMenu*>(CreateMenu(RE::LevelUpMenu::MENU_NAME));
		return menu;
	}

	void LevelUpMenuEx::InstallHook()
	{
	}
}
