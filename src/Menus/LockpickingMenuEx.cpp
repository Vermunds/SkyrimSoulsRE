#include "Menus/LockpickingMenuEx.h"

namespace SkyrimSoulsRE
{
	RE::UI_MESSAGE_RESULTS LockpickingMenuEx::ProcessMessage_Hook(RE::UIMessage& a_message)
	{
		switch (a_message.type.get())
		{
		case RE::UI_MESSAGE_TYPE::kShow:
			{
				RE::RefHandle handle;
				RE::CreateRefHandle(handle, GetTargetReference().get());
				AutoCloseManager::GetSingleton()->InitAutoClose(RE::LockpickingMenu::MENU_NAME, handle, false);

				SendSetSkyrimSoulsHUDModeMessage(true);
			}
			break;

		case RE::UI_MESSAGE_TYPE::kUpdate:
			{
				RE::UIMessageQueue::GetSingleton()->AddMessage(RE::HUDMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kUpdate, nullptr);
				AutoCloseManager::GetSingleton()->CheckAutoClose(RE::LockpickingMenu::MENU_NAME);
			}
			break;

		case RE::UI_MESSAGE_TYPE::kHide:
			SendSetSkyrimSoulsHUDModeMessage(false);
			break;
		}

		return _ProcessMessage(this, a_message);
	}

	RE::IMenu* LockpickingMenuEx::Creator()
	{
		RE::LockpickingMenu* menu = static_cast<RE::LockpickingMenu*>(CreateMenu(RE::LockpickingMenu::MENU_NAME));
		return menu;
	}

	void LockpickingMenuEx::InstallHook()
	{
		//Hook AdvanceMovie
		REL::Relocation<std::uintptr_t> vTable(RE::VTABLE_LockpickingMenu[0]);
		_ProcessMessage = vTable.write_vfunc(0x4, &LockpickingMenuEx::ProcessMessage_Hook);
	}
}
