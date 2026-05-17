#pragma once

#include <map>
#include <string>
#include <vector>

namespace SkyrimSoulsRE
{
	class Settings
	{
	public:
		static Settings* GetSingleton();

		//Unpaused Menus
		std::map<std::string, bool> unpausedMenus;

		//AutoClose
		bool autoCloseMenus;
		float autoCloseDistance;
		float autoCloseTolerance;

		//Data Updates
		std::uint32_t bottomBarMeterUpdateSteps;
		bool updateInventoryMenuBottomBar;
		bool updateContainerMenuBottomBar;
		bool updateContainerMenuPickpocketChance;
		bool updateMagicMenuBottomBar;
		bool updateMagicMenuActiveEffectTimers;
		bool updateStatsMenuPlayerInfo;
		bool updateStatsMenuSkillList;
		bool updateTweenMenuBottomBar;
		bool updateJournalMenuBottomBar;
		bool updateMapMenuBottomBar;
		bool updateSleepWaitMenuClock;

		//Controls
		bool enableMovementInMenus;
		bool enableToggleRun;
		bool enableGamepadCameraMove;
		bool enableCursorCameraMove;
		float cursorCameraVerticalSpeed;
		float cursorCameraHorizontalSpeed;

		//slow motion
		std::map<std::string, bool> slowMotionMenus;
		float slowMotionMultiplier;
		bool slowMotionCombatOnly;

		//Blur
		bool disableBlur;

		//HUD
		bool disableHUDModifications;
		float sneakMeterPosX;
		float sneakMeterPosY;

		//Combat Alert Overlay
		std::map<std::string, bool> overlayMenus;

		//Messages
		bool hideEngineFixesWarning;

		// Map Menu
		bool mapMenuAmbientSoundLoop;
		bool mapMenuCustomSky;

		// Dialogue Movement Enabler compatibility (internal setting)
		bool isUsingDME = false;

	private:
		Settings() {};
		~Settings() {};
		Settings(const Settings&) = delete;
		Settings& operator=(const Settings&) = delete;
	};

	extern void LoadSettings();
}
