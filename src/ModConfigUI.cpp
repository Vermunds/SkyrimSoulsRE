#include "ModConfigUI.h"

#include "Settings.h"
#include "Util.h"
#include "Version.h"

#include "Menus/ModMenus/DragonbornsBestiary/BestiaryMenuEx.h"
#include "Menus/ModMenus/QuestJournalOverhaul/QuestMenuEx.h"

#include <ModConfigUI/Localization.h>

namespace SkyrimSoulsRE
{
	const char* Translate(const char* a_key)
	{
		return ModConfigUI::Localization::Get(a_key);
	}

	// Slider ranges
	constexpr float AUTO_CLOSE_DISTANCE_MIN = 100.0f;
	constexpr float AUTO_CLOSE_DISTANCE_MAX = 2000.0f;
	constexpr float AUTO_CLOSE_TOLERANCE_MIN = 0.0f;
	constexpr float AUTO_CLOSE_TOLERANCE_MAX = 1000.0f;

	constexpr std::int32_t METER_UPDATE_STEPS_MIN = 0;
	constexpr std::int32_t METER_UPDATE_STEPS_MAX = 1000;

	constexpr float CAMERA_SPEED_MIN = 0.01f;
	constexpr float CAMERA_SPEED_MAX = 1.0f;

	constexpr float SLOW_MOTION_MULTIPLIER_MIN = 0.1f;
	constexpr float SLOW_MOTION_MULTIPLIER_MAX = 1.0f;

	constexpr float SNEAK_METER_POS_X_MIN = 0.0f;
	constexpr float SNEAK_METER_POS_X_MAX = 1280.0f;
	constexpr float SNEAK_METER_POS_Y_MIN = 0.0f;
	constexpr float SNEAK_METER_POS_Y_MAX = 720.0f;

	// Widget helpers, every change is saved right away
	void SaveCheckbox(ModConfigUI::Renderer& a_renderer, const char* a_labelKey, const char* a_tooltipKey, bool* a_value, bool a_default)
	{
		if (a_renderer.Checkbox(Translate(a_labelKey), a_value, a_default, a_tooltipKey ? Translate(a_tooltipKey) : nullptr))
		{
			SaveSettings();
		}
	}

	void SaveSliderFloat(ModConfigUI::Renderer& a_renderer, const char* a_labelKey, const char* a_tooltipKey, float* a_value, float a_min, float a_max, const char* a_format, float a_default)
	{
		if (a_renderer.SliderFloat(Translate(a_labelKey), a_value, a_min, a_max, a_format, a_default, a_tooltipKey ? Translate(a_tooltipKey) : nullptr))
		{
			SaveSettings();
		}
	}

	void SaveGamepadBinder(ModConfigUI::Renderer& a_renderer, const char* a_labelKey, const char* a_tooltipKey, std::uint32_t* a_value, std::uint32_t a_default)
	{
		ModConfigUI::ButtonBinding binding{ *a_value };
		if (a_renderer.BindButton(Translate(a_labelKey), &binding, ModConfigUI::ButtonBinding{ a_default }, a_tooltipKey ? Translate(a_tooltipKey) : nullptr, ModConfigUI::BindFilter::kGamepad))
		{
			*a_value = binding.key;
			SaveSettings();
		}
	}

	// One checkbox of a menu list page. The lists are per page, since which menus a setting applies to differs.
	struct MenuCheckbox
	{
		const char* menuName;  // Key in the per-menu map of Settings the page edits
		const char* labelKey;
		bool defaultValue;
	};

