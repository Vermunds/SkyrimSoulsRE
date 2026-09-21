#include "SkyrimSoulsRE.h"

#include "Menus/BarterMenuEx.h"
#include "Menus/BookMenuEx.h"
#include "Menus/CombatAlertOverlayMenu.h"
#include "Menus/ConsoleEx.h"
#include "Menus/ContainerMenuEx.h"
#include "Menus/CustomMenuEx.h"
#include "Menus/DialogueMenuEx.h"
#include "Menus/FavoritesMenuEx.h"
#include "Menus/GiftMenuEx.h"
#include "Menus/HUDMenuEx.h"
#include "Menus/InventoryMenuEx.h"
#include "Menus/JournalMenuEx.h"
#include "Menus/LevelUpMenuEx.h"
#include "Menus/LockpickingMenuEx.h"
#include "Menus/MagicMenuEx.h"
#include "Menus/MapMenuEx.h"
#include "Menus/MessageBoxMenuEx.h"
#include "Menus/ModManagerMenuEx.h"
#include "Menus/SleepWaitMenuEx.h"
#include "Menus/StatsMenuEx.h"
#include "Menus/TrainingMenuEx.h"
#include "Menus/TutorialMenuEx.h"
#include "Menus/TweenMenuEx.h"

#include "Menus/ModMenus/DragonbornsBestiary/BestiaryMenuEx.h"
#include "Menus/ModMenus/QuestJournalOverhaul/QuestMenuEx.h"

#include "MenuFlagHandler.h"

#include "Controls/BSWin32KeyboardDeviceEx.h"
#include "Controls/CameraMovement.h"
#include "Controls/InputHandlerEx.h"
#include "Controls/MenuControlsEx.h"
#include "Controls/PlayerControlsEx.h"

namespace SkyrimSoulsRE
{
	using MenuFlag = RE::IMenu::Flag;

	std::map<std::string, RE::UI::Create_t*> menuCreatorMap;

	static void ResetMenuInput()
	{
		// Make sure that the menu can't open with the cursor right at the edge of the screen.
		// If the cursor is there, bump it inside by 10 pixels
		// Otherwise, the camera can move unexpectedly.
		RE::MenuCursor* menuCursor = RE::MenuCursor::GetSingleton();
		menuCursor->cursorPosX = std::clamp(menuCursor->cursorPosX, 10.0f, menuCursor->screenWidthX - 10.0f);
		menuCursor->cursorPosY = std::clamp(menuCursor->cursorPosY, 10.0f, menuCursor->screenWidthY - 10.0f);

		// If a direction key is held when the menu opens, DirectionHandler::nextRepeat may already be exceeded, causing immediate navigation.
		// Reset it so the held key cannot fire until released and pressed again.
		RE::DirectionHandler* directionHandler = RE::MenuControls::GetSingleton()->directionHandler;
		if (directionHandler)
		{
			directionHandler->nextRepeat = std::numeric_limits<float>::max();
			directionHandler->currentRepeatCount = 0;
		}
	}

	RE::IMenu* CreateMenu(std::string_view a_menuName)
	{
		SKSE::log::info("Creating menu {}", a_menuName);

		RE::IMenu* menu = menuCreatorMap.find(a_menuName.data())->second();

		ResetMenuInput();

		if (menu->PausesGame())
		{
			RE::PlayerCharacter::GetSingleton()->InterruptCast(true);
		}

		if (!menu->RequiresUpdate())
		{
			menu->menuFlags.set(MenuFlag::kRequiresUpdate);
		}

		if (menu->InventoryItemMenu() && a_menuName != RE::FavoritesMenu::MENU_NAME)
		{
			menu->depthPriority = 1;
		}

		MenuFlagHandler::GetSingleton()->UpdateMenu(menu, a_menuName, false);

		return menu;
	}

	static std::uint32_t CountMenusWithFlag(MenuFlagEx a_flag)
	{
		RE::UI* ui = RE::UI::GetSingleton();
		std::uint32_t count = 0;
		for (auto& it : ui->menuStack)
		{
			if (it->menuFlags.all(static_cast<MenuFlag>(a_flag)))
			{
				++count;
			}
		}
		return count;
	}

	std::uint32_t GetCombatAlertOverlayCount()
	{
		return CountMenusWithFlag(MenuFlagEx::kUsesCombatAlertOverlay);
	}

	std::uint32_t GetUnpausedMenuCount()
	{
		return CountMenusWithFlag(MenuFlagEx::kUnpaused);
	}

	std::uint32_t GetSlowMotionCount()
	{
		return CountMenusWithFlag(MenuFlagEx::kUsesSlowMotion);
	}

