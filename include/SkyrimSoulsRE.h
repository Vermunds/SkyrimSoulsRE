#pragma once

#include "AutoCloseManager.h"
#include "ItemMenuUpdater.h"
#include "MenuProcessing.h"
#include "Offsets.h"
#include "PapyrusHook.h"
#include "Settings.h"
#include "UIBlurManagerEx.h"

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
	bool IsInSurvivalMode();

	void InstallMenuHooks();
	void InstallHooks();
}
