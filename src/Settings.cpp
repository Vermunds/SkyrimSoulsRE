#include "Settings.h"
#include "Util.h"

#include "Menus/ModMenus/DragonbornsBestiary/BestiaryMenuEx.h"
#include "Menus/ModMenus/QuestJournalOverhaul/QuestMenuEx.h"

#include <SimpleIni.h>

namespace
{
	constexpr const char* INI_PATH = R"(.\Data\SKSE\Plugins\SkyrimSoulsRE.ini)";

	void IniSection(CSimpleIniA& a_ini, const char* a_section, const char* a_comment = nullptr)
	{
		a_ini.SetValue(a_section, nullptr, nullptr, a_comment);
		logger::info("[{}]", a_section);
	}

	bool IniGetBool(CSimpleIniA& a_ini, const char* a_section, const char* a_key, bool a_default, const char* a_comment = nullptr)
	{
		bool val = a_ini.GetBoolValue(a_section, a_key, a_default);
		a_ini.SetBoolValue(a_section, a_key, val, a_comment, true);
		logger::info("  {}: {}", a_key, val);
		return val;
	}

	float IniGetFloat(CSimpleIniA& a_ini, const char* a_section, const char* a_key, float a_default, const char* a_comment = nullptr)
	{
		float val = static_cast<float>(a_ini.GetDoubleValue(a_section, a_key, a_default));
		a_ini.SetDoubleValue(a_section, a_key, val, a_comment, true);
		logger::info("  {}: {}", a_key, val);
		return val;
	}

	std::uint32_t IniGetUInt(CSimpleIniA& a_ini, const char* a_section, const char* a_key, std::uint32_t a_default, const char* a_comment = nullptr)
	{
		std::uint32_t val = static_cast<std::uint32_t>(a_ini.GetLongValue(a_section, a_key, a_default));
		a_ini.SetLongValue(a_section, a_key, val, a_comment, false, true);
		logger::info("  {}: {}", a_key, val);
		return val;
	}
}

namespace SkyrimSoulsRE
{
	Settings* Settings::GetSingleton()
	{
		static Settings singleton;
		return &singleton;
	}

