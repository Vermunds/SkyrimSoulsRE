#include "Menus/Hooks_TrainingMenu.h"

namespace SkyrimSoulsRE
{
	RE::IMenu* TrainingMenuEx::Creator()
	{
		return CreateMenu(RE::TrainingMenu::MENU_NAME);
	}

	void TrainingMenuEx::InstallHook()
	{
		
	}
}

