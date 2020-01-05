#include "Settings.h"

#include "SimpleIni.h"

namespace SkyrimSoulsRE
{

	Settings* Settings::singleton = nullptr;

	Settings::Settings()
	{

	}

	Settings* Settings::GetSingleton()
	{
		if (singleton)
		{
			return singleton;
		}
		singleton = new Settings();
		return singleton;
	}

	void LoadSettings()
	{
		Settings* settings = Settings::GetSingleton();

		CSimpleIniA ini;
		ini.SetUnicode();
		ini.LoadFile(".\\Data\\SKSE\\Plugins\\SkyrimSoulsRE.ini");

		ini.SetValue("UNPAUSED_MENUS", nullptr, nullptr, "#  Here you can set which menus you want to unpause.\n#  true - unpaused, false - paused");

		//Unpaused menus
		settings->unpausedMenus["TweenMenu"] = ini.GetBoolValue("UNPAUSED_MENUS", "bTweenMenu", true);
		ini.SetBoolValue("UNPAUSED_MENUS", "bTweenMenu", settings->unpausedMenus["TweenMenu"], nullptr, true);

		settings->unpausedMenus["InventoryMenu"] = ini.GetBoolValue("UNPAUSED_MENUS", "bInventoryMenu", true);
		ini.SetBoolValue("UNPAUSED_MENUS", "bInventoryMenu", settings->unpausedMenus["InventoryMenu"], nullptr, true);

		settings->unpausedMenus["MagicMenu"] = ini.GetBoolValue("UNPAUSED_MENUS", "bMagicMenu", true);
		ini.SetBoolValue("UNPAUSED_MENUS", "bMagicMenu", settings->unpausedMenus["MagicMenu"], nullptr, true);

		settings->unpausedMenus["BarterMenu"] = ini.GetBoolValue("UNPAUSED_MENUS", "bBarterMenu", true);
		ini.SetBoolValue("UNPAUSED_MENUS", "bBarterMenu", settings->unpausedMenus["BarterMenu"], nullptr, true);

		settings->unpausedMenus["ContainerMenu"] = ini.GetBoolValue("UNPAUSED_MENUS", "bContainerMenu", true);
		ini.SetBoolValue("UNPAUSED_MENUS", "bContainerMenu", settings->unpausedMenus["ContainerMenu"], nullptr, true);

		settings->unpausedMenus["GiftMenu"] = ini.GetBoolValue("UNPAUSED_MENUS", "bGiftMenu", true);
		ini.SetBoolValue("UNPAUSED_MENUS", "bGiftMenu", settings->unpausedMenus["GiftMenu"], nullptr, true);

		settings->unpausedMenus["FavoritesMenu"] = ini.GetBoolValue("UNPAUSED_MENUS", "bFavoritesMenu", true);
		ini.SetBoolValue("UNPAUSED_MENUS", "bFavoritesMenu", settings->unpausedMenus["FavoritesMenu"], nullptr, true);

		settings->unpausedMenus["Tutorial Menu"] = ini.GetBoolValue("UNPAUSED_MENUS", "bTutorialMenu", true);
		ini.SetBoolValue("UNPAUSED_MENUS", "bTutorialMenu", settings->unpausedMenus["Tutorial Menu"], nullptr, true);

		settings->unpausedMenus["Lockpicking Menu"] = ini.GetBoolValue("UNPAUSED_MENUS", "bLockpickingMenu", true);
		ini.SetBoolValue("UNPAUSED_MENUS", "bLockpickingMenu", settings->unpausedMenus["Lockpicking Menu"], nullptr, true);

		settings->unpausedMenus["Book Menu"] = ini.GetBoolValue("UNPAUSED_MENUS", "bBookMenu", true);
		ini.SetBoolValue("UNPAUSED_MENUS", "bBookMenu", settings->unpausedMenus["Book Menu"], nullptr, true);

		settings->unpausedMenus["MessageBoxMenu"] = ini.GetBoolValue("UNPAUSED_MENUS", "bMessageBoxMenu", true);
		ini.SetBoolValue("UNPAUSED_MENUS", "bMessageBoxMenu", settings->unpausedMenus["MessageBoxMenu"], nullptr, true);

		settings->unpausedMenus["TrainingMenu"] = ini.GetBoolValue("UNPAUSED_MENUS", "bTrainingMenu", true);
		ini.SetBoolValue("UNPAUSED_MENUS", "bTrainingMenu", settings->unpausedMenus["TrainingMenu"], nullptr, true);

		settings->unpausedMenus["Journal Menu"] = ini.GetBoolValue("UNPAUSED_MENUS", "bJournalMenu", true);
		ini.SetBoolValue("UNPAUSED_MENUS", "bJournalMenu", settings->unpausedMenus["Journal Menu"], nullptr, true);

		settings->unpausedMenus["Sleep/Wait Menu"] = ini.GetBoolValue("UNPAUSED_MENUS", "bSleepWaitMenu", true);
		ini.SetBoolValue("UNPAUSED_MENUS", "bSleepWaitMenu", settings->unpausedMenus["Sleep/Wait Menu"], nullptr, true);

		settings->unpausedMenus["Console"] = ini.GetBoolValue("UNPAUSED_MENUS", "bConsole", true);
		ini.SetBoolValue("UNPAUSED_MENUS", "bConsole", settings->unpausedMenus["Console"], nullptr, true);

		settings->unpausedMenus["Mod Manager Menu"] = ini.GetBoolValue("UNPAUSED_MENUS", "bBethesdaModMenu", true);
		ini.SetBoolValue("UNPAUSED_MENUS", "bBethesdaModMenu", settings->unpausedMenus["Mod Manager Menu"], nullptr, true);

		settings->unpausedMenus["CustomMenu"] = ini.GetBoolValue("UNPAUSED_MENUS", "bCustomMenu", true);
		ini.SetBoolValue("UNPAUSED_MENUS", "bCustomMenu", settings->unpausedMenus["CustomMenu"], nullptr, true);

		//Auto-close
		settings->autoCloseMenus = ini.GetBoolValue("AUTOCLOSE", "bAutoCloseMenus", true);
		ini.SetBoolValue("AUTOCLOSE", "bAutoCloseMenus", settings->autoCloseMenus, "#  If this setting is enabled, the container menu, lockpicking menu and book menu will automatically close if the target gets too far.\n#  This will make pickpocketing harder if the npc is moving.\n#  Default value is enabled", true);

		settings->autoCloseDistance = ini.GetDoubleValue("AUTOCLOSE", "fAutoCloseDistance", 350.0);
		ini.SetDoubleValue("AUTOCLOSE", "fAutoCloseDistance", settings->autoCloseDistance, "#  The maximum distance (in in-game units) before the container menu and lockpicking menu will automatically close.\n#  Too small values can cause the menus to close immediately.\n#  Default value is 350", true);

		//Controls
		settings->autoCenterCursor = ini.GetBoolValue("CONTROLS", "bAutoCenterCursor", false);
		ini.SetBoolValue("CONTROLS", "bAutoCenterCursor", settings->autoCenterCursor, "#  If enabled, the cursor will be automatically re-centered every time a menu opens.\n#  Default is disabled.", true);

		settings->enableMovementInMenus = ini.GetBoolValue("CONTROLS", "bEnableMovementInMenus", false);
		ini.SetBoolValue("CONTROLS", "bEnableMovementInMenus", settings->enableMovementInMenus, "#  If enabled, you will be able to move when a menu is open. Use the mouse (or the D-pad on controllers) to navigate the menus.\n#  (For controllers users) To change tabs in SkyUI favorites menu, use LB and RB buttons.\n#  Default is disabled.", true);

		settings->enableMovementDialogueMenu = ini.GetBoolValue("CONTROLS", "bEnableMovementDialogueMenu", false);
		ini.SetBoolValue("CONTROLS", "bEnableMovementDialogueMenu", settings->enableMovementDialogueMenu, "#  If enabled, you will be able to move in the dialogue menu as well. \n#  bEnableMovementInMenus must be enabled for this to work.\n#  Default is disabled.", true);

		settings->enableGamepadCameraMove = ini.GetBoolValue("CONTROLS", "bEnableGamepadCameraMove", false);
		ini.SetBoolValue("CONTROLS", "bEnableGamepadCameraMove", settings->enableGamepadCameraMove, "#  If enabled, you will be able to move the camera when using controllers. To rotate items in the inventory, maximize the preview first by pressing on the thumbstick.\n#  Default is disabled.", true);

		settings->enableCursorCameraMove = ini.GetBoolValue("CONTROLS", "bEnableCursorCameraMove", false);
		ini.SetBoolValue("CONTROLS", "bEnableCursorCameraMove", settings->enableCursorCameraMove, "#  If enabled, you will be able to move the camera with the mouse by moving it to the edge of the screen (similiar to how it works in the dialogue menu)\n#  Recommended to be used with bAutoCenterCursor enabled. Default is disabled.", true);

		settings->cursorCameraVerticalSpeed = ini.GetDoubleValue("CONTROLS", "fCursorCameraVerticalSpeed", 0.15);
		ini.SetDoubleValue("CONTROLS", "fCursorCameraVerticalSpeed", settings->cursorCameraVerticalSpeed, "#  The vertical speed the camera moves when bEnableCursorCameraMove is enabled. Default is 0.15", true);

		settings->cursorCameraHorizontalSpeed = ini.GetDoubleValue("CONTROLS", "fCursorCameraHorizontalSpeed", 0.20);
		ini.SetDoubleValue("CONTROLS", "fCursorCameraHorizontalSpeed", settings->cursorCameraHorizontalSpeed, "#  The horizontal speed the camera moves when bEnableCursorCameraMove is enabled. Default is 0.20", true);

		//Slowmotion
		settings->enableSlowMotion = ini.GetBoolValue("SLOWMOTION", "bEnableSlowMotion", false);
		ini.SetBoolValue("SLOWMOTION", "bEnableSlowMotion", settings->enableSlowMotion, "#  If enabled, the time will slow down when an unpaused menu is open.\n#  Currently it has an issue with the book menu, causing it to have the page turning animation in slow motion too.", true);

		settings->slowMotionMultiplier = ini.GetDoubleValue("SLOWMOTION", "fSlowMotionMultiplier", 0.5);
		ini.SetDoubleValue("SLOWMOTION", "fSlowMotionMultiplier", settings->slowMotionMultiplier, "#  This is the multiplier that will affect the game speed when a menu is open. Has no effect if bEnableSlowMotion is disabled\n#  1.0 is no slowdown, 0.5 is half the speed, etc.\n#  Minimum value : 0.1, Maximum value : 1.0, Default value : 0.5", true);

		ini.SaveFile(".\\Data\\SKSE\\Plugins\\SkyrimSoulsRE.ini");
	}
}