	void LoadSettings()
	{
		Settings* settings = Settings::GetSingleton();

		CSimpleIniA ini;
		ini.SetUnicode();
		ini.LoadFile(INI_PATH);

		logger::info("Loading settings from: {}", std::filesystem::absolute(INI_PATH).string());

		IniSection(ini, "UNPAUSED_MENUS", "# DELETE THIS FILE AND RUN THE GAME IF YOU WANT TO REGENERATE THE DEFAULT SETTINGS\n\n#  Here you can set which menus you want to be paused or unpaused.");
		settings->unpausedMenus[RE::BarterMenu::MENU_NAME.data()] = IniGetBool(ini, "UNPAUSED_MENUS", "bBarterMenu", UNPAUSED_BARTER_MENU_DEFAULT_VALUE);
		settings->unpausedMenus[RE::BookMenu::MENU_NAME.data()] = IniGetBool(ini, "UNPAUSED_MENUS", "bBookMenu", UNPAUSED_BOOK_MENU_DEFAULT_VALUE);
		settings->unpausedMenus[RE::Console::MENU_NAME.data()] = IniGetBool(ini, "UNPAUSED_MENUS", "bConsole", UNPAUSED_CONSOLE_DEFAULT_VALUE);
		settings->unpausedMenus[RE::ContainerMenu::MENU_NAME.data()] = IniGetBool(ini, "UNPAUSED_MENUS", "bContainerMenu", UNPAUSED_CONTAINER_MENU_DEFAULT_VALUE);
		settings->unpausedMenus[RE::FavoritesMenu::MENU_NAME.data()] = IniGetBool(ini, "UNPAUSED_MENUS", "bFavoritesMenu", UNPAUSED_FAVORITES_MENU_DEFAULT_VALUE);
		settings->unpausedMenus[RE::GiftMenu::MENU_NAME.data()] = IniGetBool(ini, "UNPAUSED_MENUS", "bGiftMenu", UNPAUSED_GIFT_MENU_DEFAULT_VALUE);
		settings->unpausedMenus[RE::InventoryMenu::MENU_NAME.data()] = IniGetBool(ini, "UNPAUSED_MENUS", "bInventoryMenu", UNPAUSED_INVENTORY_MENU_DEFAULT_VALUE);
		settings->unpausedMenus[RE::JournalMenu::MENU_NAME.data()] = IniGetBool(ini, "UNPAUSED_MENUS", "bJournalMenu", UNPAUSED_JOURNAL_MENU_DEFAULT_VALUE);
		settings->unpausedMenus[RE::LevelUpMenu::MENU_NAME.data()] = IniGetBool(ini, "UNPAUSED_MENUS", "bLevelUpMenu", UNPAUSED_LEVEL_UP_MENU_DEFAULT_VALUE);
		settings->unpausedMenus[RE::LockpickingMenu::MENU_NAME.data()] = IniGetBool(ini, "UNPAUSED_MENUS", "bLockpickingMenu", UNPAUSED_LOCKPICKING_MENU_DEFAULT_VALUE);
		settings->unpausedMenus[RE::MagicMenu::MENU_NAME.data()] = IniGetBool(ini, "UNPAUSED_MENUS", "bMagicMenu", UNPAUSED_MAGIC_MENU_DEFAULT_VALUE);
		settings->unpausedMenus[RE::MapMenu::MENU_NAME.data()] = IniGetBool(ini, "UNPAUSED_MENUS", "bMapMenu", UNPAUSED_MAP_MENU_DEFAULT_VALUE);
		settings->unpausedMenus[RE::MessageBoxMenu::MENU_NAME.data()] = IniGetBool(ini, "UNPAUSED_MENUS", "bMessageBoxMenu", UNPAUSED_MESSAGE_BOX_MENU_DEFAULT_VALUE);
		settings->unpausedMenus[RE::ModManagerMenu::MENU_NAME.data()] = IniGetBool(ini, "UNPAUSED_MENUS", "bModManagerMenu", UNPAUSED_MOD_MANAGER_MENU_DEFAULT_VALUE);
		settings->unpausedMenus[RE::SleepWaitMenu::MENU_NAME.data()] = IniGetBool(ini, "UNPAUSED_MENUS", "bSleepWaitMenu", UNPAUSED_SLEEP_WAIT_MENU_DEFAULT_VALUE);
		settings->unpausedMenus[RE::StatsMenu::MENU_NAME.data()] = IniGetBool(ini, "UNPAUSED_MENUS", "bStatsMenu", UNPAUSED_STATS_MENU_DEFAULT_VALUE);
		settings->unpausedMenus[RE::TrainingMenu::MENU_NAME.data()] = IniGetBool(ini, "UNPAUSED_MENUS", "bTrainingMenu", UNPAUSED_TRAINING_MENU_DEFAULT_VALUE);
		settings->unpausedMenus[RE::TutorialMenu::MENU_NAME.data()] = IniGetBool(ini, "UNPAUSED_MENUS", "bTutorialMenu", UNPAUSED_TUTORIAL_MENU_DEFAULT_VALUE);
		settings->unpausedMenus[RE::TweenMenu::MENU_NAME.data()] = IniGetBool(ini, "UNPAUSED_MENUS", "bTweenMenu", UNPAUSED_TWEEN_MENU_DEFAULT_VALUE);
		settings->unpausedMenus["CustomMenu"] = IniGetBool(ini, "UNPAUSED_MENUS", "bCustomMenu", UNPAUSED_CUSTOM_MENU_DEFAULT_VALUE);
		settings->unpausedMenus[QuestMenuEx::MENU_NAME.data()] = IniGetBool(ini, "UNPAUSED_MENUS", "bQuestJournalOverhaul_QuestMenu", UNPAUSED_QUEST_JOURNAL_OVERHAUL_QUEST_MENU_DEFAULT_VALUE);
		settings->unpausedMenus[BestiaryMenuEx::MENU_NAME.data()] = IniGetBool(ini, "UNPAUSED_MENUS", "bDragonbornsBestiary_BestiaryMenu", UNPAUSED_DRAGONBORNS_BESTIARY_BESTIARY_MENU_DEFAULT_VALUE);
		settings->pauseDuringCombat = IniGetBool(ini, "UNPAUSED_MENUS", "bPauseDuringCombat", PAUSE_DURING_COMBAT_DEFAULT_VALUE, "# If true, all affected menus will be paused if the player is in combat.");

		IniSection(ini, "AUTOCLOSE");
		settings->autoCloseMenus = IniGetBool(ini, "AUTOCLOSE", "bAutoCloseMenus", AUTO_CLOSE_MENUS_DEFAULT_VALUE, "#  Auto close menus if the distance to the object is larger than the specified value.");
		settings->autoCloseDistance = IniGetFloat(ini, "AUTOCLOSE", "fAutoCloseDistance", AUTO_CLOSE_DISTANCE_DEFAULT_VALUE, "#  The maximum distance (in in-game units) before the menus will automatically close.\n#  Too small values can cause the menus to close unexpectedly.");
		settings->autoCloseTolerance = IniGetFloat(ini, "AUTOCLOSE", "fAutoCloseTolerance", AUTO_CLOSE_TOLERANCE_DEFAULT_VALUE, "#  The maximum distance (in in-game units) where the menus NEVER auto-close (in relation to the players initial position when the menu was opened).\n#  This is used as a failsafe when the initial distance is larger than the maximum allowed to prevent the menu from closing immediately (e.g. a container opened by a script).");

		IniSection(ini, "DATA_UPDATES");
		settings->bottomBarMeterUpdateSteps = IniGetUInt(ini, "DATA_UPDATES", "iBottomBarMeterUpdateSteps", BOTTOM_BAR_METER_UPDATE_STEPS_DEFAULT_VALUE, "#  Actor value meter update divisor for the bottom bar. The bar updates only when the value crosses 1/divisor of the maximum. 0 updates on any change.");
		settings->updateInventoryMenuBottomBar = IniGetBool(ini, "DATA_UPDATES", "bUpdateInventoryMenuBottomBar", UPDATE_INVENTORY_MENU_BOTTOM_BAR_DEFAULT_VALUE);
		settings->updateContainerMenuBottomBar = IniGetBool(ini, "DATA_UPDATES", "bUpdateContainerMenuBottomBar", UPDATE_CONTAINER_MENU_BOTTOM_BAR_DEFAULT_VALUE);
		settings->updateContainerMenuPickpocketChance = IniGetBool(ini, "DATA_UPDATES", "bUpdateContainerMenuPickpocketChance", UPDATE_CONTAINER_MENU_PICKPOCKET_CHANCE_DEFAULT_VALUE);
		settings->updateMagicMenuBottomBar = IniGetBool(ini, "DATA_UPDATES", "bUpdateMagicMenuBottomBar", UPDATE_MAGIC_MENU_BOTTOM_BAR_DEFAULT_VALUE);
		settings->updateMagicMenuActiveEffectTimers = IniGetBool(ini, "DATA_UPDATES", "bUpdateMagicMenuActiveEffectTimers", UPDATE_MAGIC_MENU_ACTIVE_EFFECT_TIMERS_DEFAULT_VALUE);
		settings->updateStatsMenuPlayerInfo = IniGetBool(ini, "DATA_UPDATES", "bUpdateStatsMenuPlayerInfo", UPDATE_STATS_MENU_PLAYER_INFO_DEFAULT_VALUE);
		settings->updateStatsMenuSkillList = IniGetBool(ini, "DATA_UPDATES", "bUpdateStatsMenuSkillList", UPDATE_STATS_MENU_SKILL_LIST_DEFAULT_VALUE);
		settings->updateTweenMenuBottomBar = IniGetBool(ini, "DATA_UPDATES", "bUpdateTweenMenuBottomBar", UPDATE_TWEEN_MENU_BOTTOM_BAR_DEFAULT_VALUE);
		settings->updateJournalMenuBottomBar = IniGetBool(ini, "DATA_UPDATES", "bUpdateJournalMenuBottomBar", UPDATE_JOURNAL_MENU_BOTTOM_BAR_DEFAULT_VALUE);
		settings->updateMapMenuBottomBar = IniGetBool(ini, "DATA_UPDATES", "bUpdateMapMenuBottomBar", UPDATE_MAP_MENU_BOTTOM_BAR_DEFAULT_VALUE);
		settings->updateSleepWaitMenuClock = IniGetBool(ini, "DATA_UPDATES", "bUpdateSleepWaitMenuClock", UPDATE_SLEEP_WAIT_MENU_CLOCK_DEFAULT_VALUE);
		settings->updateQuestJournalOverhaulQuestMenuPlayerInfo = IniGetBool(ini, "DATA_UPDATES", "bUpdateQuestJournalOverhaulQuestMenuPlayerInfo", UPDATE_QUEST_JOURNAL_OVERHAUL_QUEST_MENU_PLAYER_INFO_DEFAULT_VALUE);

		IniSection(ini, "CONTROLS");
		settings->enableMovementInMenus = IniGetBool(ini, "CONTROLS", "bEnableMovementInMenus", ENABLE_MOVEMENT_IN_MENUS_DEFAULT_VALUE, "# If enabled, you will be able to move when a menu is open. Use the mouse (or the D-pad on controllers) to navigate the menus.\n#  (For controllers users) To change tabs in SkyUI favorites menu, use LB and RB buttons.");
		settings->enableToggleRun = IniGetBool(ini, "CONTROLS", "bEnableToggleRun", ENABLE_TOGGLE_RUN_DEFAULT_VALUE, "# Allows \"Toggle walk/run\" control to be available when in menus (Caps Lock by default).");
		settings->enableGamepadCameraMove = IniGetBool(ini, "CONTROLS", "bEnableGamepadCameraMove", ENABLE_GAMEPAD_CAMERA_MOVE_DEFAULT_VALUE, "# If enabled, you will be able to move the camera when using controllers. To rotate items in the inventory, maximize the preview first by pressing on the thumb stick.");
		settings->enableCursorCameraMove = IniGetBool(ini, "CONTROLS", "bEnableCursorCameraMove", ENABLE_CURSOR_CAMERA_MOVE_DEFAULT_VALUE, "# If enabled, you will be able to move the camera with the mouse by moving it to the edge of the screen (similar to how it works in the dialogue menu).");
		settings->cursorCameraVerticalSpeed = IniGetFloat(ini, "CONTROLS", "fCursorCameraVerticalSpeed", CURSOR_CAMERA_VERTICAL_SPEED_DEFAULT_VALUE, "# The vertical and horizontal speed the camera moves when bEnableCursorCameraMove is enabled.");
		settings->cursorCameraHorizontalSpeed = IniGetFloat(ini, "CONTROLS", "fCursorCameraHorizontalSpeed", CURSOR_CAMERA_HORIZONTAL_SPEED_DEFAULT_VALUE);
		settings->favoritesTabLeft = IniGetUInt(ini, "CONTROLS", "iFavoritesTabLeft", FAVORITES_TAB_LEFT_DEFAULT_VALUE, "# Gamepad button id used to switch to the previous tab in the SkyUI favorites menu. Default is LB (274).");
		settings->favoritesTabRight = IniGetUInt(ini, "CONTROLS", "iFavoritesTabRight", FAVORITES_TAB_RIGHT_DEFAULT_VALUE, "# Gamepad button id used to switch to the next tab in the SkyUI favorites menu. Default is RB (275).");

		IniSection(ini, "SLOWMOTION");
		settings->slowMotionMultiplier = IniGetFloat(ini, "SLOWMOTION", "fSlowMotionMultiplier", SLOW_MOTION_MULTIPLIER_DEFAULT_VALUE, "# This is the multiplier that will affect the game speed when a menu is open.\n# Default is 0.3, which is 30% of the full speed\n# Values lower than 0.25-0.3 can cause physics issues and are not recommended.\n# This feature also doesn't play nice with other sources of slowdowns, as it won't stack properly (like potion effects or the Slow Time shout).");
		settings->slowMotionCombatOnly = IniGetBool(ini, "SLOWMOTION", "bSlowMotionCombatOnly", SLOW_MOTION_COMBAT_ONLY_DEFAULT_VALUE, "# If enabled, slow-motion will only be applied when the player is in combat.");
		settings->slowMotionMenus[RE::BarterMenu::MENU_NAME.data()] = IniGetBool(ini, "SLOWMOTION", "bEnableSlowMotion_BarterMenu", SLOW_MOTION_BARTER_MENU_DEFAULT_VALUE, "#  Enable or disable slow-motion when a menu is open. Configurable on a per-menu basis.");
		settings->slowMotionMenus[RE::BookMenu::MENU_NAME.data()] = IniGetBool(ini, "SLOWMOTION", "bEnableSlowMotion_BookMenu", SLOW_MOTION_BOOK_MENU_DEFAULT_VALUE);
		settings->slowMotionMenus[RE::ContainerMenu::MENU_NAME.data()] = IniGetBool(ini, "SLOWMOTION", "bEnableSlowMotion_ContainerMenu", SLOW_MOTION_CONTAINER_MENU_DEFAULT_VALUE);
		settings->slowMotionMenus[RE::FavoritesMenu::MENU_NAME.data()] = IniGetBool(ini, "SLOWMOTION", "bEnableSlowMotion_FavoritesMenu", SLOW_MOTION_FAVORITES_MENU_DEFAULT_VALUE);
		settings->slowMotionMenus[RE::GiftMenu::MENU_NAME.data()] = IniGetBool(ini, "SLOWMOTION", "bEnableSlowMotion_GiftMenu", SLOW_MOTION_GIFT_MENU_DEFAULT_VALUE);
		settings->slowMotionMenus[RE::InventoryMenu::MENU_NAME.data()] = IniGetBool(ini, "SLOWMOTION", "bEnableSlowMotion_InventoryMenu", SLOW_MOTION_INVENTORY_MENU_DEFAULT_VALUE);
		settings->slowMotionMenus[RE::JournalMenu::MENU_NAME.data()] = IniGetBool(ini, "SLOWMOTION", "bEnableSlowMotion_JournalMenu", SLOW_MOTION_JOURNAL_MENU_DEFAULT_VALUE);
		settings->slowMotionMenus[RE::LevelUpMenu::MENU_NAME.data()] = IniGetBool(ini, "SLOWMOTION", "bEnableSlowMotion_LevelUpMenu", SLOW_MOTION_LEVEL_UP_MENU_DEFAULT_VALUE);
		settings->slowMotionMenus[RE::LockpickingMenu::MENU_NAME.data()] = IniGetBool(ini, "SLOWMOTION", "bEnableSlowMotion_LockpickingMenu", SLOW_MOTION_LOCKPICKING_MENU_DEFAULT_VALUE);
		settings->slowMotionMenus[RE::MagicMenu::MENU_NAME.data()] = IniGetBool(ini, "SLOWMOTION", "bEnableSlowMotion_MagicMenu", SLOW_MOTION_MAGIC_MENU_DEFAULT_VALUE);
		settings->slowMotionMenus[RE::MapMenu::MENU_NAME.data()] = IniGetBool(ini, "SLOWMOTION", "bEnableSlowMotion_MapMenu", SLOW_MOTION_MAP_MENU_DEFAULT_VALUE);
		settings->slowMotionMenus[RE::MessageBoxMenu::MENU_NAME.data()] = IniGetBool(ini, "SLOWMOTION", "bEnableSlowMotion_MessageBoxMenu", SLOW_MOTION_MESSAGE_BOX_MENU_DEFAULT_VALUE);
		settings->slowMotionMenus[RE::ModManagerMenu::MENU_NAME.data()] = IniGetBool(ini, "SLOWMOTION", "bEnableSlowMotion_ModManagerMenu", SLOW_MOTION_MOD_MANAGER_MENU_DEFAULT_VALUE);
		settings->slowMotionMenus[RE::SleepWaitMenu::MENU_NAME.data()] = IniGetBool(ini, "SLOWMOTION", "bEnableSlowMotion_SleepWaitMenu", SLOW_MOTION_SLEEP_WAIT_MENU_DEFAULT_VALUE);
		settings->slowMotionMenus[RE::StatsMenu::MENU_NAME.data()] = IniGetBool(ini, "SLOWMOTION", "bEnableSlowMotion_StatsMenu", SLOW_MOTION_STATS_MENU_DEFAULT_VALUE);
		settings->slowMotionMenus[RE::TrainingMenu::MENU_NAME.data()] = IniGetBool(ini, "SLOWMOTION", "bEnableSlowMotion_TrainingMenu", SLOW_MOTION_TRAINING_MENU_DEFAULT_VALUE);
		settings->slowMotionMenus[RE::TutorialMenu::MENU_NAME.data()] = IniGetBool(ini, "SLOWMOTION", "bEnableSlowMotion_TutorialMenu", SLOW_MOTION_TUTORIAL_MENU_DEFAULT_VALUE);
		settings->slowMotionMenus[RE::TweenMenu::MENU_NAME.data()] = IniGetBool(ini, "SLOWMOTION", "bEnableSlowMotion_TweenMenu", SLOW_MOTION_TWEEN_MENU_DEFAULT_VALUE);
		settings->slowMotionMenus["CustomMenu"] = IniGetBool(ini, "SLOWMOTION", "bEnableSlowMotion_CustomMenu", SLOW_MOTION_CUSTOM_MENU_DEFAULT_VALUE);
		settings->slowMotionMenus[QuestMenuEx::MENU_NAME.data()] = IniGetBool(ini, "SLOWMOTION", "bEnableSlowMotion_QuestJournalOverhaul_QuestMenu", SLOW_MOTION_QUEST_JOURNAL_OVERHAUL_QUEST_MENU_DEFAULT_VALUE);
		settings->slowMotionMenus[BestiaryMenuEx::MENU_NAME.data()] = IniGetBool(ini, "SLOWMOTION", "bEnableSlowMotion_DragonbornsBestiary_BestiaryMenu", SLOW_MOTION_DRAGONBORNS_BESTIARY_BESTIARY_MENU_DEFAULT_VALUE);

		IniSection(ini, "COMBAT_ALERT_OVERLAY", "# Shows a blinking red overlay when your character is in combat. Especially useful in full screen menus. You can enable or disable it individually for each menu.");
		settings->overlayMenus[RE::BarterMenu::MENU_NAME.data()] = IniGetBool(ini, "COMBAT_ALERT_OVERLAY", "bShowCAO_BarterMenu", OVERLAY_BARTER_MENU_DEFAULT_VALUE);
		settings->overlayMenus[RE::BookMenu::MENU_NAME.data()] = IniGetBool(ini, "COMBAT_ALERT_OVERLAY", "bShowCAO_BookMenu", OVERLAY_BOOK_MENU_DEFAULT_VALUE);
		settings->overlayMenus[RE::ContainerMenu::MENU_NAME.data()] = IniGetBool(ini, "COMBAT_ALERT_OVERLAY", "bShowCAO_ContainerMenu", OVERLAY_CONTAINER_MENU_DEFAULT_VALUE);
		settings->overlayMenus[RE::DialogueMenu::MENU_NAME.data()] = IniGetBool(ini, "COMBAT_ALERT_OVERLAY", "bShowCAO_DialogueMenu", OVERLAY_DIALOGUE_MENU_DEFAULT_VALUE);
		settings->overlayMenus[RE::FavoritesMenu::MENU_NAME.data()] = IniGetBool(ini, "COMBAT_ALERT_OVERLAY", "bShowCAO_FavoritesMenu", OVERLAY_FAVORITES_MENU_DEFAULT_VALUE);
		settings->overlayMenus[RE::GiftMenu::MENU_NAME.data()] = IniGetBool(ini, "COMBAT_ALERT_OVERLAY", "bShowCAO_GiftMenu", OVERLAY_GIFT_MENU_DEFAULT_VALUE);
		settings->overlayMenus[RE::InventoryMenu::MENU_NAME.data()] = IniGetBool(ini, "COMBAT_ALERT_OVERLAY", "bShowCAO_InventoryMenu", OVERLAY_INVENTORY_MENU_DEFAULT_VALUE);
		settings->overlayMenus[RE::JournalMenu::MENU_NAME.data()] = IniGetBool(ini, "COMBAT_ALERT_OVERLAY", "bShowCAO_JournalMenu", OVERLAY_JOURNAL_MENU_DEFAULT_VALUE);
		settings->overlayMenus[RE::LevelUpMenu::MENU_NAME.data()] = IniGetBool(ini, "COMBAT_ALERT_OVERLAY", "bShowCAO_LevelUpMenu", OVERLAY_LEVEL_UP_MENU_DEFAULT_VALUE);
		settings->overlayMenus[RE::LockpickingMenu::MENU_NAME.data()] = IniGetBool(ini, "COMBAT_ALERT_OVERLAY", "bShowCAO_LockpickingMenu", OVERLAY_LOCKPICKING_MENU_DEFAULT_VALUE);
		settings->overlayMenus[RE::MagicMenu::MENU_NAME.data()] = IniGetBool(ini, "COMBAT_ALERT_OVERLAY", "bShowCAO_MagicMenu", OVERLAY_MAGIC_MENU_DEFAULT_VALUE);
		settings->overlayMenus[RE::MapMenu::MENU_NAME.data()] = IniGetBool(ini, "COMBAT_ALERT_OVERLAY", "bShowCAO_MapMenu", OVERLAY_MAP_MENU_DEFAULT_VALUE);
		settings->overlayMenus[RE::MessageBoxMenu::MENU_NAME.data()] = IniGetBool(ini, "COMBAT_ALERT_OVERLAY", "bShowCAO_MessageBoxMenu", OVERLAY_MESSAGE_BOX_MENU_DEFAULT_VALUE);
		settings->overlayMenus[RE::ModManagerMenu::MENU_NAME.data()] = IniGetBool(ini, "COMBAT_ALERT_OVERLAY", "bShowCAO_ModManagerMenu", OVERLAY_MOD_MANAGER_MENU_DEFAULT_VALUE);
		settings->overlayMenus[RE::SleepWaitMenu::MENU_NAME.data()] = IniGetBool(ini, "COMBAT_ALERT_OVERLAY", "bShowCAO_SleepWaitMenu", OVERLAY_SLEEP_WAIT_MENU_DEFAULT_VALUE);
		settings->overlayMenus[RE::StatsMenu::MENU_NAME.data()] = IniGetBool(ini, "COMBAT_ALERT_OVERLAY", "bShowCAO_StatsMenu", OVERLAY_STATS_MENU_DEFAULT_VALUE);
		settings->overlayMenus[RE::TrainingMenu::MENU_NAME.data()] = IniGetBool(ini, "COMBAT_ALERT_OVERLAY", "bShowCAO_TrainingMenu", OVERLAY_TRAINING_MENU_DEFAULT_VALUE);
		settings->overlayMenus[RE::TutorialMenu::MENU_NAME.data()] = IniGetBool(ini, "COMBAT_ALERT_OVERLAY", "bShowCAO_TutorialMenu", OVERLAY_TUTORIAL_MENU_DEFAULT_VALUE);
		settings->overlayMenus[RE::TweenMenu::MENU_NAME.data()] = IniGetBool(ini, "COMBAT_ALERT_OVERLAY", "bShowCAO_TweenMenu", OVERLAY_TWEEN_MENU_DEFAULT_VALUE);
		settings->overlayMenus["CustomMenu"] = IniGetBool(ini, "COMBAT_ALERT_OVERLAY", "bShowCAO_CustomMenu", OVERLAY_CUSTOM_MENU_DEFAULT_VALUE);
		settings->overlayMenus[QuestMenuEx::MENU_NAME.data()] = IniGetBool(ini, "COMBAT_ALERT_OVERLAY", "bShowCAO_QuestJournalOverhaul_QuestMenu", OVERLAY_QUEST_JOURNAL_OVERHAUL_QUEST_MENU_DEFAULT_VALUE);
		settings->overlayMenus[BestiaryMenuEx::MENU_NAME.data()] = IniGetBool(ini, "COMBAT_ALERT_OVERLAY", "bShowCAO_DragonbornsBestiary_BestiaryMenu", OVERLAY_DRAGONBORNS_BESTIARY_BESTIARY_MENU_DEFAULT_VALUE);

		IniSection(ini, "HUD");
		settings->disableHUDModifications = IniGetBool(ini, "HUD", "bDisableHUDModifications", DISABLE_HUD_MODIFICATIONS_DEFAULT_VALUE, "# If set to true, SkyrimSouls will not attempt to modify the location of the sneak meter. Use it if you have a compatibility issue with something");
		settings->sneakMeterPosX = IniGetFloat(ini, "HUD", "fSneakMeterPosX", SNEAK_METER_POS_X_DEFAULT_VALUE, "# The position where the sneak meter will appear on the screen when a menu is open. It's necessary as some menu elements would hide it otherwise (eg. lockpicking).\n# The values are a bit arbitrary so just try different values until you find what suits you.");
		settings->sneakMeterPosY = IniGetFloat(ini, "HUD", "fSneakMeterPosY", SNEAK_METER_POS_Y_DEFAULT_VALUE);

		IniSection(ini, "BLUR");
		settings->disableBlur = IniGetBool(ini, "BLUR", "bDisableBlur", DISABLE_BLUR_DEFAULT_VALUE, "# If this is enabled, background blur will be disabled in menus");

		IniSection(ini, "MESSAGES");
		settings->hideEngineFixesWarning = IniGetBool(ini, "MESSAGES", "bHideEngineFixesWarning", HIDE_ENGINE_FIXES_WARNING_DEFAULT_VALUE, "# Disables the warning message on startup if Engine Fixes is not detected.");

		IniSection(ini, "MAP_MENU");
		settings->mapMenuAmbientSoundLoop = IniGetBool(ini, "MAP_MENU", "bMapMenuAmbientSoundLoop", MAP_MENU_AMBIENT_SOUND_LOOP_DEFAULT_VALUE, "# If enabled, the map menu looping ambient sound will play as normal. By default it is disabled since the game world is unpaused and both would play simultaneously.");
		settings->mapMenuCustomSky = IniGetBool(ini, "MAP_MENU", "bMapMenuCustomSky", MAP_MENU_CUSTOM_SKY_DEFAULT_VALUE, "# If enabled, the map menu will use a separate sky/weather state decoupled from the real world weather.\n# This prevents the map from affecting gameplay (e.g. weather changes, precipitation), but it can be invasive.\n# Highly recommended to keep it enabled, but can be disabled if it conflicts with something.");
		settings->mapMenuUniqueMapWeather = IniGetBool(ini, "MAP_MENU", "bMapMenuUniqueMapWeather", MAP_MENU_UNIQUE_MAP_WEATHER_DEFAULT_VALUE, "# Adds support for Unique Map Weather by doodlum when decoupled map weather is enabled.\n# Does nothing if the mod is not installed.");

		logger::info("Settings loaded.");

		ini.SaveFile(INI_PATH);
	}

