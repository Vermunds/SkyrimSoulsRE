#include "Menus/BookMenuEx.h"

namespace SkyrimSoulsRE
{
	RE::UI_MESSAGE_RESULTS BookMenuEx::ProcessMessage_Hook(RE::UIMessage& a_message)
	{
		switch (a_message.type.get())
		{
		case RE::UI_MESSAGE_TYPE::kShow:
			{
				RE::RefHandle handle;
				RE::CreateRefHandle(handle, GetTargetReference().get());
				AutoCloseManager::GetSingleton()->InitAutoClose(RE::BookMenu::MENU_NAME, handle, false);
			}
			break;

		case RE::UI_MESSAGE_TYPE::kUpdate:
			{
				AutoCloseManager::GetSingleton()->CheckAutoClose(RE::BookMenu::MENU_NAME);
			}
		}

		return _ProcessMessage(this, a_message);
	}

	RE::IMenu* BookMenuEx::Creator()
	{
		RE::BookMenu* menu = static_cast<RE::BookMenu*>(CreateMenu(RE::BookMenu::MENU_NAME));
		return menu;
	}

	void BookMenuEx::InstallHook()
	{
		//Hook AdvanceMovie
		REL::Relocation<std::uintptr_t> vTable(RE::VTABLE_BookMenu[0]);
		_ProcessMessage = vTable.write_vfunc(0x4, &BookMenuEx::ProcessMessage_Hook);

		// Fix for book not appearing
		REL::safe_write(Offsets::Menus::BookMenu::ProcessMessage.address() + 0x76, std::uint16_t(0x9090));
	}
}
