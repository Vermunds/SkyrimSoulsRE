#include "Menus/BarterMenuEx.h"

namespace SkyrimSoulsRE
{
	RE::UI_MESSAGE_RESULTS BarterMenuEx::ProcessMessage_Hook(RE::UIMessage& a_message)
	{
		AutoCloseManager* autoCloseManager = AutoCloseManager::GetSingleton();

		switch (a_message.type.get())
		{
		case RE::UI_MESSAGE_TYPE::kShow:

			SendSetSkyrimSoulsHUDModeMessage(true);
			if (RE::RefHandle handle = GetTargetRefHandle())
			{
				autoCloseManager = AutoCloseManager::GetSingleton();
				autoCloseManager->InitAutoClose(RE::BarterMenu::MENU_NAME, handle, true);
			}
			break;

		case RE::UI_MESSAGE_TYPE::kUpdate:
			autoCloseManager->CheckAutoClose(RE::BarterMenu::MENU_NAME);
			UpdateBottomBar();
			break;

		case RE::UI_MESSAGE_TYPE::kHide:
			SendSetSkyrimSoulsHUDModeMessage(false);
			break;
		}

		return _ProcessMessage(this, a_message);
	}

	void BarterMenuEx::UpdateBottomBar()
	{
		using func_t = decltype(&BarterMenuEx::UpdateBottomBar);
		REL::Relocation<func_t> func(Offsets::Menus::BarterMenu::UpdateBottomBar);
		return func(this);
	}

	RE::IMenu* BarterMenuEx::Creator()
	{
		RE::BarterMenu* menu = static_cast<RE::BarterMenu*>(CreateMenu(RE::BarterMenu::MENU_NAME));
		return menu;
	}

	void BarterMenuEx::InstallHook()
	{
		//Hook AdvanceMovie + ProcessMessage
		REL::Relocation<std::uintptr_t> vTable(RE::VTABLE_BarterMenu[0]);
		_ProcessMessage = vTable.write_vfunc(0x4, &BarterMenuEx::ProcessMessage_Hook);
	}
}
