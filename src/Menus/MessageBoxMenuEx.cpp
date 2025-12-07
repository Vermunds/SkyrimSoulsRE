#include "Menus/MessageBoxMenuEx.h"

namespace SkyrimSoulsRE
{
	RE::UI_MESSAGE_RESULTS MessageBoxMenuEx::ProcessMessage_Hook(RE::UIMessage& a_message)
	{
		if (a_message.type == RE::UI_MESSAGE_TYPE::kUpdate)
		{
			RE::UIMessageQueue::GetSingleton()->AddMessage(RE::HUDMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kUpdate, nullptr);
		}

		return _ProcessMessage(this, a_message);
	}

	RE::IMenu* MessageBoxMenuEx::Creator()
	{
		RE::MessageBoxMenu* menu = static_cast<RE::MessageBoxMenu*>(CreateMenu(RE::MessageBoxMenu::MENU_NAME));

		return menu;
	}

	void MessageBoxMenuEx::InstallHook()
	{
		//Hook AdvanceMovie
		REL::Relocation<std::uintptr_t> vTable(RE::VTABLE_MessageBoxMenu[0]);
		_ProcessMessage = vTable.write_vfunc(0x4, &MessageBoxMenuEx::ProcessMessage_Hook);
	}
}
