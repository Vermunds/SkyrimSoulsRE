#include "Menus/InventoryMenuEx.h"

namespace SkyrimSoulsRE
{
	RE::UI_MESSAGE_RESULTS InventoryMenuEx::ProcessMessage_Hook(RE::UIMessage& a_message)
	{
		switch (a_message.type.get())
		{
		case RE::UI_MESSAGE_TYPE::kShow:
			SendSetSkyrimSoulsHUDModeMessage(true);
			break;

		case RE::UI_MESSAGE_TYPE::kUpdate:
			UpdateBottomBar();
			break;

		case RE::UI_MESSAGE_TYPE::kHide:
			SendSetSkyrimSoulsHUDModeMessage(false);
			break;
		}

		return _ProcessMessage(this, a_message);
	}

	void InventoryMenuEx::UpdateBottomBar()
	{
		using func_t = decltype(&InventoryMenuEx::UpdateBottomBar);
		REL::Relocation<func_t> func(Offsets::Menus::InventoryMenu::UpdateBottomBar);
		return func(this);
	}

	RE::IMenu* InventoryMenuEx::Creator()
	{
		RE::InventoryMenu* menu = static_cast<RE::InventoryMenu*>(CreateMenu(RE::InventoryMenu::MENU_NAME));
		return menu;
	}

	void InventoryMenuEx::InstallHook()
	{
		REL::Relocation<std::uintptr_t> vTable(RE::VTABLE_InventoryMenu[0]);
		_ProcessMessage = vTable.write_vfunc(0x4, &InventoryMenuEx::ProcessMessage_Hook);
	}
};