	constexpr MenuCheckbox UNPAUSED_VANILLA_MENUS[] = {
		{ RE::BarterMenu::MENU_NAME.data(), "$SSRE_Menu_Barter", UNPAUSED_BARTER_MENU_DEFAULT_VALUE },
		{ RE::BookMenu::MENU_NAME.data(), "$SSRE_Menu_Book", UNPAUSED_BOOK_MENU_DEFAULT_VALUE },
		{ RE::Console::MENU_NAME.data(), "$SSRE_Menu_Console", UNPAUSED_CONSOLE_DEFAULT_VALUE },
		{ RE::ContainerMenu::MENU_NAME.data(), "$SSRE_Menu_Container", UNPAUSED_CONTAINER_MENU_DEFAULT_VALUE },
		{ RE::FavoritesMenu::MENU_NAME.data(), "$SSRE_Menu_Favorites", UNPAUSED_FAVORITES_MENU_DEFAULT_VALUE },
		{ RE::GiftMenu::MENU_NAME.data(), "$SSRE_Menu_Gift", UNPAUSED_GIFT_MENU_DEFAULT_VALUE },
		{ RE::InventoryMenu::MENU_NAME.data(), "$SSRE_Menu_Inventory", UNPAUSED_INVENTORY_MENU_DEFAULT_VALUE },
		{ RE::JournalMenu::MENU_NAME.data(), "$SSRE_Menu_Journal", UNPAUSED_JOURNAL_MENU_DEFAULT_VALUE },
		{ RE::LevelUpMenu::MENU_NAME.data(), "$SSRE_Menu_LevelUp", UNPAUSED_LEVEL_UP_MENU_DEFAULT_VALUE },
		{ RE::LockpickingMenu::MENU_NAME.data(), "$SSRE_Menu_Lockpicking", UNPAUSED_LOCKPICKING_MENU_DEFAULT_VALUE },
		{ RE::MagicMenu::MENU_NAME.data(), "$SSRE_Menu_Magic", UNPAUSED_MAGIC_MENU_DEFAULT_VALUE },
		{ RE::MapMenu::MENU_NAME.data(), "$SSRE_Menu_Map", UNPAUSED_MAP_MENU_DEFAULT_VALUE },
		{ RE::MessageBoxMenu::MENU_NAME.data(), "$SSRE_Menu_MessageBox", UNPAUSED_MESSAGE_BOX_MENU_DEFAULT_VALUE },
		{ RE::ModManagerMenu::MENU_NAME.data(), "$SSRE_Menu_ModManager", UNPAUSED_MOD_MANAGER_MENU_DEFAULT_VALUE },
		{ RE::SleepWaitMenu::MENU_NAME.data(), "$SSRE_Menu_SleepWait", UNPAUSED_SLEEP_WAIT_MENU_DEFAULT_VALUE },
		{ RE::StatsMenu::MENU_NAME.data(), "$SSRE_Menu_Stats", UNPAUSED_STATS_MENU_DEFAULT_VALUE },
		{ RE::TrainingMenu::MENU_NAME.data(), "$SSRE_Menu_Training", UNPAUSED_TRAINING_MENU_DEFAULT_VALUE },
		{ RE::TutorialMenu::MENU_NAME.data(), "$SSRE_Menu_Tutorial", UNPAUSED_TUTORIAL_MENU_DEFAULT_VALUE },
		{ RE::TweenMenu::MENU_NAME.data(), "$SSRE_Menu_Tween", UNPAUSED_TWEEN_MENU_DEFAULT_VALUE },
		{ "CustomMenu", "$SSRE_Menu_Custom", UNPAUSED_CUSTOM_MENU_DEFAULT_VALUE }
	};

	constexpr MenuCheckbox UNPAUSED_MOD_MENUS[] = {
		{ QuestMenuEx::MENU_NAME.data(), "$SSRE_Menu_QuestJournalOverhaul", UNPAUSED_QUEST_JOURNAL_OVERHAUL_QUEST_MENU_DEFAULT_VALUE },
		{ BestiaryMenuEx::MENU_NAME.data(), "$SSRE_Menu_DragonbornsBestiary", UNPAUSED_DRAGONBORNS_BESTIARY_BESTIARY_MENU_DEFAULT_VALUE }
	};

