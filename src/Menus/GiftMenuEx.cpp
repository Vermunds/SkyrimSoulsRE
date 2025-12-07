#include "Menus/GiftMenuEx.h"

namespace SkyrimSoulsRE
{
	RE::UI_MESSAGE_RESULTS GiftMenuEx::ProcessMessage_Hook(RE::UIMessage& a_message)
	{
		if (a_message.type == RE::UI_MESSAGE_TYPE::kHide)
		{
			HUDMenuEx* hudMenu = static_cast<HUDMenuEx*>(RE::UI::GetSingleton()->GetMenu(RE::InterfaceStrings::GetSingleton()->hudMenu).get());
			if (hudMenu)
			{
				hudMenu->SetSkyrimSoulsMode(false);
			}
		}
		if (a_message.type == RE::UI_MESSAGE_TYPE::kUpdate)
		{
			this->UpdateBottomBar();
			AutoCloseManager::GetSingleton()->CheckAutoClose(RE::GiftMenu::MENU_NAME);
		}

		return _ProcessMessage(this, a_message);
	}

	void GiftMenuEx::UpdateBottomBar()
	{
		using func_t = decltype(&GiftMenuEx::UpdateBottomBar);
		REL::Relocation<func_t> func(Offsets::Menus::GiftMenu::UpdateBottomBar);
		return func(this);
	}

	RE::IMenu* GiftMenuEx::Creator()
	{
		RE::GiftMenu* menu = static_cast<RE::GiftMenu*>(CreateMenu(RE::GiftMenu::MENU_NAME));

		RE::RefHandle handle = menu->IsPlayerGifting() ? menu->GetReceiverRefHandle() : menu->GetGifterRefHandle();
		if (handle)
		{
			AutoCloseManager* autoCloseManager = AutoCloseManager::GetSingleton();
			autoCloseManager->InitAutoClose(RE::GiftMenu::MENU_NAME, handle, true);
		}

		HUDMenuEx* hudMenu = static_cast<HUDMenuEx*>(RE::UI::GetSingleton()->GetMenu(RE::InterfaceStrings::GetSingleton()->hudMenu).get());
		if (hudMenu)
		{
			hudMenu->SetSkyrimSoulsMode(true);
		}

		return menu;
	}

	void GiftMenuEx::InstallHook()
	{
		REL::Relocation<std::uintptr_t> vTable(RE::VTABLE_GiftMenu[0]);
		_ProcessMessage = vTable.write_vfunc(0x4, &GiftMenuEx::ProcessMessage_Hook);
	}
}
