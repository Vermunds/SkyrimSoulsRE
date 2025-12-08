#include "Menus/BarterMenuEx.h"

namespace SkyrimSoulsRE
{
	RE::UI_MESSAGE_RESULTS BarterMenuEx::ProcessMessage_Hook(RE::UIMessage& a_message)
	{
		switch (a_message.type.get())
		{
		case RE::UI_MESSAGE_TYPE::kShow:
			{
				RE::RefHandle handle = this->GetTargetRefHandle();
				if (handle)
				{
					AutoCloseManager* autoCloseManager = AutoCloseManager::GetSingleton();
					autoCloseManager->InitAutoClose(RE::BarterMenu::MENU_NAME, handle, true);
				}

				HUDMenuEx* hudMenu = static_cast<HUDMenuEx*>(
					RE::UI::GetSingleton()->GetMenu(RE::InterfaceStrings::GetSingleton()->hudMenu).get());
				if (hudMenu)
				{
					hudMenu->SetSkyrimSoulsMode(true);
				}
				break;
			}

		case RE::UI_MESSAGE_TYPE::kUpdate:
			{
				AutoCloseManager::GetSingleton()->CheckAutoClose(RE::BarterMenu::MENU_NAME);
				break;
			}

		case RE::UI_MESSAGE_TYPE::kHide:
			{
				HUDMenuEx* hudMenu = static_cast<HUDMenuEx*>(RE::UI::GetSingleton()->GetMenu(RE::InterfaceStrings::GetSingleton()->hudMenu).get());
				if (hudMenu)
				{
					hudMenu->SetSkyrimSoulsMode(false);
				}
				break;
			}
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
