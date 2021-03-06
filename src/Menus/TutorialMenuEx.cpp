#include "Menus/TutorialMenuEx.h"

namespace SkyrimSoulsRE
{
	RE::IMenu* TutorialMenuEx::Creator()
	{
		return CreateMenu(RE::TutorialMenu::MENU_NAME);
	}

	void TutorialMenuEx::InstallHook()
	{

	}
}