	// The console is left out, it is never slowed down.
	constexpr MenuCheckbox SLOW_MOTION_VANILLA_MENUS[] = {
		{ RE::BarterMenu::MENU_NAME.data(), "$SSRE_Menu_Barter", SLOW_MOTION_BARTER_MENU_DEFAULT_VALUE },
		{ RE::BookMenu::MENU_NAME.data(), "$SSRE_Menu_Book", SLOW_MOTION_BOOK_MENU_DEFAULT_VALUE },
		{ RE::ContainerMenu::MENU_NAME.data(), "$SSRE_Menu_Container", SLOW_MOTION_CONTAINER_MENU_DEFAULT_VALUE },
		{ RE::FavoritesMenu::MENU_NAME.data(), "$SSRE_Menu_Favorites", SLOW_MOTION_FAVORITES_MENU_DEFAULT_VALUE },
		{ RE::GiftMenu::MENU_NAME.data(), "$SSRE_Menu_Gift", SLOW_MOTION_GIFT_MENU_DEFAULT_VALUE },
		{ RE::InventoryMenu::MENU_NAME.data(), "$SSRE_Menu_Inventory", SLOW_MOTION_INVENTORY_MENU_DEFAULT_VALUE },
		{ RE::JournalMenu::MENU_NAME.data(), "$SSRE_Menu_Journal", SLOW_MOTION_JOURNAL_MENU_DEFAULT_VALUE },
		{ RE::LevelUpMenu::MENU_NAME.data(), "$SSRE_Menu_LevelUp", SLOW_MOTION_LEVEL_UP_MENU_DEFAULT_VALUE },
		{ RE::LockpickingMenu::MENU_NAME.data(), "$SSRE_Menu_Lockpicking", SLOW_MOTION_LOCKPICKING_MENU_DEFAULT_VALUE },
		{ RE::MagicMenu::MENU_NAME.data(), "$SSRE_Menu_Magic", SLOW_MOTION_MAGIC_MENU_DEFAULT_VALUE },
		{ RE::MapMenu::MENU_NAME.data(), "$SSRE_Menu_Map", SLOW_MOTION_MAP_MENU_DEFAULT_VALUE },
		{ RE::MessageBoxMenu::MENU_NAME.data(), "$SSRE_Menu_MessageBox", SLOW_MOTION_MESSAGE_BOX_MENU_DEFAULT_VALUE },
		{ RE::ModManagerMenu::MENU_NAME.data(), "$SSRE_Menu_ModManager", SLOW_MOTION_MOD_MANAGER_MENU_DEFAULT_VALUE },
		{ RE::SleepWaitMenu::MENU_NAME.data(), "$SSRE_Menu_SleepWait", SLOW_MOTION_SLEEP_WAIT_MENU_DEFAULT_VALUE },
		{ RE::StatsMenu::MENU_NAME.data(), "$SSRE_Menu_Stats", SLOW_MOTION_STATS_MENU_DEFAULT_VALUE },
		{ RE::TrainingMenu::MENU_NAME.data(), "$SSRE_Menu_Training", SLOW_MOTION_TRAINING_MENU_DEFAULT_VALUE },
		{ RE::TutorialMenu::MENU_NAME.data(), "$SSRE_Menu_Tutorial", SLOW_MOTION_TUTORIAL_MENU_DEFAULT_VALUE },
		{ RE::TweenMenu::MENU_NAME.data(), "$SSRE_Menu_Tween", SLOW_MOTION_TWEEN_MENU_DEFAULT_VALUE },
		{ "CustomMenu", "$SSRE_Menu_Custom", SLOW_MOTION_CUSTOM_MENU_DEFAULT_VALUE }
	};

	constexpr MenuCheckbox SLOW_MOTION_MOD_MENUS[] = {
		{ QuestMenuEx::MENU_NAME.data(), "$SSRE_Menu_QuestJournalOverhaul", SLOW_MOTION_QUEST_JOURNAL_OVERHAUL_QUEST_MENU_DEFAULT_VALUE },
		{ BestiaryMenuEx::MENU_NAME.data(), "$SSRE_Menu_DragonbornsBestiary", SLOW_MOTION_DRAGONBORNS_BESTIARY_BESTIARY_MENU_DEFAULT_VALUE }
	};