	bool IsFullScreenMenuOpen()
	{
		RE::UI* ui = RE::UI::GetSingleton();
		return ui->IsMenuOpen(RE::MapMenu::MENU_NAME) || ui->IsMenuOpen(RE::StatsMenu::MENU_NAME);
	}

	bool IsInSurvivalMode()
	{
		static const RE::BGSDefaultObjectManager* dobj = RE::BGSDefaultObjectManager::GetSingleton();
		const RE::TESGlobal* survival = dobj ? dobj->GetObject<RE::TESGlobal>(RE::DEFAULT_OBJECT::kSurvivalModeEnabled) : nullptr;
		return survival ? survival->value == 1.0f : false;
	};

	void InstallMenuHooks()
	{
		RE::UI* ui = RE::UI::GetSingleton();

		menuCreatorMap.emplace(RE::ContainerMenu::MENU_NAME, ui->menuMap.find(RE::ContainerMenu::MENU_NAME)->second.create);
		menuCreatorMap.emplace(RE::InventoryMenu::MENU_NAME, ui->menuMap.find(RE::InventoryMenu::MENU_NAME)->second.create);
		menuCreatorMap.emplace(RE::MagicMenu::MENU_NAME, ui->menuMap.find(RE::MagicMenu::MENU_NAME)->second.create);
		menuCreatorMap.emplace(RE::TweenMenu::MENU_NAME, ui->menuMap.find(RE::TweenMenu::MENU_NAME)->second.create);
		menuCreatorMap.emplace(RE::SleepWaitMenu::MENU_NAME, ui->menuMap.find(RE::SleepWaitMenu::MENU_NAME)->second.create);
		menuCreatorMap.emplace(RE::BarterMenu::MENU_NAME, ui->menuMap.find(RE::BarterMenu::MENU_NAME)->second.create);
		menuCreatorMap.emplace(RE::CraftingMenu::MENU_NAME, ui->menuMap.find(RE::CraftingMenu::MENU_NAME)->second.create);
		menuCreatorMap.emplace(RE::GiftMenu::MENU_NAME, ui->menuMap.find(RE::GiftMenu::MENU_NAME)->second.create);
		menuCreatorMap.emplace(RE::FavoritesMenu::MENU_NAME, ui->menuMap.find(RE::FavoritesMenu::MENU_NAME)->second.create);
		menuCreatorMap.emplace(RE::TrainingMenu::MENU_NAME, ui->menuMap.find(RE::TrainingMenu::MENU_NAME)->second.create);
		menuCreatorMap.emplace(RE::TutorialMenu::MENU_NAME, ui->menuMap.find(RE::TutorialMenu::MENU_NAME)->second.create);
		menuCreatorMap.emplace(RE::LockpickingMenu::MENU_NAME, ui->menuMap.find(RE::LockpickingMenu::MENU_NAME)->second.create);
		menuCreatorMap.emplace(RE::BookMenu::MENU_NAME, ui->menuMap.find(RE::BookMenu::MENU_NAME)->second.create);
		menuCreatorMap.emplace(RE::Console::MENU_NAME, ui->menuMap.find(RE::Console::MENU_NAME)->second.create);
		menuCreatorMap.emplace(RE::JournalMenu::MENU_NAME, ui->menuMap.find(RE::JournalMenu::MENU_NAME)->second.create);
		menuCreatorMap.emplace(RE::MessageBoxMenu::MENU_NAME, ui->menuMap.find(RE::MessageBoxMenu::MENU_NAME)->second.create);
		menuCreatorMap.emplace(RE::ModManagerMenu::MENU_NAME, ui->menuMap.find(RE::ModManagerMenu::MENU_NAME)->second.create);
		menuCreatorMap.emplace(RE::HUDMenu::MENU_NAME, ui->menuMap.find(RE::HUDMenu::MENU_NAME)->second.create);
		menuCreatorMap.emplace(RE::MapMenu::MENU_NAME, ui->menuMap.find(RE::MapMenu::MENU_NAME)->second.create);
		menuCreatorMap.emplace(RE::StatsMenu::MENU_NAME, ui->menuMap.find(RE::StatsMenu::MENU_NAME)->second.create);
		menuCreatorMap.emplace(RE::LevelUpMenu::MENU_NAME, ui->menuMap.find(RE::LevelUpMenu::MENU_NAME)->second.create);
		menuCreatorMap.emplace("CustomMenu", ui->menuMap.find("CustomMenu")->second.create);

		ui->menuMap.find(RE::ContainerMenu::MENU_NAME)->second.create = ContainerMenuEx::Creator;
		ui->menuMap.find(RE::InventoryMenu::MENU_NAME)->second.create = InventoryMenuEx::Creator;
		ui->menuMap.find(RE::MagicMenu::MENU_NAME)->second.create = MagicMenuEx::Creator;
		ui->menuMap.find(RE::TweenMenu::MENU_NAME)->second.create = TweenMenuEx::Creator;
		ui->menuMap.find(RE::SleepWaitMenu::MENU_NAME)->second.create = SleepWaitMenuEx::Creator;
		ui->menuMap.find(RE::BarterMenu::MENU_NAME)->second.create = BarterMenuEx::Creator;
		ui->menuMap.find(RE::GiftMenu::MENU_NAME)->second.create = GiftMenuEx::Creator;
		ui->menuMap.find(RE::FavoritesMenu::MENU_NAME)->second.create = FavoritesMenuEx::Creator;
		ui->menuMap.find(RE::TrainingMenu::MENU_NAME)->second.create = TrainingMenuEx::Creator;
		ui->menuMap.find(RE::TutorialMenu::MENU_NAME)->second.create = TutorialMenuEx::Creator;
		ui->menuMap.find(RE::LockpickingMenu::MENU_NAME)->second.create = LockpickingMenuEx::Creator;
		ui->menuMap.find(RE::BookMenu::MENU_NAME)->second.create = BookMenuEx::Creator;
		ui->menuMap.find(RE::Console::MENU_NAME)->second.create = ConsoleEx::Creator;
		ui->menuMap.find(RE::JournalMenu::MENU_NAME)->second.create = JournalMenuEx::Creator;
		ui->menuMap.find(RE::MessageBoxMenu::MENU_NAME)->second.create = MessageBoxMenuEx::Creator;
		ui->menuMap.find(RE::ModManagerMenu::MENU_NAME)->second.create = ModManagerMenuEx::Creator;
		ui->menuMap.find(RE::HUDMenu::MENU_NAME)->second.create = HUDMenuEx::Creator;
		ui->menuMap.find(RE::MapMenu::MENU_NAME)->second.create = MapMenuEx::Creator;
		ui->menuMap.find(RE::StatsMenu::MENU_NAME)->second.create = StatsMenuEx::Creator;
		ui->menuMap.find(RE::LevelUpMenu::MENU_NAME)->second.create = LevelUpMenuEx::Creator;
		ui->menuMap.find("CustomMenu")->second.create = CustomMenuEx::Creator;

		ui->Register(CombatAlertOverlayMenu::MENU_NAME, CombatAlertOverlayMenu::Creator);

		if (auto it = ui->menuMap.find(QuestMenuEx::MENU_NAME); it != ui->menuMap.end())
		{
			menuCreatorMap.emplace(QuestMenuEx::MENU_NAME, it->second.create);
			it->second.create = QuestMenuEx::Creator;
		}

		if (auto it = ui->menuMap.find(BestiaryMenuEx::MENU_NAME); it != ui->menuMap.end())
		{
			menuCreatorMap.emplace(BestiaryMenuEx::MENU_NAME, it->second.create);
			it->second.create = BestiaryMenuEx::Creator;
		}
	}

