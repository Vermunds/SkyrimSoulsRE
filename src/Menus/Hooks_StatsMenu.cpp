#include "Menus/Hooks_StatsMenu.h"

namespace SkyrimSoulsRE
{
    RE::IMenu* StatsMenuEx::Creator()
    {
        StatsMenuEx* menu = static_cast<StatsMenuEx*>(CreateMenu(RE::StatsMenu::MENU_NAME));
        menu->CanProcess(nullptr);
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
    }
}