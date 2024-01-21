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

		//Blur
		bool disableBlur;

		//Saving
		std::uint32_t saveDelayMS;

		//HUD
		bool disableHUDModifications;
		float sneakMeterPosX;
		float sneakMeterPosY;

		//Combat Alert Overlay
		std::map<std::string, bool> overlayMenus;

		//Messages
		bool hideEngineFixesWarning;

		// Experimental
		bool enableMenuCaching;

		// Dialogue Movement Enabler compatibility (internal setting)
		bool isUsingDME = false;

	private:
		Settings(){};
		~Settings(){};
		Settings(const Settings&) = delete;
		Settings& operator=(const Settings&) = delete;
	};

	extern void LoadSettings();
}