	// The dialogue menu is not unpaused by this mod, but it can still show the overlay. The console can not.
	constexpr MenuCheckbox OVERLAY_VANILLA_MENUS[] = {
		{ RE::BarterMenu::MENU_NAME.data(), "$SSRE_Menu_Barter", OVERLAY_BARTER_MENU_DEFAULT_VALUE },
		{ RE::BookMenu::MENU_NAME.data(), "$SSRE_Menu_Book", OVERLAY_BOOK_MENU_DEFAULT_VALUE },
		{ RE::ContainerMenu::MENU_NAME.data(), "$SSRE_Menu_Container", OVERLAY_CONTAINER_MENU_DEFAULT_VALUE },
		{ RE::DialogueMenu::MENU_NAME.data(), "$SSRE_Menu_Dialogue", OVERLAY_DIALOGUE_MENU_DEFAULT_VALUE },
		{ RE::FavoritesMenu::MENU_NAME.data(), "$SSRE_Menu_Favorites", OVERLAY_FAVORITES_MENU_DEFAULT_VALUE },
		{ RE::GiftMenu::MENU_NAME.data(), "$SSRE_Menu_Gift", OVERLAY_GIFT_MENU_DEFAULT_VALUE },
		{ RE::InventoryMenu::MENU_NAME.data(), "$SSRE_Menu_Inventory", OVERLAY_INVENTORY_MENU_DEFAULT_VALUE },
		{ RE::JournalMenu::MENU_NAME.data(), "$SSRE_Menu_Journal", OVERLAY_JOURNAL_MENU_DEFAULT_VALUE },
		{ RE::LevelUpMenu::MENU_NAME.data(), "$SSRE_Menu_LevelUp", OVERLAY_LEVEL_UP_MENU_DEFAULT_VALUE },
		{ RE::LockpickingMenu::MENU_NAME.data(), "$SSRE_Menu_Lockpicking", OVERLAY_LOCKPICKING_MENU_DEFAULT_VALUE },
		{ RE::MagicMenu::MENU_NAME.data(), "$SSRE_Menu_Magic", OVERLAY_MAGIC_MENU_DEFAULT_VALUE },
		{ RE::MapMenu::MENU_NAME.data(), "$SSRE_Menu_Map", OVERLAY_MAP_MENU_DEFAULT_VALUE },
		{ RE::MessageBoxMenu::MENU_NAME.data(), "$SSRE_Menu_MessageBox", OVERLAY_MESSAGE_BOX_MENU_DEFAULT_VALUE },
		{ RE::ModManagerMenu::MENU_NAME.data(), "$SSRE_Menu_ModManager", OVERLAY_MOD_MANAGER_MENU_DEFAULT_VALUE },
		{ RE::SleepWaitMenu::MENU_NAME.data(), "$SSRE_Menu_SleepWait", OVERLAY_SLEEP_WAIT_MENU_DEFAULT_VALUE },
		{ RE::StatsMenu::MENU_NAME.data(), "$SSRE_Menu_Stats", OVERLAY_STATS_MENU_DEFAULT_VALUE },
		{ RE::TrainingMenu::MENU_NAME.data(), "$SSRE_Menu_Training", OVERLAY_TRAINING_MENU_DEFAULT_VALUE },
		{ RE::TutorialMenu::MENU_NAME.data(), "$SSRE_Menu_Tutorial", OVERLAY_TUTORIAL_MENU_DEFAULT_VALUE },
		{ RE::TweenMenu::MENU_NAME.data(), "$SSRE_Menu_Tween", OVERLAY_TWEEN_MENU_DEFAULT_VALUE },
		{ "CustomMenu", "$SSRE_Menu_Custom", OVERLAY_CUSTOM_MENU_DEFAULT_VALUE }
	};

	constexpr MenuCheckbox OVERLAY_MOD_MENUS[] = {
		{ QuestMenuEx::MENU_NAME.data(), "$SSRE_Menu_QuestJournalOverhaul", OVERLAY_QUEST_JOURNAL_OVERHAUL_QUEST_MENU_DEFAULT_VALUE },
		{ BestiaryMenuEx::MENU_NAME.data(), "$SSRE_Menu_DragonbornsBestiary", OVERLAY_DRAGONBORNS_BESTIARY_BESTIARY_MENU_DEFAULT_VALUE }
	};

	// Draws a_menus split evenly into two columns, filled top to bottom.
	void DrawMenuColumns(ModConfigUI::Renderer& a_renderer, const char* a_id, std::map<std::string, bool>& a_values, std::span<const MenuCheckbox> a_menus)
	{
		if (a_menus.empty() || !a_renderer.BeginColumns(a_id, 2))
		{
			return;
		}

		std::size_t firstColumnCount = (a_menus.size() + 1) / 2;
		for (std::size_t i = 0; i < a_menus.size(); ++i)
		{
			if (i == 0 || i == firstColumnCount)
			{
				a_renderer.NextColumn();
			}
			// Every menu label key has a matching "<labelKey>_Tooltip" key
			std::string tooltipKey = std::string(a_menus[i].labelKey) + "_Tooltip";
			SaveCheckbox(a_renderer, a_menus[i].labelKey, tooltipKey.c_str(), &a_values[a_menus[i].menuName], a_menus[i].defaultValue);
		}

		a_renderer.EndColumns();
	}

