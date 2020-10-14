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

		ini.SetValue("UNPAUSED_MENUS", nullptr, nullptr, "# DELETE THIS FILE AND RUN THE GAME IF YOU WANT TO REGENERATE THE DEFAULT SETTINGS\n\n#  Here you can set which menus you want to be paused or unpaused.");

		//Unpaused menus
		settings->unpausedMenus[RE::BarterMenu::MENU_NAME.data()] = ini.GetBoolValue("UNPAUSED_MENUS", "bBarterMenu", true);
		settings->unpausedMenus[RE::BookMenu::MENU_NAME.data()] = ini.GetBoolValue("UNPAUSED_MENUS", "bBookMenu", true);
		settings->unpausedMenus[RE::Console::MENU_NAME.data()] = ini.GetBoolValue("UNPAUSED_MENUS", "bConsole", true);
		settings->unpausedMenus[RE::ContainerMenu::MENU_NAME.data()] = ini.GetBoolValue("UNPAUSED_MENUS", "bContainerMenu", true);
		settings->unpausedMenus[RE::FavoritesMenu::MENU_NAME.data()] = ini.GetBoolValue("UNPAUSED_MENUS", "bFavoritesMenu", true);
		settings->unpausedMenus[RE::GiftMenu::MENU_NAME.data()] = ini.GetBoolValue("UNPAUSED_MENUS", "bGiftMenu", true);
		settings->unpausedMenus[RE::InventoryMenu::MENU_NAME.data()] = ini.GetBoolValue("UNPAUSED_MENUS", "bInventoryMenu", true);
		settings->unpausedMenus[RE::JournalMenu::MENU_NAME.data()] = ini.GetBoolValue("UNPAUSED_MENUS", "bJournalMenu", true);
		settings->unpausedMenus[RE::LevelUpMenu::MENU_NAME.data()] = ini.GetBoolValue("UNPAUSED_MENUS", "bLevelUpMenu", true);
		settings->unpausedMenus[RE::LockpickingMenu::MENU_NAME.data()] = ini.GetBoolValue("UNPAUSED_MENUS", "bLockpickingMenu", true);
		settings->unpausedMenus[RE::MagicMenu::MENU_NAME.data()] = ini.GetBoolValue("UNPAUSED_MENUS", "bMagicMenu", true);
		settings->unpausedMenus[RE::MapMenu::MENU_NAME.data()] = ini.GetBoolValue("UNPAUSED_MENUS", "bMapMenu", true);
		settings->unpausedMenus[RE::MessageBoxMenu::MENU_NAME.data()] = ini.GetBoolValue("UNPAUSED_MENUS", "bMessageBoxMenu", true);
		settings->unpausedMenus[RE::ModManagerMenu::MENU_NAME.data()] = ini.GetBoolValue("UNPAUSED_MENUS", "bModManagerMenu", true);
		settings->unpausedMenus[RE::SleepWaitMenu::MENU_NAME.data()] = ini.GetBoolValue("UNPAUSED_MENUS", "bSleepWaitMenu", true);
		settings->unpausedMenus[RE::StatsMenu::MENU_NAME.data()] = ini.GetBoolValue("UNPAUSED_MENUS", "bStatsMenu", true);
		settings->unpausedMenus[RE::TrainingMenu::MENU_NAME.data()] = ini.GetBoolValue("UNPAUSED_MENUS", "bTrainingMenu", true);
		settings->unpausedMenus[RE::TutorialMenu::MENU_NAME.data()] = ini.GetBoolValue("UNPAUSED_MENUS", "bTutorialMenu", true);
		settings->unpausedMenus[RE::TweenMenu::MENU_NAME.data()] = ini.GetBoolValue("UNPAUSED_MENUS", "bTweenMenu", true);
		settings->unpausedMenus["CustomMenu"] = ini.GetBoolValue("UNPAUSED_MENUS", "bCustomMenu", true);

		ini.SetBoolValue("UNPAUSED_MENUS", "bBarterMenu", settings->unpausedMenus[RE::BarterMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("UNPAUSED_MENUS", "bBookMenu", settings->unpausedMenus[RE::BookMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("UNPAUSED_MENUS", "bConsole", settings->unpausedMenus[RE::Console::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("UNPAUSED_MENUS", "bContainerMenu", settings->unpausedMenus[RE::ContainerMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("UNPAUSED_MENUS", "bFavoritesMenu", settings->unpausedMenus[RE::FavoritesMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("UNPAUSED_MENUS", "bGiftMenu", settings->unpausedMenus[RE::GiftMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("UNPAUSED_MENUS", "bInventoryMenu", settings->unpausedMenus[RE::InventoryMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("UNPAUSED_MENUS", "bJournalMenu", settings->unpausedMenus[RE::JournalMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("UNPAUSED_MENUS", "bLevelUpMenu", settings->unpausedMenus[RE::LevelUpMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("UNPAUSED_MENUS", "bLockpickingMenu", settings->unpausedMenus[RE::LockpickingMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("UNPAUSED_MENUS", "bMagicMenu", settings->unpausedMenus[RE::MagicMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("UNPAUSED_MENUS", "bMapMenu", settings->unpausedMenus[RE::MapMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("UNPAUSED_MENUS", "bMessageBoxMenu", settings->unpausedMenus[RE::MessageBoxMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("UNPAUSED_MENUS", "bModManagerMenu", settings->unpausedMenus[RE::ModManagerMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("UNPAUSED_MENUS", "bSleepWaitMenu", settings->unpausedMenus[RE::SleepWaitMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("UNPAUSED_MENUS", "bStatsMenu", settings->unpausedMenus[RE::StatsMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("UNPAUSED_MENUS", "bTrainingMenu", settings->unpausedMenus[RE::TrainingMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("UNPAUSED_MENUS", "bTutorialMenu", settings->unpausedMenus[RE::TutorialMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("UNPAUSED_MENUS", "bTweenMenu", settings->unpausedMenus[RE::TweenMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("UNPAUSED_MENUS", "bCustomMenu", settings->unpausedMenus["CustomMenu"], nullptr, true);

		//Auto-close
		settings->autoCloseMenus = ini.GetBoolValue("AUTOCLOSE", "bAutoCloseMenus", true);
		settings->autoCloseDialogueMenus = ini.GetBoolValue("AUTOCLOSE", "bAutoCloseDialogueMenus", false);
		settings->autoCloseSleepWaitMenu = ini.GetBoolValue("AUTOCLOSE", "bAutoCloseSleepWaitMenu", false);
		settings->autoCloseDistance = ini.GetDoubleValue("AUTOCLOSE", "fAutoCloseDistance", 350.0);
		settings->autoCloseDistanceDialogueContext = ini.GetDoubleValue("AUTOCLOSE", "fAutoCloseDistanceDialogueContext", 1000.0);

		ini.SetBoolValue("AUTOCLOSE", "bAutoCloseMenus", settings->autoCloseMenus, "#  Auto close menus if the distance to the object is larger than this value. Affects ContainerMenu, LockpickingMenu and BookMenu.\n#  This will make pickpocketing harder if the npc is moving.", true);
		ini.SetBoolValue("AUTOCLOSE", "bAutoCloseDialogueMenus", settings->autoCloseDialogueMenus, "#  Affects DialogueMenu, BarterMenu, GiftMenu and TrainingMenu. Should be enabled if bEnableMovementDialogueMenu is set to true.", true);
		ini.SetBoolValue("AUTOCLOSE", "bAutoCloseSleepWaitMenu", settings->autoCloseSleepWaitMenu, "#  (Experimental) Closes the sleep/wait menu if you get too far from the bed.", true);
		ini.SetDoubleValue("AUTOCLOSE", "fAutoCloseDistance", settings->autoCloseDistance, "#  The maximum distance (in in-game units) before the menus will automatically close.\n#  Too small values can cause the menus to close immediately.", true);
		ini.SetDoubleValue("AUTOCLOSE", "fAutoCloseDistanceDialogueContext", settings->autoCloseDistanceDialogueContext, "#  The maximum distance for dialogue related menus (used by bAutoCloseDialogueMenus)", true);

		//Controls
		settings->enableMovementInMenus = ini.GetBoolValue("CONTROLS", "bEnableMovementInMenus", false);
		settings->enableMovementDialogueMenu = ini.GetBoolValue("CONTROLS", "bEnableMovementDialogueMenu", false);
		settings->enableGamepadCameraMove = ini.GetBoolValue("CONTROLS", "bEnableGamepadCameraMove", true);
		settings->enableCursorCameraMove = ini.GetBoolValue("CONTROLS", "bEnableCursorCameraMove", true);
		settings->cursorCameraVerticalSpeed = ini.GetDoubleValue("CONTROLS", "fCursorCameraVerticalSpeed", 0.15);
		settings->cursorCameraHorizontalSpeed = ini.GetDoubleValue("CONTROLS", "fCursorCameraHorizontalSpeed", 0.20);

		ini.SetBoolValue("CONTROLS", "bEnableMovementInMenus", settings->enableMovementInMenus, "#  If enabled, you will be able to move when a menu is open. Use the mouse (or the D-pad on controllers) to navigate the menus.\n#  (For controllers users) To change tabs in SkyUI favorites menu, use LB and RB buttons.", true);
		ini.SetBoolValue("CONTROLS", "bEnableMovementDialogueMenu", settings->enableMovementDialogueMenu, "#  If enabled, you will be able to move in the dialogue menu as well. \n#  bEnableMovementInMenus must be enabled for this to work.", true);
		ini.SetBoolValue("CONTROLS", "bEnableGamepadCameraMove", settings->enableGamepadCameraMove, "#  If enabled, you will be able to move the camera when using controllers. To rotate items in the inventory, maximize the preview first by pressing on the thumbstick.", true);
		ini.SetBoolValue("CONTROLS", "bEnableCursorCameraMove", settings->enableCursorCameraMove, "#  If enabled, you will be able to move the camera with the mouse by moving it to the edge of the screen (similiar to how it works in the dialogue menu).", true);
		ini.SetDoubleValue("CONTROLS", "fCursorCameraVerticalSpeed", settings->cursorCameraVerticalSpeed, "#  The vertical and horizontal speed the camera moves when bEnableCursorCameraMove is enabled.", true);
		ini.SetDoubleValue("CONTROLS", "fCursorCameraHorizontalSpeed", settings->cursorCameraHorizontalSpeed, nullptr, true);

		//Slowmotion
		settings->enableSlowMotion = ini.GetBoolValue("SLOWMOTION", "bEnableSlowMotion", false);
		settings->slowMotionMultiplier = ini.GetDoubleValue("SLOWMOTION", "fSlowMotionMultiplier", 0.5);

		ini.SetBoolValue("SLOWMOTION", "bEnableSlowMotion", settings->enableSlowMotion, "#  If enabled, the time will slow down when an unpaused menu is open.\n#  Currently it has an issue with the book menu, causing it to have the page turning animation in slow motion too.", true);
		ini.SetDoubleValue("SLOWMOTION", "fSlowMotionMultiplier", settings->slowMotionMultiplier, "#  This is the multiplier that will affect the game speed when a menu is open. Has no effect if bEnableSlowMotion is disabled\n#  1.0 is no slowdown, 0.5 is half the speed, etc.", true);

		ini.SetValue("COMBAT_ALERT_OVERLAY", nullptr, nullptr, "#  Shows a blinking red overlay when your character is in combat. Especially useful in full screen menus. You can enable or disable it individually for each menu.");

		//CombatAlertOverlay
		settings->overlayMenus[RE::BarterMenu::MENU_NAME.data()] = ini.GetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_BarterMenu", true);
		settings->overlayMenus[RE::BookMenu::MENU_NAME.data()] = ini.GetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_BookMenu", true);
		settings->overlayMenus[RE::ContainerMenu::MENU_NAME.data()] = ini.GetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_ContainerMenu", true);
		settings->overlayMenus[RE::DialogueMenu::MENU_NAME.data()] = ini.GetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_DialogueMenu", true);
		settings->overlayMenus[RE::FavoritesMenu::MENU_NAME.data()] = ini.GetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_FavoritesMenu", true);
		settings->overlayMenus[RE::GiftMenu::MENU_NAME.data()] = ini.GetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_GiftMenu", true);
		settings->overlayMenus[RE::InventoryMenu::MENU_NAME.data()] = ini.GetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_InventoryMenu", true);
		settings->overlayMenus[RE::JournalMenu::MENU_NAME.data()] = ini.GetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_JournalMenu", true);
		settings->overlayMenus[RE::LevelUpMenu::MENU_NAME.data()] = ini.GetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_LevelUpMenu", true);
		settings->overlayMenus[RE::LockpickingMenu::MENU_NAME.data()] = ini.GetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_LockpickingMenu", true);
		settings->overlayMenus[RE::MagicMenu::MENU_NAME.data()] = ini.GetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_MagicMenu", true);
		settings->overlayMenus[RE::MapMenu::MENU_NAME.data()] = ini.GetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_MapMenu", true);
		settings->overlayMenus[RE::MessageBoxMenu::MENU_NAME.data()] = ini.GetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_MessageBoxMenu", true);
		settings->overlayMenus[RE::ModManagerMenu::MENU_NAME.data()] = ini.GetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_ModManagerMenu", true);
		settings->overlayMenus[RE::SleepWaitMenu::MENU_NAME.data()] = ini.GetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_SleepWaitMenu", true);
		settings->overlayMenus[RE::StatsMenu::MENU_NAME.data()] = ini.GetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_StatsMenu", true);
		settings->overlayMenus[RE::TrainingMenu::MENU_NAME.data()] = ini.GetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_TrainingMenu", true);
		settings->overlayMenus[RE::TutorialMenu::MENU_NAME.data()] = ini.GetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_TutorialMenu", true);
		settings->overlayMenus[RE::TweenMenu::MENU_NAME.data()] = ini.GetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_TweenMenu", true);
		settings->overlayMenus["CustomMenu"] = ini.GetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_CustomMenu", true);

		ini.SetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_BarterMenu", settings->overlayMenus[RE::BarterMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_BookMenu", settings->overlayMenus[RE::BookMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_DialogueMenu", settings->overlayMenus[RE::DialogueMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_ContainerMenu", settings->overlayMenus[RE::ContainerMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_FavoritesMenu", settings->overlayMenus[RE::FavoritesMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_GiftMenu", settings->overlayMenus[RE::GiftMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_InventoryMenu", settings->overlayMenus[RE::InventoryMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_JournalMenu", settings->overlayMenus[RE::JournalMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_LevelUpMenu", settings->overlayMenus[RE::LevelUpMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_LockpickingMenu", settings->overlayMenus[RE::LockpickingMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_MagicMenu", settings->overlayMenus[RE::MagicMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_MapMenu", settings->overlayMenus[RE::MapMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_MessageBoxMenu", settings->overlayMenus[RE::MessageBoxMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_ModManagerMenu", settings->overlayMenus[RE::ModManagerMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_SleepWaitMenu", settings->overlayMenus[RE::SleepWaitMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_StatsMenu", settings->overlayMenus[RE::StatsMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_TrainingMenu", settings->overlayMenus[RE::TrainingMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_TutorialMenu", settings->overlayMenus[RE::TutorialMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_TweenMenu", settings->overlayMenus[RE::TweenMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_CustomMenu", settings->overlayMenus["CustomMenu"], nullptr, true);

		//HUD
		settings->sneakMeterPosX = ini.GetDoubleValue("HUD", "fSneakMeterPosX", 24.0);
		settings->sneakMeterPosY = ini.GetDoubleValue("HUD", "fSneakMeterPosY", 120.0);

		ini.SetDoubleValue("HUD", "fSneakMeterPosX", settings->sneakMeterPosX, "#  The position where the sneak meter will appear on the screen when a menu is open. It's necesary as some menu elements would hide it otherwise (eg. lockpicking).\n# The values are a bit arbitary so just try different values until you find what suits you.", true);
		ini.SetDoubleValue("HUD", "fSneakMeterPosY", settings->sneakMeterPosY, nullptr, true);


		//Blur
		settings->disableBlur = ini.GetBoolValue("BLUR", "bDisableBlur", false);

		ini.SetBoolValue("BLUR", "bDisableBlur", settings->disableBlur, "#  If this is enabled, background blur will be diabled in menus", true);

		//Saving
		settings->saveDelayMS = ini.GetLongValue("SAVING", "iSaveDelayMS", 1000);

		ini.SetLongValue("SAVING", "iSaveDelayMS", settings->saveDelayMS, "#  Additional delay (in milliseconds) when saving from the Journal Menu, to make sure that saving happens form a paused state.\n# Larger values will increase the time saving takes.", false, true);

		ini.SaveFile(".\\Data\\SKSE\\Plugins\\SkyrimSoulsRE.ini");
	}
}
