#include "SkyrimSoulsRE.h"
#include "Windows.h"

namespace SkyrimSoulsRE
{
	using MenuFlag = RE::IMenu::Flag;

	std::map<std::string, RE::UI::Create_t*> menuCreatorMap;

	// Make sure that the menu can't open with the cursor right at the edge of the screen.
	// If the cursor is there, bump it inside by 10 pixels
	// Otherwise, the camera can move unexpectedly.
	void CheckCursorPosition()
	{
		RE::MenuCursor* menuCursor = RE::MenuCursor::GetSingleton();
		menuCursor->cursorPosX = std::clamp(menuCursor->cursorPosX, 10.0f, menuCursor->screenWidthX - 10.0f);
		menuCursor->cursorPosY = std::clamp(menuCursor->cursorPosY, 10.0f, menuCursor->screenWidthY - 10.0f);
	}

	RE::IMenu* CreateMenu(std::string_view a_menuName)
	{
		SKSE::log::info("Creating menu {}", a_menuName);

		RE::UI* ui = RE::UI::GetSingleton();
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
				menu->menuFlags.reset(MenuFlag::kPausesGame);

				if (!isConsole)
				{
					bool usesSlowMotion = settings->slowMotionMenus[a_menuName.data()];

					if (usesSlowMotion)
					{
						menu->menuFlags.set(static_cast<MenuFlag>(MenuFlagEx::kUsesSlowMotion));
					}

					menu->menuFlags.set(static_cast<MenuFlag>(MenuFlagEx::kUnpaused));
				}
			}

			RE::PlayerCharacter::GetSingleton()->InterruptCast(true);
		}

		if (!menu->RequiresUpdate())
		{
			menu->menuFlags.set(RE::IMenu::Flag::kRequiresUpdate);
		}

		if (usesOverlay && a_menuName != RE::HUDMenu::MENU_NAME)
		{
			menu->menuFlags.set(static_cast<MenuFlag>(MenuFlagEx::kUsesCombatAlertOverlay));

			if (!ui->IsMenuOpen(CombatAlertOverlayMenu::MENU_NAME))
			{
				RE::UIMessageQueue* msgQueue = RE::UIMessageQueue::GetSingleton();
				msgQueue->AddMessage(CombatAlertOverlayMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kShow, nullptr);
			}
		}

		if (menu->FreezeFrameBackground() && isUnpaused)
		{
			menu->menuFlags.reset(MenuFlag::kFreezeFrameBackground);
		}

		if (menu->InventoryItemMenu() && a_menuName != RE::FavoritesMenu::MENU_NAME)
		{
			menu->depthPriority = 1;
		}

		return menu;
	}

	std::uint32_t GetCombatAlertOverlayCount()
	{
		RE::UI* ui = RE::UI::GetSingleton();

		std::uint32_t count = 0;

		for (auto& it : ui->menuStack)
		{
			RE::IMenu* menu = it.get();
			if (menu->menuFlags.all(static_cast<MenuFlag>(MenuFlagEx::kUsesCombatAlertOverlay)))
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

		for (auto& it : ui->menuStack)
		{
			RE::IMenu* menu = it.get();
			if (menu->menuFlags.all(static_cast<MenuFlag>(MenuFlagEx::kUnpaused)))
			{
				++count;
			}
		}

		return count;
	}

	std::uint32_t GetSlowMotionCount()
	{
		RE::UI* ui = RE::UI::GetSingleton();

		std::uint32_t count = 0;

		for (auto& it : ui->menuStack)
		{
			RE::IMenu* menu = it.get();
			if (menu->menuFlags.all(static_cast<MenuFlag>(MenuFlagEx::kUsesSlowMotion)))
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
		ui->GetEventSource<RE::MenuOpenCloseEvent>()->AddEventSink(SlowMotionHandler::GetSingleton());

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

		ContainerMenuEx::ParseTranslations();
	}

	void Process_UI_Hook(RE::ScrapHeap* a_scrapHeap)
	{
		// Call original
		using func_t = void (*)(RE::ScrapHeap*);
		REL::Relocation<func_t> func(REL::ID(82084).address());
		func(a_scrapHeap);

		RE::UI* ui = RE::UI::GetSingleton();

		if (!ui->GameIsPaused())
		{
			using ProcessMessages_t = void (*)(RE::UI*);
			REL::Relocation<ProcessMessages_t> ProcessMessages(REL::ID(82082).address());
			ProcessMessages(ui);

			using AdvanceMenus_t = void (*)(RE::UI*);
			REL::Relocation<AdvanceMenus_t> AdvanceMenus(REL::ID(82083).address());
			AdvanceMenus(ui);

			using GetUnkExecuteUIScriptsSingleton_t = void* (*)(void);
			REL::Relocation<GetUnkExecuteUIScriptsSingleton_t> GetUnkExecuteUIScriptsSingleton(REL::ID(52950).address());
			void* unkExecuteUIScriptsSingleton = GetUnkExecuteUIScriptsSingleton();

			using ExecuteUIScripts_t = void (*)(void*);
			REL::Relocation<ExecuteUIScripts_t> ExecuteUIScripts(REL::ID(52952).address());
			ExecuteUIScripts(unkExecuteUIScriptsSingleton);
		}
	}

	void InstallHooks()
	{
		Papyrus::InstallHook();
		CameraMovement::InstallHook();
		Audio::InstallHook();
		ItemMenuUpdater::InstallHook();
		UIBlurManagerEx::InstallHook();
		MenuCache::InstallHook();

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

		// Disable UI job
		REL::safe_write(Offsets::Job::UI.address() + 0xB, std::uint8_t(0xEB));

		// Hook UI processing
		SKSE::GetTrampoline().write_call<5>(Offsets::Main::Update.address() + 0xADF, (uintptr_t)Process_UI_Hook);
	}
}