	// One checkbox per menu for one of the per-menu maps of Settings, with two buttons that set all of them at once.
	// The labels of those buttons are passed in, since what "enabled" means depends on the setting.
	// a_firstButtonValue is the value the first button sets, the second one sets the opposite.
	void DrawMenuList(ModConfigUI::Renderer& a_renderer, std::map<std::string, bool>& a_values, std::span<const MenuCheckbox> a_vanillaMenus, std::span<const MenuCheckbox> a_modMenus, const char* a_firstButtonKey, bool a_firstButtonValue, const char* a_secondButtonKey)
	{
		a_renderer.SeparatorText(Translate("$SSRE_Section_VanillaMenus"));

		bool firstClicked = a_renderer.Button(Translate(a_firstButtonKey));
		a_renderer.SameLine();
		bool secondClicked = a_renderer.Button(Translate(a_secondButtonKey));

		if (firstClicked || secondClicked)
		{
			bool value = firstClicked ? a_firstButtonValue : !a_firstButtonValue;
			for (const MenuCheckbox& menu : a_vanillaMenus)
			{
				a_values[menu.menuName] = value;
			}
			for (const MenuCheckbox& menu : a_modMenus)
			{
				a_values[menu.menuName] = value;
			}
			SaveSettings();
		}

		DrawMenuColumns(a_renderer, "SSRE_VanillaMenus", a_values, a_vanillaMenus);

		a_renderer.Separator();

		DrawMenuColumns(a_renderer, "SSRE_ModMenus", a_values, a_modMenus);
	}

	// Pages
	void DrawUnpausedMenusPage(ModConfigUI::Renderer& a_renderer)
	{
		Settings* settings = Settings::GetSingleton();

		a_renderer.TextWrappedMuted(Translate("$SSRE_UnpausedMenus_Description"));

		a_renderer.SeparatorText(Translate("$SSRE_Section_General"));
		SaveCheckbox(a_renderer, "$SSRE_PauseDuringCombat", "$SSRE_PauseDuringCombat_Tooltip", &settings->pauseDuringCombat, PAUSE_DURING_COMBAT_DEFAULT_VALUE);

		DrawMenuList(a_renderer, settings->unpausedMenus, UNPAUSED_VANILLA_MENUS, UNPAUSED_MOD_MENUS, "$SSRE_PauseAll", false, "$SSRE_UnpauseAll");
	}

	void DrawAutoClosePage(ModConfigUI::Renderer& a_renderer)
	{
		Settings* settings = Settings::GetSingleton();

		a_renderer.SeparatorText(Translate("$SSRE_Section_General"));
		SaveCheckbox(a_renderer, "$SSRE_AutoCloseMenus", "$SSRE_AutoCloseMenus_Tooltip", &settings->autoCloseMenus, AUTO_CLOSE_MENUS_DEFAULT_VALUE);

		a_renderer.BeginDisabled(!settings->autoCloseMenus);
		SaveSliderFloat(a_renderer, "$SSRE_AutoCloseDistance", "$SSRE_AutoCloseDistance_Tooltip", &settings->autoCloseDistance, AUTO_CLOSE_DISTANCE_MIN, AUTO_CLOSE_DISTANCE_MAX, "%.0f", AUTO_CLOSE_DISTANCE_DEFAULT_VALUE);
		SaveSliderFloat(a_renderer, "$SSRE_AutoCloseTolerance", "$SSRE_AutoCloseTolerance_Tooltip", &settings->autoCloseTolerance, AUTO_CLOSE_TOLERANCE_MIN, AUTO_CLOSE_TOLERANCE_MAX, "%.0f", AUTO_CLOSE_TOLERANCE_DEFAULT_VALUE);
		a_renderer.EndDisabled();
	}

