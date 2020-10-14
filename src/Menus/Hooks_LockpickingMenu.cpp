#include "Menus/Hooks_LockpickingMenu.h"

namespace SkyrimSoulsRE
{

	void LockpickingMenuEx::AdvanceMovie_Hook(float a_interval, std::uint32_t a_currentTime)
	{

		// Update the HUD
		HUDMenuEx* hudMenu = static_cast<HUDMenuEx*>(RE::UI::GetSingleton()->GetMenu(RE::InterfaceStrings::GetSingleton()->hudMenu).get());
		if (hudMenu)
		{
			for (auto it = hudMenu->objects.begin(); it != hudMenu->objects.end(); ++it)
			{
				RE::HUDObject* obj = *it;
				obj->Update();
			}
		}
		return _AdvanceMovie(this, a_interval, a_currentTime);
	}

	RE::UI_MESSAGE_RESULTS LockpickingMenuEx::ProcessMessage_Hook(RE::UIMessage& a_message)
	{
		if (a_message.type == RE::UI_MESSAGE_TYPE::kHide)
		{
			HUDMenuEx* hudMenu = static_cast<HUDMenuEx*>(RE::UI::GetSingleton()->GetMenu(RE::InterfaceStrings::GetSingleton()->hudMenu).get());
			if (hudMenu)
			{
				hudMenu->SetStealPickpocketHUDMode(false);
			}
		}
		return _ProcessMessage(this, a_message);
	}

	RE::IMenu* LockpickingMenuEx::Creator()
	{
		RE::LockpickingMenu* menu = static_cast<RE::LockpickingMenu*>(CreateMenu(RE::LockpickingMenu::MENU_NAME));

		RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();

		RE::TESObjectREFRPtr* refptr = reinterpret_cast<RE::TESObjectREFRPtr*>(Offsets::Menus::LockpickingMenu::TargetRef.address());
		assert(refptr);

		RE::TESObjectREFR* ref = refptr->get();
		if (!ref)
		{
			SKSE::log::error("Failed to find Lockpicking Menu target!");
		}

		AutoCloseManager* autoCloseManager = AutoCloseManager::GetSingleton();
		autoCloseManager->InitAutoClose(RE::ContainerMenu::MENU_NAME, ref, false);

		HUDMenuEx* hudMenu = static_cast<HUDMenuEx*>(RE::UI::GetSingleton()->GetMenu(RE::InterfaceStrings::GetSingleton()->hudMenu).get());
		if (hudMenu)
		{
			hudMenu->SetStealPickpocketHUDMode(true);
		}

		return menu;
	}

	void LockpickingMenuEx::InstallHook()
	{
		//Hook AdvanceMovie
		REL::Relocation<std::uintptr_t> vTable(Offsets::Menus::LockpickingMenu::Vtbl);
		_ProcessMessage = vTable.write_vfunc(0x4, &LockpickingMenuEx::ProcessMessage_Hook);
		_AdvanceMovie = vTable.write_vfunc(0x5, &LockpickingMenuEx::AdvanceMovie_Hook);

		REL::safe_write(Offsets::LockpickingMenu_Hook.address() + 0xE0, std::uint16_t(0x9090));
	}
}