#pragma once

#include <string>
#include <vector>
#include <map>

namespace SkyrimSoulsRE
{
	class Settings
	{
	private:
		static Settings * singleton;
		Settings();
	public:

		static Settings* GetSingleton();

		//Unpaused Menus
		std::map<std::string, bool> unpausedMenus;

		//AutoClose
		bool	autoCloseMenus;
		float	autoCloseDistance;

		//Controls
		bool	autoCenterCursor;
		bool	enableMovementInMenus;
		bool	enableMovementDialogueMenu;
		bool	enableGamepadCameraMove;
		bool	enableCursorCameraMove;
		float	cursorCameraVerticalSpeed;
		float	cursorCameraHorizontalSpeed;

		//slow motion
		bool	enableSlowMotion;
		float	slowMotionMultiplier;
	};

	extern void LoadSettings();
}
