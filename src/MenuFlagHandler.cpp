#include "MenuFlagHandler.h"
#include "Settings.h"
#include "SkyrimSoulsRE.h"

#include "Menus/CombatAlertOverlayMenu.h"

namespace SkyrimSoulsRE
{
	using MenuFlag = RE::IMenu::Flag;

	void MenuFlagHandler::UpdateMenu(RE::IMenu* a_menu, std::string_view a_menuName, bool a_isOpen)
	{
		RE::UI* ui = RE::UI::GetSingleton();
		Settings* settings = Settings::GetSingleton();

		std::string menuName(a_menuName);
		bool isConsole = menuName == RE::Console::MENU_NAME;

		if (!isConsole)
		{
			auto overlayIt = settings->overlayMenus.find(menuName);
			bool usesOverlay = overlayIt != settings->overlayMenus.end() && overlayIt->second;
			if (usesOverlay)
			{
				a_menu->menuFlags.set(static_cast<MenuFlag>(MenuFlagEx::kUsesCombatAlertOverlay));
			}
			else
			{
				a_menu->menuFlags.reset(static_cast<MenuFlag>(MenuFlagEx::kUsesCombatAlertOverlay));
			}

			// The overlay hides itself once no menu uses it any more, it only has to be shown here.
			if (usesOverlay && !ui->IsMenuOpen(CombatAlertOverlayMenu::MENU_NAME))
			{
				RE::UIMessageQueue::GetSingleton()->AddMessage(CombatAlertOverlayMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kShow, nullptr);
			}
		}

		// Check if we should care about this menu at all
		if (!a_menu->menuFlags.all(MenuFlag::kPausesGame) && !a_menu->menuFlags.all(static_cast<MenuFlag>(MenuFlagEx::kUnpaused)))
		{
			return;
		}

		auto unpausedIt = settings->unpausedMenus.find(menuName);
		if (unpausedIt == settings->unpausedMenus.end())
		{
			return;
		}

		// An active Sleep/Wait menu must never run unpaused.
		if (menuName == RE::SleepWaitMenu::MENU_NAME && static_cast<RE::SleepWaitMenu*>(a_menu)->isActive)
		{
			return;
		}

		RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
		bool inCombat = player && player->IsInCombat();

		bool wantUnpaused = unpausedIt->second;
		bool shouldPause = !wantUnpaused || (settings->pauseDuringCombat && inCombat);

		bool isPaused = a_menu->menuFlags.all(MenuFlag::kPausesGame);
		if (shouldPause != isPaused)
		{
			if (shouldPause)
			{
				a_menu->menuFlags.set(MenuFlag::kPausesGame);
				if (a_isOpen)
				{
					ui->numPausesGame++;
				}
			}
			else
			{
				a_menu->menuFlags.reset(MenuFlag::kPausesGame);
				if (a_isOpen)
				{
					ui->numPausesGame--;
				}
			}
		}

		// Clear the freeze frame background flag if we want to be unpaused, or the game deadlocks
		if (wantUnpaused)
		{
			a_menu->menuFlags.reset(MenuFlag::kFreezeFrameBackground);
		}

		// The console is unpaused like any other menu, but it is never marked as unpaused and never slowed down.
		if (isConsole)
		{
			return;
		}

		if (wantUnpaused)
		{
			a_menu->menuFlags.set(static_cast<MenuFlag>(MenuFlagEx::kUnpaused));
		}
		else
		{
			a_menu->menuFlags.reset(static_cast<MenuFlag>(MenuFlagEx::kUnpaused));
		}

		auto slowMotionIt = settings->slowMotionMenus.find(menuName);
		if (wantUnpaused && slowMotionIt != settings->slowMotionMenus.end() && slowMotionIt->second)
		{
			a_menu->menuFlags.set(static_cast<MenuFlag>(MenuFlagEx::kUsesSlowMotion));
		}
		else
		{
			a_menu->menuFlags.reset(static_cast<MenuFlag>(MenuFlagEx::kUsesSlowMotion));
		}
	}

	void MenuFlagHandler::Update()
	{
		RE::UI* ui = RE::UI::GetSingleton();

		for (auto& entry : ui->menuMap)
		{
			RE::IMenu* menu = entry.second.menu.get();
			if (!menu)
			{
				continue;
			}

			UpdateMenu(menu, entry.first.c_str(), true);
		}
	}

	MenuFlagHandler* MenuFlagHandler::GetSingleton()
	{
		static MenuFlagHandler singleton;
		return &singleton;
	}
}
