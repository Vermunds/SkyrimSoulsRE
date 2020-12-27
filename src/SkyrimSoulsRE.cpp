#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	using Flag = RE::IMenu::Flag;

	std::map<std::string, RE::UI::Create_t*> menuCreatorMap;

	//Used to make sure that the menu can't open with the cursor right at the edge of the screen
	void CheckCursorPosition()
	{
		RE::NiPoint2* cursorPosition = reinterpret_cast<RE::NiPoint2*>(Offsets::Misc::CursorPosition.address());
		RE::INIPrefSettingCollection* pref = RE::INIPrefSettingCollection::GetSingleton();

		std::uint32_t resX = pref->GetSetting("iSize W:DISPLAY")->GetUInt();
		std::uint32_t resY = pref->GetSetting("iSize H:DISPLAY")->GetUInt();

		if (cursorPosition->x < 10.0f)
		{
			cursorPosition->x = 10.0f;
		}
		else if (cursorPosition->x > resX - 10.0f)
		{
			cursorPosition->x = resX - 10.0f;
		}

		if (cursorPosition->y < 10.0f)
		{
			cursorPosition->y = 10.0f;
		}
		else if (cursorPosition->y > resY - 10.0f)
		{
			cursorPosition->x = resY - 10.0f;
		}
	}

	RE::IMenu* CreateMenu(std::string_view a_menuName)
	{	
		RE::UI* ui = RE::UI::GetSingleton();
		RE::UIBlurManager* blurManager = RE::UIBlurManager::GetSingleton();
		Settings* settings = Settings::GetSingleton();

		RE::IMenu* menu = menuCreatorMap.find(a_menuName.data())->second();

		CheckCursorPosition();

		bool isConsole = a_menuName == RE::Console::MENU_NAME;
		bool isUnpaused = settings->unpausedMenus[a_menuName.data()];
		bool usesOverlay = isConsole ? false : settings->overlayMenus[a_menuName.data()];

		if (menu->PausesGame())
		{

			if (isUnpaused)
			{
				menu->menuFlags &= ~Flag::kPausesGame;

				if (!isConsole)
				{
					menu->menuFlags |= static_cast<Flag>(MenuFlagEx::kUnpaused);
				}
			}

			RE::PlayerCharacter::GetSingleton()->InterruptCast(true);
		}

		if (usesOverlay && a_menuName != RE::HUDMenu::MENU_NAME)
		{
			menu->menuFlags |= static_cast<Flag>(MenuFlagEx::kUsesCombatAlertOverlay);
			if (!ui->IsMenuOpen(CombatAlertOverlayMenu::MENU_NAME))
			{
				RE::UIMessageQueue* msgQueue = RE::UIMessageQueue::GetSingleton();
				msgQueue->AddMessage(CombatAlertOverlayMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kShow, nullptr);
			}
		}

		if (menu->FreezeFrameBackground() && isUnpaused)
		{
			menu->menuFlags &= ~Flag::kFreezeFrameBackground;
		}

		if (menu->InventoryItemMenu())
		{
			menu->depthPriority = 1;
		}
		
		if (settings->disableBlur)
		{
			if (blurManager->blurCount > 0)
			{
				blurManager->DecrementBlurCount();
			}
			else if (a_menuName == RE::TweenMenu::MENU_NAME)
			{
				blurManager->blurCount--;
			}
		}

		return menu;
	}

	std::uint32_t GetCombatAlertOverlayCount()
	{
		RE::UI* ui = RE::UI::GetSingleton();

		std::uint32_t count = 0;

		for (auto & it : ui->menuStack)
		{
			RE::IMenu* menu = it.get();
			if ((menu->menuFlags & static_cast<Flag>(MenuFlagEx::kUsesCombatAlertOverlay)) != Flag::kNone)
			{
				++count;
			}
		}

		return count;
	}

	std::uint32_t GetUnpausedMenuCount()
	{
		RE::UI* ui = RE::UI::GetSingleton();

		std::uint32_t count = 0;

		for (auto & it : ui->menuStack)
		{
			RE::IMenu* menu = it.get();
			if ((menu->menuFlags & static_cast<Flag>(MenuFlagEx::kUnpaused)) != Flag::kNone)
			{
				++count;
			}
		}

		return count;
	}

	bool IsFullScreenMenuOpen()
	{
		RE::UI* ui = RE::UI::GetSingleton();
		return ui->IsMenuOpen(RE::MapMenu::MENU_NAME) || ui->IsMenuOpen(RE::StatsMenu::MENU_NAME);
	}

	void InstallMenuHooks()
	{
		RE::UI* ui = RE::UI::GetSingleton();
		ui->GetEventSource<RE::MenuOpenCloseEvent>()->AddEventSink(&g_slowMotionHandler);

		menuCreatorMap.emplace(RE::ContainerMenu::MENU_NAME, ui->menuMap.find(RE::ContainerMenu::MENU_NAME)->second.create);
		menuCreatorMap.emplace(RE::DialogueMenu::MENU_NAME, ui->menuMap.find(RE::DialogueMenu::MENU_NAME)->second.create);
		menuCreatorMap.emplace(RE::InventoryMenu::MENU_NAME, ui->menuMap.find(RE::InventoryMenu::MENU_NAME)->second.create);
		menuCreatorMap.emplace(RE::MagicMenu::MENU_NAME, ui->menuMap.find(RE::MagicMenu::MENU_NAME)->second.create);
		menuCreatorMap.emplace(RE::TweenMenu::MENU_NAME, ui->menuMap.find(RE::TweenMenu::MENU_NAME)->second.create);
		menuCreatorMap.emplace(RE::SleepWaitMenu::MENU_NAME, ui->menuMap.find(RE::SleepWaitMenu::MENU_NAME)->second.create);
		menuCreatorMap.emplace(RE::BarterMenu::MENU_NAME, ui->menuMap.find(RE::BarterMenu::MENU_NAME)->second.create);
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
		ui->menuMap.find(RE::MapMenu::MENU_NAME)->second.create = MapMenuEx::Creator;;
		ui->menuMap.find(RE::StatsMenu::MENU_NAME)->second.create = StatsMenuEx::Creator;
		ui->menuMap.find(RE::LevelUpMenu::MENU_NAME)->second.create = LevelUpMenuEx::Creator;
		ui->menuMap.find("CustomMenu")->second.create = CustomMenuEx::Creator;

		ui->Register(CombatAlertOverlayMenu::MENU_NAME, CombatAlertOverlayMenu::Creator);
	}

	void InstallHooks()
	{	

		UnpausedTaskQueue::InstallHook();

		Papyrus::InstallHook();
		ConsoleCommandHooks::InstallHook();
		CameraMovement::InstallHook();
		Audio::InstallHook();
		ItemMenuUpdater::InstallHook();

		MenuControlsEx::InstallHook();
		PlayerControlsEx::InstallHook();

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
