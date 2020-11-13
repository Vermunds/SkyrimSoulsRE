#include "Menus/Hooks_MagicMenu.h"

namespace SkyrimSoulsRE
{
	//testing
	auto lastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch());

	RE::UI_MESSAGE_RESULTS MagicMenuEx::ProcessMessage_Hook(RE::UIMessage& a_message)
	{
		if (a_message.type == RE::UI_MESSAGE_TYPE::kHide)
		{
			HUDMenuEx* hudMenu = static_cast<HUDMenuEx*>(RE::UI::GetSingleton()->GetMenu(RE::InterfaceStrings::GetSingleton()->hudMenu).get());
			if (hudMenu)
			{
				hudMenu->SetSkyrimSoulsMode(false);
			}
		}
		return _ProcessMessage(this, a_message);
	}

	void MagicMenuEx::AdvanceMovie_Hook(float a_interval, std::uint32_t a_currentTime)
	{
		this->UpdateBottomBar();

		auto currTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch());

		if (currTime - lastUpdate > std::chrono::milliseconds(1000))
		{
			reinterpret_cast<RE::ItemList*>(this->unk30)->Update();
			lastUpdate = currTime;
		}
		
		return _AdvanceMovie(this, a_interval, a_currentTime);
	}

	void MagicMenuEx::UpdateBottomBar()
	{
		using func_t = decltype(&MagicMenuEx::UpdateBottomBar);
		REL::Relocation<func_t> func(Offsets::Menus::MagicMenu::UpdateBottomBar);
		return func(this);
	}

	RE::IMenu* MagicMenuEx::Creator()
	{
		lastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch());

		HUDMenuEx* hudMenu = static_cast<HUDMenuEx*>(RE::UI::GetSingleton()->GetMenu(RE::InterfaceStrings::GetSingleton()->hudMenu).get());
		if (hudMenu)
		{
			hudMenu->SetSkyrimSoulsMode(true);
		}

		return CreateMenu(RE::MagicMenu::MENU_NAME);
	}

	void MagicMenuEx::InstallHook()
	{
		//Hook AdvanceMovie
		REL::Relocation<std::uintptr_t> vTable(Offsets::Menus::MagicMenu::Vtbl);
		_ProcessMessage = vTable.write_vfunc(0x4, &MagicMenuEx::ProcessMessage_Hook);
		_AdvanceMovie = vTable.write_vfunc(0x5, &MagicMenuEx::AdvanceMovie_Hook);
	}
}
