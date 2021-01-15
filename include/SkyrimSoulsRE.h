#pragma once

#include "AudioHooks.h"
#include "ConsoleCommandHooks.h"
#include "ItemMenuUpdater.h"
#include "Offsets.h"
#include "PapyrusHook.h"
#include "Settings.h"
#include "SlowMotionHandler.h"
#include "UnpausedTaskQueue.h"

#include "AutoCloseManager.h"

#include "Menus/CombatAlertOverlayMenu.h"
#include "Menus/Hooks_BarterMenu.h"
#include "Menus/Hooks_BookMenu.h"
#include "Menus/Hooks_Console.h"
#include "Menus/Hooks_ContainerMenu.h"
#include "Menus/Hooks_CustomMenu.h"
#include "Menus/Hooks_DialogueMenu.h"
#include "Menus/Hooks_FavoritesMenu.h"
#include "Menus/Hooks_GiftMenu.h"
#include "Menus/Hooks_HUDMenu.h"
#include "Menus/Hooks_InventoryMenu.h"
#include "Menus/Hooks_JournalMenu.h"
#include "Menus/Hooks_LevelUpMenu.h"
#include "Menus/Hooks_LockpickingMenu.h"
#include "Menus/Hooks_MagicMenu.h"
#include "Menus/Hooks_MapMenu.h"
#include "Menus/Hooks_MessageBoxMenu.h"
#include "Menus/Hooks_ModManagerMenu.h"
#include "Menus/Hooks_SleepWaitMenu.h"
#include "Menus/Hooks_StatsMenu.h"
#include "Menus/Hooks_TrainingMenu.h"
#include "Menus/Hooks_TutorialMenu.h"
#include "Menus/Hooks_TweenMenu.h"

#include "Controls/CameraMovement.h"
#include "Controls/InputHandlerEx.h"
#include "Controls/MapInputHandlerEx.h"
#include "Controls/MenuControlsEx.h"
#include "Controls/PlayerControlsEx.h"

namespace SkyrimSoulsRE
{
	enum class MenuFlagEx : std::uint32_t
	{
		kUnpaused = 1 << 28,
		kUsesCombatAlertOverlay = 1 << 29,
		kUsesSlowMotion = 1 << 30
	};

	RE::IMenu* CreateMenu(std::string_view a_menuName);
	std::uint32_t GetCombatAlertOverlayCount();
	std::uint32_t GetUnpausedMenuCount();
	std::uint32_t GetSlowMotionCount();
	bool IsFullScreenMenuOpen();

	void InstallMenuHooks();
	void InstallHooks();
}