	void DrawDataUpdatesPage(ModConfigUI::Renderer& a_renderer)
	{
		Settings* settings = Settings::GetSingleton();

		a_renderer.TextWrappedMuted(Translate("$SSRE_DataUpdates_Description"));

		a_renderer.SeparatorText(Translate("$SSRE_Section_General"));

		std::int32_t shownSteps = static_cast<std::int32_t>(std::min<std::uint32_t>(settings->bottomBarMeterUpdateSteps, METER_UPDATE_STEPS_MAX));
		std::int32_t meterUpdateSteps = shownSteps;
		bool finished = a_renderer.SliderInt(Translate("$SSRE_MeterUpdateSteps"), &meterUpdateSteps, METER_UPDATE_STEPS_MIN, METER_UPDATE_STEPS_MAX, "%d", static_cast<std::int32_t>(BOTTOM_BAR_METER_UPDATE_STEPS_DEFAULT_VALUE), Translate("$SSRE_MeterUpdateSteps_Tooltip"));
		if (meterUpdateSteps != shownSteps)
		{
			settings->bottomBarMeterUpdateSteps = static_cast<std::uint32_t>(meterUpdateSteps);
		}
		if (finished)
		{
			SaveSettings();
		}

		// Sections are ordered alphabetically, each one gets its own ID scope since the checkbox labels repeat.
		a_renderer.SeparatorText(Translate("$SSRE_Menu_Container"));
		a_renderer.PushID(0);
		SaveCheckbox(a_renderer, "$SSRE_UpdateBottomBar", nullptr, &settings->updateContainerMenuBottomBar, UPDATE_CONTAINER_MENU_BOTTOM_BAR_DEFAULT_VALUE);
		SaveCheckbox(a_renderer, "$SSRE_UpdatePickpocketChance", nullptr, &settings->updateContainerMenuPickpocketChance, UPDATE_CONTAINER_MENU_PICKPOCKET_CHANCE_DEFAULT_VALUE);
		a_renderer.PopID();

		a_renderer.SeparatorText(Translate("$SSRE_Menu_Inventory"));
		a_renderer.PushID(1);
		SaveCheckbox(a_renderer, "$SSRE_UpdateBottomBar", nullptr, &settings->updateInventoryMenuBottomBar, UPDATE_INVENTORY_MENU_BOTTOM_BAR_DEFAULT_VALUE);
		a_renderer.PopID();

		a_renderer.SeparatorText(Translate("$SSRE_Menu_Journal"));
		a_renderer.PushID(2);
		SaveCheckbox(a_renderer, "$SSRE_UpdateBottomBar", nullptr, &settings->updateJournalMenuBottomBar, UPDATE_JOURNAL_MENU_BOTTOM_BAR_DEFAULT_VALUE);
		a_renderer.PopID();

		a_renderer.SeparatorText(Translate("$SSRE_Menu_Magic"));
		a_renderer.PushID(3);
		SaveCheckbox(a_renderer, "$SSRE_UpdateBottomBar", nullptr, &settings->updateMagicMenuBottomBar, UPDATE_MAGIC_MENU_BOTTOM_BAR_DEFAULT_VALUE);
		SaveCheckbox(a_renderer, "$SSRE_UpdateActiveEffectTimers", nullptr, &settings->updateMagicMenuActiveEffectTimers, UPDATE_MAGIC_MENU_ACTIVE_EFFECT_TIMERS_DEFAULT_VALUE);
		a_renderer.PopID();

		a_renderer.SeparatorText(Translate("$SSRE_Menu_Map"));
		a_renderer.PushID(4);
		SaveCheckbox(a_renderer, "$SSRE_UpdateBottomBar", nullptr, &settings->updateMapMenuBottomBar, UPDATE_MAP_MENU_BOTTOM_BAR_DEFAULT_VALUE);
		a_renderer.PopID();

		a_renderer.SeparatorText(Translate("$SSRE_Menu_QuestJournalOverhaul"));
		a_renderer.PushID(5);
		SaveCheckbox(a_renderer, "$SSRE_UpdatePlayerInfo", nullptr, &settings->updateQuestJournalOverhaulQuestMenuPlayerInfo, UPDATE_QUEST_JOURNAL_OVERHAUL_QUEST_MENU_PLAYER_INFO_DEFAULT_VALUE);
		a_renderer.PopID();

		a_renderer.SeparatorText(Translate("$SSRE_Menu_SleepWait"));
		a_renderer.PushID(6);
		SaveCheckbox(a_renderer, "$SSRE_UpdateClock", nullptr, &settings->updateSleepWaitMenuClock, UPDATE_SLEEP_WAIT_MENU_CLOCK_DEFAULT_VALUE);
		a_renderer.PopID();

		a_renderer.SeparatorText(Translate("$SSRE_Menu_Stats"));
		a_renderer.PushID(7);
		SaveCheckbox(a_renderer, "$SSRE_UpdatePlayerInfo", nullptr, &settings->updateStatsMenuPlayerInfo, UPDATE_STATS_MENU_PLAYER_INFO_DEFAULT_VALUE);
		SaveCheckbox(a_renderer, "$SSRE_UpdateSkillList", nullptr, &settings->updateStatsMenuSkillList, UPDATE_STATS_MENU_SKILL_LIST_DEFAULT_VALUE);
		a_renderer.PopID();

		a_renderer.SeparatorText(Translate("$SSRE_Menu_Tween"));
		a_renderer.PushID(8);
		SaveCheckbox(a_renderer, "$SSRE_UpdateBottomBar", nullptr, &settings->updateTweenMenuBottomBar, UPDATE_TWEEN_MENU_BOTTOM_BAR_DEFAULT_VALUE);
		a_renderer.PopID();
	}

