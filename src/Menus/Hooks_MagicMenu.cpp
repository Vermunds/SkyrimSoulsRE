#include "Menus/Hooks_MagicMenu.h"

namespace SkyrimSoulsRE
{
	//testing
	auto lastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch());

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

		return CreateMenu(RE::MagicMenu::MENU_NAME);
	}

	void MagicMenuEx::InstallHook()
	{
		//Hook AdvanceMovie
		REL::Relocation<std::uintptr_t> vTable(Offsets::Menus::MagicMenu::Vtbl);
		_AdvanceMovie = vTable.write_vfunc(0x5, &MagicMenuEx::AdvanceMovie_Hook);
	}
}

