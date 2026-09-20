#pragma once

namespace SkyrimSoulsRE
{
	//Unpaused Menus
	constexpr bool UNPAUSED_BARTER_MENU_DEFAULT_VALUE = true;
	constexpr bool UNPAUSED_BOOK_MENU_DEFAULT_VALUE = true;
	constexpr bool UNPAUSED_CONSOLE_DEFAULT_VALUE = true;
	constexpr bool UNPAUSED_CONTAINER_MENU_DEFAULT_VALUE = true;
	constexpr bool UNPAUSED_FAVORITES_MENU_DEFAULT_VALUE = true;
	constexpr bool UNPAUSED_GIFT_MENU_DEFAULT_VALUE = true;
	constexpr bool UNPAUSED_INVENTORY_MENU_DEFAULT_VALUE = true;
	constexpr bool UNPAUSED_JOURNAL_MENU_DEFAULT_VALUE = true;
	constexpr bool UNPAUSED_LEVEL_UP_MENU_DEFAULT_VALUE = true;
	constexpr bool UNPAUSED_LOCKPICKING_MENU_DEFAULT_VALUE = true;
	constexpr bool UNPAUSED_MAGIC_MENU_DEFAULT_VALUE = true;
	constexpr bool UNPAUSED_MAP_MENU_DEFAULT_VALUE = true;
	constexpr bool UNPAUSED_MESSAGE_BOX_MENU_DEFAULT_VALUE = true;
	constexpr bool UNPAUSED_MOD_MANAGER_MENU_DEFAULT_VALUE = true;
	constexpr bool UNPAUSED_SLEEP_WAIT_MENU_DEFAULT_VALUE = true;
	constexpr bool UNPAUSED_STATS_MENU_DEFAULT_VALUE = true;
	constexpr bool UNPAUSED_TRAINING_MENU_DEFAULT_VALUE = true;
	constexpr bool UNPAUSED_TUTORIAL_MENU_DEFAULT_VALUE = true;
	constexpr bool UNPAUSED_TWEEN_MENU_DEFAULT_VALUE = true;
	constexpr bool UNPAUSED_CUSTOM_MENU_DEFAULT_VALUE = true;
	constexpr bool UNPAUSED_QUEST_JOURNAL_OVERHAUL_QUEST_MENU_DEFAULT_VALUE = true;
	constexpr bool UNPAUSED_DRAGONBORNS_BESTIARY_BESTIARY_MENU_DEFAULT_VALUE = true;
	constexpr bool PAUSE_DURING_COMBAT_DEFAULT_VALUE = false;

	//AutoClose
	constexpr bool AUTO_CLOSE_MENUS_DEFAULT_VALUE = true;
	constexpr float AUTO_CLOSE_DISTANCE_DEFAULT_VALUE = 400.f;
	constexpr float AUTO_CLOSE_TOLERANCE_DEFAULT_VALUE = 100.f;

	//Data Updates
	constexpr std::uint32_t BOTTOM_BAR_METER_UPDATE_STEPS_DEFAULT_VALUE = 100;
	constexpr bool UPDATE_INVENTORY_MENU_BOTTOM_BAR_DEFAULT_VALUE = true;
	constexpr bool UPDATE_CONTAINER_MENU_BOTTOM_BAR_DEFAULT_VALUE = true;
	constexpr bool UPDATE_CONTAINER_MENU_PICKPOCKET_CHANCE_DEFAULT_VALUE = true;
	constexpr bool UPDATE_MAGIC_MENU_BOTTOM_BAR_DEFAULT_VALUE = true;
	constexpr bool UPDATE_MAGIC_MENU_ACTIVE_EFFECT_TIMERS_DEFAULT_VALUE = true;
	constexpr bool UPDATE_STATS_MENU_PLAYER_INFO_DEFAULT_VALUE = true;
	constexpr bool UPDATE_STATS_MENU_SKILL_LIST_DEFAULT_VALUE = true;
	constexpr bool UPDATE_TWEEN_MENU_BOTTOM_BAR_DEFAULT_VALUE = true;
	constexpr bool UPDATE_JOURNAL_MENU_BOTTOM_BAR_DEFAULT_VALUE = true;
	constexpr bool UPDATE_MAP_MENU_BOTTOM_BAR_DEFAULT_VALUE = true;
	constexpr bool UPDATE_SLEEP_WAIT_MENU_CLOCK_DEFAULT_VALUE = true;
	constexpr bool UPDATE_QUEST_JOURNAL_OVERHAUL_QUEST_MENU_PLAYER_INFO_DEFAULT_VALUE = true;