	void DrawControlsPage(ModConfigUI::Renderer& a_renderer)
	{
		Settings* settings = Settings::GetSingleton();

		a_renderer.SeparatorText(Translate("$SSRE_Section_Movement"));
		SaveCheckbox(a_renderer, "$SSRE_EnableMovementInMenus", "$SSRE_EnableMovementInMenus_Tooltip", &settings->enableMovementInMenus, ENABLE_MOVEMENT_IN_MENUS_DEFAULT_VALUE);

		// The controls below only exist because the menus take player input, they do nothing on their own.
		a_renderer.BeginDisabled(!settings->enableMovementInMenus);
		SaveCheckbox(a_renderer, "$SSRE_EnableToggleRun", "$SSRE_EnableToggleRun_Tooltip", &settings->enableToggleRun, ENABLE_TOGGLE_RUN_DEFAULT_VALUE);
		a_renderer.EndDisabled();

		a_renderer.SeparatorText(Translate("$SSRE_Section_Camera"));
		SaveCheckbox(a_renderer, "$SSRE_EnableGamepadCameraMove", "$SSRE_EnableGamepadCameraMove_Tooltip", &settings->enableGamepadCameraMove, ENABLE_GAMEPAD_CAMERA_MOVE_DEFAULT_VALUE);
		SaveCheckbox(a_renderer, "$SSRE_EnableCursorCameraMove", "$SSRE_EnableCursorCameraMove_Tooltip", &settings->enableCursorCameraMove, ENABLE_CURSOR_CAMERA_MOVE_DEFAULT_VALUE);

		a_renderer.BeginDisabled(!settings->enableCursorCameraMove);
		SaveSliderFloat(a_renderer, "$SSRE_CursorCameraVerticalSpeed", "$SSRE_CursorCameraSpeed_Tooltip", &settings->cursorCameraVerticalSpeed, CAMERA_SPEED_MIN, CAMERA_SPEED_MAX, "%.2f", CURSOR_CAMERA_VERTICAL_SPEED_DEFAULT_VALUE);
		SaveSliderFloat(a_renderer, "$SSRE_CursorCameraHorizontalSpeed", "$SSRE_CursorCameraSpeed_Tooltip", &settings->cursorCameraHorizontalSpeed, CAMERA_SPEED_MIN, CAMERA_SPEED_MAX, "%.2f", CURSOR_CAMERA_HORIZONTAL_SPEED_DEFAULT_VALUE);
		a_renderer.EndDisabled();

		a_renderer.SeparatorText(Translate("$SSRE_Section_FavoritesMenu"));

		a_renderer.BeginDisabled(!settings->enableMovementInMenus);
		SaveGamepadBinder(a_renderer, "$SSRE_FavoritesTabLeft", "$SSRE_FavoritesTabLeft_Tooltip", &settings->favoritesTabLeft, FAVORITES_TAB_LEFT_DEFAULT_VALUE);
		SaveGamepadBinder(a_renderer, "$SSRE_FavoritesTabRight", "$SSRE_FavoritesTabRight_Tooltip", &settings->favoritesTabRight, FAVORITES_TAB_RIGHT_DEFAULT_VALUE);
		a_renderer.EndDisabled();
	}

	void DrawSlowMotionPage(ModConfigUI::Renderer& a_renderer)
	{
		Settings* settings = Settings::GetSingleton();

		a_renderer.TextWrappedMuted(Translate("$SSRE_SlowMotion_Description"));

		a_renderer.SeparatorText(Translate("$SSRE_Section_General"));
		SaveSliderFloat(a_renderer, "$SSRE_SlowMotionMultiplier", "$SSRE_SlowMotionMultiplier_Tooltip", &settings->slowMotionMultiplier, SLOW_MOTION_MULTIPLIER_MIN, SLOW_MOTION_MULTIPLIER_MAX, "%.2f", SLOW_MOTION_MULTIPLIER_DEFAULT_VALUE);
		SaveCheckbox(a_renderer, "$SSRE_SlowMotionCombatOnly", "$SSRE_SlowMotionCombatOnly_Tooltip", &settings->slowMotionCombatOnly, SLOW_MOTION_COMBAT_ONLY_DEFAULT_VALUE);

		DrawMenuList(a_renderer, settings->slowMotionMenus, SLOW_MOTION_VANILLA_MENUS, SLOW_MOTION_MOD_MENUS, "$SSRE_EnableAll", true, "$SSRE_DisableAll");
	}

	void DrawCombatAlertOverlayPage(ModConfigUI::Renderer& a_renderer)
	{
		Settings* settings = Settings::GetSingleton();

		a_renderer.TextWrappedMuted(Translate("$SSRE_CombatAlertOverlay_Description"));

		DrawMenuList(a_renderer, settings->overlayMenus, OVERLAY_VANILLA_MENUS, OVERLAY_MOD_MENUS, "$SSRE_EnableAll", true, "$SSRE_DisableAll");
	}

