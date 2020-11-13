#include "Menus/Hooks_StatsMenu.h"

namespace SkyrimSoulsRE
{
    void StatsMenuEx::AdvanceMovie_Hook(float a_interval, std::uint32_t a_currentTime)
    {
        HUDMenuEx* hudMenu = static_cast<HUDMenuEx*>(RE::UI::GetSingleton()->GetMenu(RE::HUDMenu::MENU_NAME).get());
        if (hudMenu)
        {
            hudMenu->UpdateHUD();
        }
        return _AdvanceMovie(this, a_interval, a_currentTime);
    }

    RE::IMenu* StatsMenuEx::Creator()
    {
        StatsMenuEx* menu = static_cast<StatsMenuEx*>(CreateMenu(RE::StatsMenu::MENU_NAME));
        return menu;
    }

    void StatsMenuEx::InstallHook()
    {
        // Fix for menu not appearing
		REL::safe_write(Offsets::Menus::StatsMenu::ProcessMessage.address() + 0x929, std::uint32_t(0x90909090));
		REL::safe_write(Offsets::Menus::StatsMenu::ProcessMessage.address() + 0x92D, std::uint16_t(0x9090));

        // Prevent setting kFreezeFrameBackground flag
		REL::safe_write(Offsets::Menus::StatsMenu::ProcessMessage.address() + 0xAEC, std::uint32_t(0x90909090));

        // Keep the menu updated
        REL::safe_write(Offsets::Menus::StatsMenu::ProcessMessage.address() + 0xFBC, std::uint16_t(0x9090));

        // Fix for controls not working
		REL::safe_write(Offsets::Menus::StatsMenu::CanProcess.address() + 0x46, std::uint32_t(0x90909090));
		REL::safe_write(Offsets::Menus::StatsMenu::CanProcess.address() + 0x4A, std::uint16_t(0x9090));

        //Hook AdvanceMovie
        REL::Relocation<std::uintptr_t> vTable(Offsets::Menus::StatsMenu::Vtbl);
        _AdvanceMovie = vTable.write_vfunc(0x5, &StatsMenuEx::AdvanceMovie_Hook);
    }
}
