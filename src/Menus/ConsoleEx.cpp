#include "Menus/ConsoleEx.h"

namespace SkyrimSoulsRE
{
	RE::IMenu* ConsoleEx::Creator()
	{
		RE::Console* menu = static_cast<RE::Console*>(CreateMenu(RE::Console::MENU_NAME));
		return menu;
	}

	void ConsoleEx::InstallHook()
	{
	}
}