	void DrawMiscellaneousPage(ModConfigUI::Renderer& a_renderer)
	{
		Settings* settings = Settings::GetSingleton();

		a_renderer.SeparatorText(Translate("$SSRE_Section_HUD"));
		SaveCheckbox(a_renderer, "$SSRE_DisableHUDModifications", "$SSRE_DisableHUDModifications_Tooltip", &settings->disableHUDModifications, DISABLE_HUD_MODIFICATIONS_DEFAULT_VALUE);

		a_renderer.BeginDisabled(settings->disableHUDModifications);
		SaveSliderFloat(a_renderer, "$SSRE_SneakMeterPosX", "$SSRE_SneakMeterPos_Tooltip", &settings->sneakMeterPosX, SNEAK_METER_POS_X_MIN, SNEAK_METER_POS_X_MAX, "%.0f", SNEAK_METER_POS_X_DEFAULT_VALUE);
		SaveSliderFloat(a_renderer, "$SSRE_SneakMeterPosY", "$SSRE_SneakMeterPos_Tooltip", &settings->sneakMeterPosY, SNEAK_METER_POS_Y_MIN, SNEAK_METER_POS_Y_MAX, "%.0f", SNEAK_METER_POS_Y_DEFAULT_VALUE);
		a_renderer.EndDisabled();

		a_renderer.SeparatorText(Translate("$SSRE_Section_Blur"));
		SaveCheckbox(a_renderer, "$SSRE_DisableBlur", "$SSRE_DisableBlur_Tooltip", &settings->disableBlur, DISABLE_BLUR_DEFAULT_VALUE);

		a_renderer.SeparatorText(Translate("$SSRE_Menu_Map"));
		SaveCheckbox(a_renderer, "$SSRE_MapMenuAmbientSoundLoop", "$SSRE_MapMenuAmbientSoundLoop_Tooltip", &settings->mapMenuAmbientSoundLoop, MAP_MENU_AMBIENT_SOUND_LOOP_DEFAULT_VALUE);
		SaveCheckbox(a_renderer, "$SSRE_MapMenuCustomSky", "$SSRE_MapMenuCustomSky_Tooltip", &settings->mapMenuCustomSky, MAP_MENU_CUSTOM_SKY_DEFAULT_VALUE);
		a_renderer.BeginDisabled(!settings->mapMenuCustomSky);
		SaveCheckbox(a_renderer, "$SSRE_MapMenuUniqueMapWeather", "$SSRE_MapMenuUniqueMapWeather_Tooltip", &settings->mapMenuUniqueMapWeather, MAP_MENU_UNIQUE_MAP_WEATHER_DEFAULT_VALUE);
		a_renderer.EndDisabled();

		a_renderer.SeparatorText(Translate("$SSRE_Section_Messages"));
		SaveCheckbox(a_renderer, "$SSRE_HideEngineFixesWarning", "$SSRE_HideEngineFixesWarning_Tooltip", &settings->hideEngineFixesWarning, HIDE_ENGINE_FIXES_WARNING_DEFAULT_VALUE);
	}

	void InstallModConfigUI()
	{
		static constexpr ModConfigUI::ModInfo MOD_INFO{
			.pluginName = Version::NAME.data(),
			.displayName = Version::FORMATTED_NAME.data(),
			.version = Version::STRING.data(),
			.author = Version::AUTHOR.data(),
			.description = "$SSRE_Description",
			.nexusUrl = "https://www.nexusmods.com/skyrimspecialedition/mods/27859",
			.sourceUrl = "https://github.com/Vermunds/SkyrimSoulsRE"
		};

		static constexpr ModConfigUI::Page PAGES[] = {
			{ "$SSRE_Page_UnpausedMenus", &DrawUnpausedMenusPage },
			{ "$SSRE_Page_AutoClose", &DrawAutoClosePage },
			{ "$SSRE_Page_DataUpdates", &DrawDataUpdatesPage },
			{ "$SSRE_Page_Controls", &DrawControlsPage },
			{ "$SSRE_Page_SlowMotion", &DrawSlowMotionPage },
			{ "$SSRE_Page_CombatAlertOverlay", &DrawCombatAlertOverlayPage },
			{ "$SSRE_Page_Miscellaneous", &DrawMiscellaneousPage }
		};

		ModConfigUI::Install(MOD_INFO, PAGES, &RestoreDefaults);
	}
}