	//Controls
	constexpr bool ENABLE_MOVEMENT_IN_MENUS_DEFAULT_VALUE = true;
	constexpr bool ENABLE_TOGGLE_RUN_DEFAULT_VALUE = false;
	constexpr bool ENABLE_GAMEPAD_CAMERA_MOVE_DEFAULT_VALUE = true;
	constexpr bool ENABLE_CURSOR_CAMERA_MOVE_DEFAULT_VALUE = true;
	constexpr float CURSOR_CAMERA_VERTICAL_SPEED_DEFAULT_VALUE = 0.15f;
	constexpr float CURSOR_CAMERA_HORIZONTAL_SPEED_DEFAULT_VALUE = 0.25f;
	constexpr std::uint32_t FAVORITES_TAB_LEFT_DEFAULT_VALUE = 274;   // LB, Util::GamepadMaskToKeycode(kLeftShoulder)
	constexpr std::uint32_t FAVORITES_TAB_RIGHT_DEFAULT_VALUE = 275;  // RB, Util::GamepadMaskToKeycode(kRightShoulder)

	//slow motion
	constexpr float SLOW_MOTION_MULTIPLIER_DEFAULT_VALUE = 0.3f;
	constexpr bool SLOW_MOTION_COMBAT_ONLY_DEFAULT_VALUE = false;
	constexpr bool SLOW_MOTION_BARTER_MENU_DEFAULT_VALUE = false;
	constexpr bool SLOW_MOTION_BOOK_MENU_DEFAULT_VALUE = false;
	constexpr bool SLOW_MOTION_CONTAINER_MENU_DEFAULT_VALUE = false;
	constexpr bool SLOW_MOTION_FAVORITES_MENU_DEFAULT_VALUE = false;
	constexpr bool SLOW_MOTION_GIFT_MENU_DEFAULT_VALUE = false;
	constexpr bool SLOW_MOTION_INVENTORY_MENU_DEFAULT_VALUE = false;
	constexpr bool SLOW_MOTION_JOURNAL_MENU_DEFAULT_VALUE = false;
	constexpr bool SLOW_MOTION_LEVEL_UP_MENU_DEFAULT_VALUE = false;
	constexpr bool SLOW_MOTION_LOCKPICKING_MENU_DEFAULT_VALUE = false;
	constexpr bool SLOW_MOTION_MAGIC_MENU_DEFAULT_VALUE = false;
	constexpr bool SLOW_MOTION_MAP_MENU_DEFAULT_VALUE = false;
	constexpr bool SLOW_MOTION_MESSAGE_BOX_MENU_DEFAULT_VALUE = false;
	constexpr bool SLOW_MOTION_MOD_MANAGER_MENU_DEFAULT_VALUE = false;
	constexpr bool SLOW_MOTION_SLEEP_WAIT_MENU_DEFAULT_VALUE = false;
	constexpr bool SLOW_MOTION_STATS_MENU_DEFAULT_VALUE = false;
	constexpr bool SLOW_MOTION_TRAINING_MENU_DEFAULT_VALUE = false;
	constexpr bool SLOW_MOTION_TUTORIAL_MENU_DEFAULT_VALUE = false;
	constexpr bool SLOW_MOTION_TWEEN_MENU_DEFAULT_VALUE = false;
	constexpr bool SLOW_MOTION_CUSTOM_MENU_DEFAULT_VALUE = false;
	constexpr bool SLOW_MOTION_QUEST_JOURNAL_OVERHAUL_QUEST_MENU_DEFAULT_VALUE = false;
	constexpr bool SLOW_MOTION_DRAGONBORNS_BESTIARY_BESTIARY_MENU_DEFAULT_VALUE = false;

