#include "Menus/GiftMenuEx.h"

namespace SkyrimSoulsRE
{
	RE::UI_MESSAGE_RESULTS GiftMenuEx::ProcessMessage_Hook(RE::UIMessage& a_message)
	{
		AutoCloseManager* autoCloseManager = AutoCloseManager::GetSingleton();

		switch (a_message.type.get())
		{
		case RE::UI_MESSAGE_TYPE::kShow:
			{
				RE::RefHandle handle = IsPlayerGifting() ? GetReceiverRefHandle() : GetGifterRefHandle();
				if (handle)
				{
					autoCloseManager->InitAutoClose(RE::GiftMenu::MENU_NAME, handle, true);
				}

				SendSetSkyrimSoulsHUDModeMessage(true);
			}
			break;

		case RE::UI_MESSAGE_TYPE::kUpdate:
			UpdateBottomBar();
			autoCloseManager->CheckAutoClose(RE::GiftMenu::MENU_NAME);
			break;

		case RE::UI_MESSAGE_TYPE::kHide:
			SendSetSkyrimSoulsHUDModeMessage(false);
			break;
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
		return menu;
	}

	void GiftMenuEx::InstallHook()
	{
		REL::Relocation<std::uintptr_t> vTable(RE::VTABLE_GiftMenu[0]);
		_ProcessMessage = vTable.write_vfunc(0x4, &GiftMenuEx::ProcessMessage_Hook);
	}
}