	void InstallHooks()
	{
		Papyrus::InstallHook();
		CameraMovement::InstallHook();
		ItemMenuUpdater::InstallHook();
		UIBlurManagerEx::InstallHook();
		MenuProcessing::InstallHook();

		MenuControlsEx::InstallHook();
		PlayerControlsEx::InstallHook();
		BSWin32KeyboardDeviceEx::InstallHook();

		BarterMenuEx::InstallHook();
		BookMenuEx::InstallHook();
		ConsoleEx::InstallHook();
		ContainerMenuEx::InstallHook();
		DialogueMenuEx::InstallHook();
		FavoritesMenuEx::InstallHook();
		GiftMenuEx::InstallHook();
		HUDMenuEx::InstallHook();
		InventoryMenuEx::InstallHook();
		JournalMenuEx::InstallHook();
		LevelUpMenuEx::InstallHook();
		LockpickingMenuEx::InstallHook();
		MagicMenuEx::InstallHook();
		MapMenuEx::InstallHook();
		MessageBoxMenuEx::InstallHook();
		ModManagerMenuEx::InstallHook();
		SleepWaitMenuEx::InstallHook();
		StatsMenuEx::InstallHook();
		TrainingMenuEx::InstallHook();
		TutorialMenuEx::InstallHook();
		TweenMenuEx::InstallHook();
	}
}