	//Combat Alert Overlay
	constexpr bool OVERLAY_BARTER_MENU_DEFAULT_VALUE = true;
	constexpr bool OVERLAY_BOOK_MENU_DEFAULT_VALUE = true;
	constexpr bool OVERLAY_CONTAINER_MENU_DEFAULT_VALUE = true;
	constexpr bool OVERLAY_DIALOGUE_MENU_DEFAULT_VALUE = true;
	constexpr bool OVERLAY_FAVORITES_MENU_DEFAULT_VALUE = true;
	constexpr bool OVERLAY_GIFT_MENU_DEFAULT_VALUE = true;
	constexpr bool OVERLAY_INVENTORY_MENU_DEFAULT_VALUE = true;
	constexpr bool OVERLAY_JOURNAL_MENU_DEFAULT_VALUE = true;
	constexpr bool OVERLAY_LEVEL_UP_MENU_DEFAULT_VALUE = true;
	constexpr bool OVERLAY_LOCKPICKING_MENU_DEFAULT_VALUE = true;
	constexpr bool OVERLAY_MAGIC_MENU_DEFAULT_VALUE = true;
	constexpr bool OVERLAY_MAP_MENU_DEFAULT_VALUE = true;
	constexpr bool OVERLAY_MESSAGE_BOX_MENU_DEFAULT_VALUE = true;
	constexpr bool OVERLAY_MOD_MANAGER_MENU_DEFAULT_VALUE = true;
	constexpr bool OVERLAY_SLEEP_WAIT_MENU_DEFAULT_VALUE = true;
	constexpr bool OVERLAY_STATS_MENU_DEFAULT_VALUE = true;
	constexpr bool OVERLAY_TRAINING_MENU_DEFAULT_VALUE = true;
	constexpr bool OVERLAY_TUTORIAL_MENU_DEFAULT_VALUE = true;
	constexpr bool OVERLAY_TWEEN_MENU_DEFAULT_VALUE = true;
	constexpr bool OVERLAY_CUSTOM_MENU_DEFAULT_VALUE = true;
	constexpr bool OVERLAY_QUEST_JOURNAL_OVERHAUL_QUEST_MENU_DEFAULT_VALUE = true;
	constexpr bool OVERLAY_DRAGONBORNS_BESTIARY_BESTIARY_MENU_DEFAULT_VALUE = true;

	//HUD
	constexpr bool DISABLE_HUD_MODIFICATIONS_DEFAULT_VALUE = false;
	constexpr float SNEAK_METER_POS_X_DEFAULT_VALUE = 24.f;
	constexpr float SNEAK_METER_POS_Y_DEFAULT_VALUE = 120.f;

	//Blur
	constexpr bool DISABLE_BLUR_DEFAULT_VALUE = false;

	//Messages
	constexpr bool HIDE_ENGINE_FIXES_WARNING_DEFAULT_VALUE = false;

	// Map Menu
	constexpr bool MAP_MENU_AMBIENT_SOUND_LOOP_DEFAULT_VALUE = false;
	constexpr bool MAP_MENU_CUSTOM_SKY_DEFAULT_VALUE = true;

	class Settings
	{
	public:
		static Settings* GetSingleton();

		//Unpaused Menus
		std::map<std::string, bool> unpausedMenus;
		bool pauseDuringCombat;

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
		bool updateQuestJournalOverhaulQuestMenuPlayerInfo;

		//Controls
		bool enableMovementInMenus;
		bool enableToggleRun;
		bool enableGamepadCameraMove;
		bool enableCursorCameraMove;
		float cursorCameraVerticalSpeed;
		float cursorCameraHorizontalSpeed;
		std::uint32_t favoritesTabLeft;
		std::uint32_t favoritesTabRight;

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

		// Internal
		bool isUsingDialogueMovementEnabler = false;
		bool isUsingGoToBed = false;

	private:
		Settings() {};
		~Settings() {};
		Settings(const Settings&) = delete;
		Settings& operator=(const Settings&) = delete;
	};

	void LoadSettings();
	void SaveSettings();
	void RestoreDefaults();
}