	void SaveSettings()
	{
		Settings* settings = Settings::GetSingleton();

		CSimpleIniA ini;
		ini.SetUnicode();
		ini.LoadFile(INI_PATH);

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
		ini.SetBoolValue("UNPAUSED_MENUS", "bQuestJournalOverhaul_QuestMenu", settings->unpausedMenus[QuestMenuEx::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("UNPAUSED_MENUS", "bDragonbornsBestiary_BestiaryMenu", settings->unpausedMenus[BestiaryMenuEx::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("UNPAUSED_MENUS", "bPauseDuringCombat", settings->pauseDuringCombat, nullptr, true);

		ini.SetBoolValue("AUTOCLOSE", "bAutoCloseMenus", settings->autoCloseMenus, nullptr, true);
		ini.SetDoubleValue("AUTOCLOSE", "fAutoCloseDistance", settings->autoCloseDistance, nullptr, true);
		ini.SetDoubleValue("AUTOCLOSE", "fAutoCloseTolerance", settings->autoCloseTolerance, nullptr, true);

		ini.SetLongValue("DATA_UPDATES", "iBottomBarMeterUpdateSteps", settings->bottomBarMeterUpdateSteps, nullptr, false, true);
		ini.SetBoolValue("DATA_UPDATES", "bUpdateInventoryMenuBottomBar", settings->updateInventoryMenuBottomBar, nullptr, true);
		ini.SetBoolValue("DATA_UPDATES", "bUpdateContainerMenuBottomBar", settings->updateContainerMenuBottomBar, nullptr, true);
		ini.SetBoolValue("DATA_UPDATES", "bUpdateContainerMenuPickpocketChance", settings->updateContainerMenuPickpocketChance, nullptr, true);
		ini.SetBoolValue("DATA_UPDATES", "bUpdateMagicMenuBottomBar", settings->updateMagicMenuBottomBar, nullptr, true);
		ini.SetBoolValue("DATA_UPDATES", "bUpdateMagicMenuActiveEffectTimers", settings->updateMagicMenuActiveEffectTimers, nullptr, true);
		ini.SetBoolValue("DATA_UPDATES", "bUpdateStatsMenuPlayerInfo", settings->updateStatsMenuPlayerInfo, nullptr, true);
		ini.SetBoolValue("DATA_UPDATES", "bUpdateStatsMenuSkillList", settings->updateStatsMenuSkillList, nullptr, true);
		ini.SetBoolValue("DATA_UPDATES", "bUpdateTweenMenuBottomBar", settings->updateTweenMenuBottomBar, nullptr, true);
		ini.SetBoolValue("DATA_UPDATES", "bUpdateJournalMenuBottomBar", settings->updateJournalMenuBottomBar, nullptr, true);
		ini.SetBoolValue("DATA_UPDATES", "bUpdateMapMenuBottomBar", settings->updateMapMenuBottomBar, nullptr, true);
		ini.SetBoolValue("DATA_UPDATES", "bUpdateSleepWaitMenuClock", settings->updateSleepWaitMenuClock, nullptr, true);
		ini.SetBoolValue("DATA_UPDATES", "bUpdateQuestJournalOverhaulQuestMenuPlayerInfo", settings->updateQuestJournalOverhaulQuestMenuPlayerInfo, nullptr, true);

		ini.SetBoolValue("CONTROLS", "bEnableMovementInMenus", settings->enableMovementInMenus, nullptr, true);
		ini.SetBoolValue("CONTROLS", "bEnableToggleRun", settings->enableToggleRun, nullptr, true);
		ini.SetBoolValue("CONTROLS", "bEnableGamepadCameraMove", settings->enableGamepadCameraMove, nullptr, true);
		ini.SetBoolValue("CONTROLS", "bEnableCursorCameraMove", settings->enableCursorCameraMove, nullptr, true);
		ini.SetDoubleValue("CONTROLS", "fCursorCameraVerticalSpeed", settings->cursorCameraVerticalSpeed, nullptr, true);
		ini.SetDoubleValue("CONTROLS", "fCursorCameraHorizontalSpeed", settings->cursorCameraHorizontalSpeed, nullptr, true);
		ini.SetLongValue("CONTROLS", "iFavoritesTabLeft", settings->favoritesTabLeft, nullptr, false, true);
		ini.SetLongValue("CONTROLS", "iFavoritesTabRight", settings->favoritesTabRight, nullptr, false, true);

		ini.SetDoubleValue("SLOWMOTION", "fSlowMotionMultiplier", settings->slowMotionMultiplier, nullptr, true);
		ini.SetBoolValue("SLOWMOTION", "bSlowMotionCombatOnly", settings->slowMotionCombatOnly, nullptr, true);
		ini.SetBoolValue("SLOWMOTION", "bEnableSlowMotion_BarterMenu", settings->slowMotionMenus[RE::BarterMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("SLOWMOTION", "bEnableSlowMotion_BookMenu", settings->slowMotionMenus[RE::BookMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("SLOWMOTION", "bEnableSlowMotion_ContainerMenu", settings->slowMotionMenus[RE::ContainerMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("SLOWMOTION", "bEnableSlowMotion_FavoritesMenu", settings->slowMotionMenus[RE::FavoritesMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("SLOWMOTION", "bEnableSlowMotion_GiftMenu", settings->slowMotionMenus[RE::GiftMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("SLOWMOTION", "bEnableSlowMotion_InventoryMenu", settings->slowMotionMenus[RE::InventoryMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("SLOWMOTION", "bEnableSlowMotion_JournalMenu", settings->slowMotionMenus[RE::JournalMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("SLOWMOTION", "bEnableSlowMotion_LevelUpMenu", settings->slowMotionMenus[RE::LevelUpMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("SLOWMOTION", "bEnableSlowMotion_LockpickingMenu", settings->slowMotionMenus[RE::LockpickingMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("SLOWMOTION", "bEnableSlowMotion_MagicMenu", settings->slowMotionMenus[RE::MagicMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("SLOWMOTION", "bEnableSlowMotion_MapMenu", settings->slowMotionMenus[RE::MapMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("SLOWMOTION", "bEnableSlowMotion_MessageBoxMenu", settings->slowMotionMenus[RE::MessageBoxMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("SLOWMOTION", "bEnableSlowMotion_ModManagerMenu", settings->slowMotionMenus[RE::ModManagerMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("SLOWMOTION", "bEnableSlowMotion_SleepWaitMenu", settings->slowMotionMenus[RE::SleepWaitMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("SLOWMOTION", "bEnableSlowMotion_StatsMenu", settings->slowMotionMenus[RE::StatsMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("SLOWMOTION", "bEnableSlowMotion_TrainingMenu", settings->slowMotionMenus[RE::TrainingMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("SLOWMOTION", "bEnableSlowMotion_TutorialMenu", settings->slowMotionMenus[RE::TutorialMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("SLOWMOTION", "bEnableSlowMotion_TweenMenu", settings->slowMotionMenus[RE::TweenMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("SLOWMOTION", "bEnableSlowMotion_CustomMenu", settings->slowMotionMenus["CustomMenu"], nullptr, true);
		ini.SetBoolValue("SLOWMOTION", "bEnableSlowMotion_QuestJournalOverhaul_QuestMenu", settings->slowMotionMenus[QuestMenuEx::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("SLOWMOTION", "bEnableSlowMotion_DragonbornsBestiary_BestiaryMenu", settings->slowMotionMenus[BestiaryMenuEx::MENU_NAME.data()], nullptr, true);

		ini.SetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_BarterMenu", settings->overlayMenus[RE::BarterMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_BookMenu", settings->overlayMenus[RE::BookMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_ContainerMenu", settings->overlayMenus[RE::ContainerMenu::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_DialogueMenu", settings->overlayMenus[RE::DialogueMenu::MENU_NAME.data()], nullptr, true);
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
		ini.SetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_QuestJournalOverhaul_QuestMenu", settings->overlayMenus[QuestMenuEx::MENU_NAME.data()], nullptr, true);
		ini.SetBoolValue("COMBAT_ALERT_OVERLAY", "bShowCAO_DragonbornsBestiary_BestiaryMenu", settings->overlayMenus[BestiaryMenuEx::MENU_NAME.data()], nullptr, true);

		ini.SetBoolValue("HUD", "bDisableHUDModifications", settings->disableHUDModifications, nullptr, true);
		ini.SetDoubleValue("HUD", "fSneakMeterPosX", settings->sneakMeterPosX, nullptr, true);
		ini.SetDoubleValue("HUD", "fSneakMeterPosY", settings->sneakMeterPosY, nullptr, true);

		ini.SetBoolValue("BLUR", "bDisableBlur", settings->disableBlur, nullptr, true);

		ini.SetBoolValue("MESSAGES", "bHideEngineFixesWarning", settings->hideEngineFixesWarning, nullptr, true);

		ini.SetBoolValue("MAP_MENU", "bMapMenuAmbientSoundLoop", settings->mapMenuAmbientSoundLoop, nullptr, true);
		ini.SetBoolValue("MAP_MENU", "bMapMenuCustomSky", settings->mapMenuCustomSky, nullptr, true);
		ini.SetBoolValue("MAP_MENU", "bMapMenuUniqueMapWeather", settings->mapMenuUniqueMapWeather, nullptr, true);

		ini.SaveFile(INI_PATH);

		logger::info("Settings saved.");
	}

	void RestoreDefaults()
	{
		Settings* settings = Settings::GetSingleton();

		settings->unpausedMenus[RE::BarterMenu::MENU_NAME.data()] = UNPAUSED_BARTER_MENU_DEFAULT_VALUE;
		settings->unpausedMenus[RE::BookMenu::MENU_NAME.data()] = UNPAUSED_BOOK_MENU_DEFAULT_VALUE;
		settings->unpausedMenus[RE::Console::MENU_NAME.data()] = UNPAUSED_CONSOLE_DEFAULT_VALUE;
		settings->unpausedMenus[RE::ContainerMenu::MENU_NAME.data()] = UNPAUSED_CONTAINER_MENU_DEFAULT_VALUE;
		settings->unpausedMenus[RE::FavoritesMenu::MENU_NAME.data()] = UNPAUSED_FAVORITES_MENU_DEFAULT_VALUE;
		settings->unpausedMenus[RE::GiftMenu::MENU_NAME.data()] = UNPAUSED_GIFT_MENU_DEFAULT_VALUE;
		settings->unpausedMenus[RE::InventoryMenu::MENU_NAME.data()] = UNPAUSED_INVENTORY_MENU_DEFAULT_VALUE;
		settings->unpausedMenus[RE::JournalMenu::MENU_NAME.data()] = UNPAUSED_JOURNAL_MENU_DEFAULT_VALUE;
		settings->unpausedMenus[RE::LevelUpMenu::MENU_NAME.data()] = UNPAUSED_LEVEL_UP_MENU_DEFAULT_VALUE;
		settings->unpausedMenus[RE::LockpickingMenu::MENU_NAME.data()] = UNPAUSED_LOCKPICKING_MENU_DEFAULT_VALUE;
		settings->unpausedMenus[RE::MagicMenu::MENU_NAME.data()] = UNPAUSED_MAGIC_MENU_DEFAULT_VALUE;
		settings->unpausedMenus[RE::MapMenu::MENU_NAME.data()] = UNPAUSED_MAP_MENU_DEFAULT_VALUE;
		settings->unpausedMenus[RE::MessageBoxMenu::MENU_NAME.data()] = UNPAUSED_MESSAGE_BOX_MENU_DEFAULT_VALUE;
		settings->unpausedMenus[RE::ModManagerMenu::MENU_NAME.data()] = UNPAUSED_MOD_MANAGER_MENU_DEFAULT_VALUE;
		settings->unpausedMenus[RE::SleepWaitMenu::MENU_NAME.data()] = UNPAUSED_SLEEP_WAIT_MENU_DEFAULT_VALUE;
		settings->unpausedMenus[RE::StatsMenu::MENU_NAME.data()] = UNPAUSED_STATS_MENU_DEFAULT_VALUE;
		settings->unpausedMenus[RE::TrainingMenu::MENU_NAME.data()] = UNPAUSED_TRAINING_MENU_DEFAULT_VALUE;
		settings->unpausedMenus[RE::TutorialMenu::MENU_NAME.data()] = UNPAUSED_TUTORIAL_MENU_DEFAULT_VALUE;
		settings->unpausedMenus[RE::TweenMenu::MENU_NAME.data()] = UNPAUSED_TWEEN_MENU_DEFAULT_VALUE;
		settings->unpausedMenus["CustomMenu"] = UNPAUSED_CUSTOM_MENU_DEFAULT_VALUE;
		settings->unpausedMenus[QuestMenuEx::MENU_NAME.data()] = UNPAUSED_QUEST_JOURNAL_OVERHAUL_QUEST_MENU_DEFAULT_VALUE;
		settings->unpausedMenus[BestiaryMenuEx::MENU_NAME.data()] = UNPAUSED_DRAGONBORNS_BESTIARY_BESTIARY_MENU_DEFAULT_VALUE;

		settings->slowMotionMenus[RE::BarterMenu::MENU_NAME.data()] = SLOW_MOTION_BARTER_MENU_DEFAULT_VALUE;
		settings->slowMotionMenus[RE::BookMenu::MENU_NAME.data()] = SLOW_MOTION_BOOK_MENU_DEFAULT_VALUE;
		settings->slowMotionMenus[RE::ContainerMenu::MENU_NAME.data()] = SLOW_MOTION_CONTAINER_MENU_DEFAULT_VALUE;
		settings->slowMotionMenus[RE::FavoritesMenu::MENU_NAME.data()] = SLOW_MOTION_FAVORITES_MENU_DEFAULT_VALUE;
		settings->slowMotionMenus[RE::GiftMenu::MENU_NAME.data()] = SLOW_MOTION_GIFT_MENU_DEFAULT_VALUE;
		settings->slowMotionMenus[RE::InventoryMenu::MENU_NAME.data()] = SLOW_MOTION_INVENTORY_MENU_DEFAULT_VALUE;
		settings->slowMotionMenus[RE::JournalMenu::MENU_NAME.data()] = SLOW_MOTION_JOURNAL_MENU_DEFAULT_VALUE;
		settings->slowMotionMenus[RE::LevelUpMenu::MENU_NAME.data()] = SLOW_MOTION_LEVEL_UP_MENU_DEFAULT_VALUE;
		settings->slowMotionMenus[RE::LockpickingMenu::MENU_NAME.data()] = SLOW_MOTION_LOCKPICKING_MENU_DEFAULT_VALUE;
		settings->slowMotionMenus[RE::MagicMenu::MENU_NAME.data()] = SLOW_MOTION_MAGIC_MENU_DEFAULT_VALUE;
		settings->slowMotionMenus[RE::MapMenu::MENU_NAME.data()] = SLOW_MOTION_MAP_MENU_DEFAULT_VALUE;
		settings->slowMotionMenus[RE::MessageBoxMenu::MENU_NAME.data()] = SLOW_MOTION_MESSAGE_BOX_MENU_DEFAULT_VALUE;
		settings->slowMotionMenus[RE::ModManagerMenu::MENU_NAME.data()] = SLOW_MOTION_MOD_MANAGER_MENU_DEFAULT_VALUE;
		settings->slowMotionMenus[RE::SleepWaitMenu::MENU_NAME.data()] = SLOW_MOTION_SLEEP_WAIT_MENU_DEFAULT_VALUE;
		settings->slowMotionMenus[RE::StatsMenu::MENU_NAME.data()] = SLOW_MOTION_STATS_MENU_DEFAULT_VALUE;
		settings->slowMotionMenus[RE::TrainingMenu::MENU_NAME.data()] = SLOW_MOTION_TRAINING_MENU_DEFAULT_VALUE;
		settings->slowMotionMenus[RE::TutorialMenu::MENU_NAME.data()] = SLOW_MOTION_TUTORIAL_MENU_DEFAULT_VALUE;
		settings->slowMotionMenus[RE::TweenMenu::MENU_NAME.data()] = SLOW_MOTION_TWEEN_MENU_DEFAULT_VALUE;
		settings->slowMotionMenus["CustomMenu"] = SLOW_MOTION_CUSTOM_MENU_DEFAULT_VALUE;
		settings->slowMotionMenus[QuestMenuEx::MENU_NAME.data()] = SLOW_MOTION_QUEST_JOURNAL_OVERHAUL_QUEST_MENU_DEFAULT_VALUE;
		settings->slowMotionMenus[BestiaryMenuEx::MENU_NAME.data()] = SLOW_MOTION_DRAGONBORNS_BESTIARY_BESTIARY_MENU_DEFAULT_VALUE;

		settings->overlayMenus[RE::BarterMenu::MENU_NAME.data()] = OVERLAY_BARTER_MENU_DEFAULT_VALUE;
		settings->overlayMenus[RE::BookMenu::MENU_NAME.data()] = OVERLAY_BOOK_MENU_DEFAULT_VALUE;
		settings->overlayMenus[RE::ContainerMenu::MENU_NAME.data()] = OVERLAY_CONTAINER_MENU_DEFAULT_VALUE;
		settings->overlayMenus[RE::DialogueMenu::MENU_NAME.data()] = OVERLAY_DIALOGUE_MENU_DEFAULT_VALUE;
		settings->overlayMenus[RE::FavoritesMenu::MENU_NAME.data()] = OVERLAY_FAVORITES_MENU_DEFAULT_VALUE;
		settings->overlayMenus[RE::GiftMenu::MENU_NAME.data()] = OVERLAY_GIFT_MENU_DEFAULT_VALUE;
		settings->overlayMenus[RE::InventoryMenu::MENU_NAME.data()] = OVERLAY_INVENTORY_MENU_DEFAULT_VALUE;
		settings->overlayMenus[RE::JournalMenu::MENU_NAME.data()] = OVERLAY_JOURNAL_MENU_DEFAULT_VALUE;
		settings->overlayMenus[RE::LevelUpMenu::MENU_NAME.data()] = OVERLAY_LEVEL_UP_MENU_DEFAULT_VALUE;
		settings->overlayMenus[RE::LockpickingMenu::MENU_NAME.data()] = OVERLAY_LOCKPICKING_MENU_DEFAULT_VALUE;
		settings->overlayMenus[RE::MagicMenu::MENU_NAME.data()] = OVERLAY_MAGIC_MENU_DEFAULT_VALUE;
		settings->overlayMenus[RE::MapMenu::MENU_NAME.data()] = OVERLAY_MAP_MENU_DEFAULT_VALUE;
		settings->overlayMenus[RE::MessageBoxMenu::MENU_NAME.data()] = OVERLAY_MESSAGE_BOX_MENU_DEFAULT_VALUE;
		settings->overlayMenus[RE::ModManagerMenu::MENU_NAME.data()] = OVERLAY_MOD_MANAGER_MENU_DEFAULT_VALUE;
		settings->overlayMenus[RE::SleepWaitMenu::MENU_NAME.data()] = OVERLAY_SLEEP_WAIT_MENU_DEFAULT_VALUE;
		settings->overlayMenus[RE::StatsMenu::MENU_NAME.data()] = OVERLAY_STATS_MENU_DEFAULT_VALUE;
		settings->overlayMenus[RE::TrainingMenu::MENU_NAME.data()] = OVERLAY_TRAINING_MENU_DEFAULT_VALUE;
		settings->overlayMenus[RE::TutorialMenu::MENU_NAME.data()] = OVERLAY_TUTORIAL_MENU_DEFAULT_VALUE;
		settings->overlayMenus[RE::TweenMenu::MENU_NAME.data()] = OVERLAY_TWEEN_MENU_DEFAULT_VALUE;
		settings->overlayMenus["CustomMenu"] = OVERLAY_CUSTOM_MENU_DEFAULT_VALUE;
		settings->overlayMenus[QuestMenuEx::MENU_NAME.data()] = OVERLAY_QUEST_JOURNAL_OVERHAUL_QUEST_MENU_DEFAULT_VALUE;
		settings->overlayMenus[BestiaryMenuEx::MENU_NAME.data()] = OVERLAY_DRAGONBORNS_BESTIARY_BESTIARY_MENU_DEFAULT_VALUE;

		settings->pauseDuringCombat = PAUSE_DURING_COMBAT_DEFAULT_VALUE;

		settings->autoCloseMenus = AUTO_CLOSE_MENUS_DEFAULT_VALUE;
		settings->autoCloseDistance = AUTO_CLOSE_DISTANCE_DEFAULT_VALUE;
		settings->autoCloseTolerance = AUTO_CLOSE_TOLERANCE_DEFAULT_VALUE;

		settings->bottomBarMeterUpdateSteps = BOTTOM_BAR_METER_UPDATE_STEPS_DEFAULT_VALUE;
		settings->updateInventoryMenuBottomBar = UPDATE_INVENTORY_MENU_BOTTOM_BAR_DEFAULT_VALUE;
		settings->updateContainerMenuBottomBar = UPDATE_CONTAINER_MENU_BOTTOM_BAR_DEFAULT_VALUE;
		settings->updateContainerMenuPickpocketChance = UPDATE_CONTAINER_MENU_PICKPOCKET_CHANCE_DEFAULT_VALUE;
		settings->updateMagicMenuBottomBar = UPDATE_MAGIC_MENU_BOTTOM_BAR_DEFAULT_VALUE;
		settings->updateMagicMenuActiveEffectTimers = UPDATE_MAGIC_MENU_ACTIVE_EFFECT_TIMERS_DEFAULT_VALUE;
		settings->updateStatsMenuPlayerInfo = UPDATE_STATS_MENU_PLAYER_INFO_DEFAULT_VALUE;
		settings->updateStatsMenuSkillList = UPDATE_STATS_MENU_SKILL_LIST_DEFAULT_VALUE;
		settings->updateTweenMenuBottomBar = UPDATE_TWEEN_MENU_BOTTOM_BAR_DEFAULT_VALUE;
		settings->updateJournalMenuBottomBar = UPDATE_JOURNAL_MENU_BOTTOM_BAR_DEFAULT_VALUE;
		settings->updateMapMenuBottomBar = UPDATE_MAP_MENU_BOTTOM_BAR_DEFAULT_VALUE;
		settings->updateSleepWaitMenuClock = UPDATE_SLEEP_WAIT_MENU_CLOCK_DEFAULT_VALUE;
		settings->updateQuestJournalOverhaulQuestMenuPlayerInfo = UPDATE_QUEST_JOURNAL_OVERHAUL_QUEST_MENU_PLAYER_INFO_DEFAULT_VALUE;

		settings->enableMovementInMenus = ENABLE_MOVEMENT_IN_MENUS_DEFAULT_VALUE;
		settings->enableToggleRun = ENABLE_TOGGLE_RUN_DEFAULT_VALUE;
		settings->enableGamepadCameraMove = ENABLE_GAMEPAD_CAMERA_MOVE_DEFAULT_VALUE;
		settings->enableCursorCameraMove = ENABLE_CURSOR_CAMERA_MOVE_DEFAULT_VALUE;
		settings->cursorCameraVerticalSpeed = CURSOR_CAMERA_VERTICAL_SPEED_DEFAULT_VALUE;
		settings->cursorCameraHorizontalSpeed = CURSOR_CAMERA_HORIZONTAL_SPEED_DEFAULT_VALUE;
		settings->favoritesTabLeft = FAVORITES_TAB_LEFT_DEFAULT_VALUE;
		settings->favoritesTabRight = FAVORITES_TAB_RIGHT_DEFAULT_VALUE;

		settings->slowMotionMultiplier = SLOW_MOTION_MULTIPLIER_DEFAULT_VALUE;
		settings->slowMotionCombatOnly = SLOW_MOTION_COMBAT_ONLY_DEFAULT_VALUE;

		settings->disableHUDModifications = DISABLE_HUD_MODIFICATIONS_DEFAULT_VALUE;
		settings->sneakMeterPosX = SNEAK_METER_POS_X_DEFAULT_VALUE;
		settings->sneakMeterPosY = SNEAK_METER_POS_Y_DEFAULT_VALUE;

		settings->disableBlur = DISABLE_BLUR_DEFAULT_VALUE;

		settings->hideEngineFixesWarning = HIDE_ENGINE_FIXES_WARNING_DEFAULT_VALUE;

		settings->mapMenuAmbientSoundLoop = MAP_MENU_AMBIENT_SOUND_LOOP_DEFAULT_VALUE;
		settings->mapMenuCustomSky = MAP_MENU_CUSTOM_SKY_DEFAULT_VALUE;
		settings->mapMenuUniqueMapWeather = MAP_MENU_UNIQUE_MAP_WEATHER_DEFAULT_VALUE;

		SaveSettings();
	}
}
