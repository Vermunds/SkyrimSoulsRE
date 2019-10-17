#include "Events.h"

#include <vector>  // vector

#include "Settings.h"
#include "Hooks.h"
#include "Offsets.h"

#include "RE/Skyrim.h"

namespace SkyrimSoulsRE
{
	UInt8 unpausedMenuCount = 0;

	bool justOpenedContainer = false;
	bool justOpenedLockpicking = false;
	bool justOpenedBook = false;

	bool isInSlowMotion = false;

	RE::EventResult MenuOpenCloseEventHandler::ReceiveEvent(RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_dispatcher)
	{
		using RE::EventResult;
		typedef RE::IMenu::Flag Flag;

		RE::MenuManager*		mm = RE::MenuManager::GetSingleton();
		RE::UIStringHolder*	strHolder = RE::UIStringHolder::GetSingleton();
		RE::UIManager*		uiManager = RE::UIManager::GetSingleton();
		SkyrimSoulsRE::SettingStore* settings = SkyrimSoulsRE::SettingStore::GetSingleton();

		RE::IMenu* menu = mm->GetMenu(a_event->menuName).get();
		if (menu && menu->menuDepth == 0 && (a_event->menuName != strHolder->tweenMenu)) {
			menu->menuDepth++;
		}

		if (IsInWhiteList(a_event->menuName) && !(a_event->menuName == strHolder->console)) {
			//console might cause an underflow for some reason so it's not going to be affected by this
			(a_event->isOpening) ? (unpausedMenuCount++) : (unpausedMenuCount--);
		}

		if (a_event->isOpening) {

			//Needed for auto-close
			if (a_event->menuName == strHolder->containerMenu)
			{
				justOpenedContainer = true;
			}
			else if (a_event->menuName == strHolder->lockpickingMenu)
			{
				justOpenedLockpicking = true;
			}
			else if (a_event->menuName == strHolder->bookMenu)
			{
				justOpenedBook = true;
			}

			//Opening dialogue when an unpaused menu is open
			if ((a_event->menuName == strHolder->dialogueMenu) && unpausedMenuCount) {
				//Close tween menu if open
				uiManager->AddMessage(strHolder->tweenMenu, RE::UIMessage::Message::kClose, 0);

				RE::GFxMovieView* view = mm->GetMovieView(strHolder->dialogueMenu);
				view->SetVisible(false);
				mm->GetMenu(strHolder->dialogueMenu)->menuDepth = 0;
			}
			//Opening unpaused menu when a dialogue is open
			if (mm->IsMenuOpen(strHolder->dialogueMenu) && unpausedMenuCount) {

				RE::GFxMovieView* view = mm->GetMovieView(strHolder->dialogueMenu);
				view->SetVisible(false);
				mm->GetMenu(strHolder->dialogueMenu)->menuDepth = 0;
			}
		}
		else {
			//Closing menu when a dialogue is open
			if (mm->IsMenuOpen(strHolder->dialogueMenu) && !unpausedMenuCount) {
				RE::GFxMovieView* view = mm->GetMovieView(strHolder->dialogueMenu);
				view->SetVisible(true);
				mm->GetMenu(strHolder->dialogueMenu)->menuDepth = 3;
			}
			if (a_event->menuName == strHolder->cursorMenu && settings->GetSetting(settings->controls, "bAutoCenterCursor")->GetBool())
			{
				RE::NiPoint2* cursorPosition = reinterpret_cast<RE::NiPoint2*>(Offsets::CursorPosition.GetUIntPtr());
				RE::INIPrefSettingCollection* pref = RE::INIPrefSettingCollection::GetSingleton();

				cursorPosition->x = pref->GetSetting("iSize W:DISPLAY")->GetUInt() / 2.0;
				cursorPosition->y = pref->GetSetting("iSize H:DISPLAY")->GetUInt() / 2.0;
			}
		}

		static bool enableSlowMotion = settings->GetSetting(settings->slowMotion, "bEnableSlowMotion")->GetBool();

		if (enableSlowMotion)
		{
 			static UInt32 multiplierPercent = settings->GetSetting(settings->slowMotion, "uSlowMotionPercent")->GetInt();
			float* globalTimescale = reinterpret_cast<float*>(Offsets::GlobalTimescaleMultipler.GetUIntPtr());

			float multiplier;
			if (multiplierPercent >= 10 && 100 >= multiplierPercent)
			{
				multiplier = (float)multiplierPercent / 100.0;
			}
			else {
				multiplier = 1.0;
			}

			if (unpausedMenuCount && !isInSlowMotion)
			{
				isInSlowMotion = true;
				*globalTimescale = multiplier * (*globalTimescale);
			}
			else if (!unpausedMenuCount && isInSlowMotion){
				isInSlowMotion = false;
				*globalTimescale = (1.0 / multiplier) * (*globalTimescale);

				//Safety check
				if (*globalTimescale > 1.0)
				{
					*globalTimescale = 1.0;
				}
			}
		}

		if (unpausedMenuCount) {
			
			//Fix for QuickLoot
			RE::IMenu* lootMenu = mm->GetMenu("LootMenu").get();
			if (lootMenu) {
				lootMenu->view->SetVisible(false);
			}
		}


		if (!a_event || !IsInWhiteList(a_event->menuName)) {
			return EventResult::kContinue;
		}
	   
		if (menu) {
			if (a_event->menuName == strHolder->sleepWaitMenu)
			{
				Hooks::Register_Func(menu->fxDelegate.get(), Hooks::HookType::kSleepWaitMenu);
			}
			else if (a_event->menuName == strHolder->console)
			{
				Hooks::Register_Func(menu->fxDelegate.get(), Hooks::HookType::kConsole);
			}
			else if (a_event->menuName == strHolder->messageBoxMenu)
			{
				Hooks::Register_Func(menu->fxDelegate.get(), Hooks::HookType::kMessageBoxMenu);
			} 
			else if (a_event->menuName == strHolder->journalMenu)
			{
				Hooks::Register_Func(menu->fxDelegate.get(), Hooks::HookType::kJournalMenu);
			}
			
			if (menu->PausesGame()) {
				menu->flags &= ~Flag::kPauseGame;
				if (mm->GameIsPaused()) {
					mm->numPauseGame--;
				}
			}
			if (menu->StopsDrawingWorld()) {
				menu->flags &= ~Flag::kStopDrawingWorld;
			}
		}
		return EventResult::kContinue;
	}


	void MenuOpenCloseEventHandler::Init()
	{
		RE::UIStringHolder* strHolder = RE::UIStringHolder::GetSingleton();
		SettingStore* settings = SettingStore::GetSingleton();

		RE::BSFixedString customMenu = "CustomMenu";

		if (settings->GetSetting(settings->unpausedMenus, "tweenMenu")->GetBool()) {
			_whiteList.emplace_back(strHolder->tweenMenu);
		}
		if (settings->GetSetting(settings->unpausedMenus, "inventoryMenu")->GetBool()){
			_whiteList.emplace_back(strHolder->inventoryMenu);
		}
		if (settings->GetSetting(settings->unpausedMenus, "magicMenu")->GetBool()) {
			_whiteList.emplace_back(strHolder->magicMenu);
		}
		if (settings->GetSetting(settings->unpausedMenus, "barterMenu")->GetBool()) {
			_whiteList.emplace_back(strHolder->barterMenu);
		}
		if (settings->GetSetting(settings->unpausedMenus, "containerMenu")->GetBool()) {
			_whiteList.emplace_back(strHolder->containerMenu);
		}
		if (settings->GetSetting(settings->unpausedMenus, "giftMenu")->GetBool()) {
			_whiteList.emplace_back(strHolder->giftMenu);
		}
		if (settings->GetSetting(settings->unpausedMenus, "favoritesMenu")->GetBool()) {
			_whiteList.emplace_back(strHolder->favoritesMenu);
		}
		if (settings->GetSetting(settings->unpausedMenus, "tutorialMenu")->GetBool()) {
			_whiteList.emplace_back(strHolder->tutorialMenu);
		}
		if (settings->GetSetting(settings->unpausedMenus, "bookMenu")->GetBool()) {
			_whiteList.emplace_back(strHolder->bookMenu);
		}
		if (settings->GetSetting(settings->unpausedMenus, "lockpickingMenu")->GetBool()) {
			_whiteList.emplace_back(strHolder->lockpickingMenu);
		}
		if (settings->GetSetting(settings->unpausedMenus, "messageBoxMenu")->GetBool()) {
			_whiteList.emplace_back(strHolder->messageBoxMenu);
		}
		if (settings->GetSetting(settings->unpausedMenus, "trainingMenu")->GetBool()) {
			_whiteList.emplace_back(strHolder->trainingMenu);
		}
		if (settings->GetSetting(settings->unpausedMenus, "journalMenu")->GetBool()) {
			_whiteList.emplace_back(strHolder->journalMenu);
		}
		if (settings->GetSetting(settings->unpausedMenus, "sleepWaitMenu")->GetBool()) {
			_whiteList.emplace_back(strHolder->sleepWaitMenu);
		}
		/*
		if (settingStore->GetSetting("mapMenu")) {
			_whiteList.emplace_back(strHolder->mapMenu);
		}
		if (settingStore->GetSetting("statsMenu")) {
			_whiteList.emplace_back(strHolder->statsMenu);
			_whiteList.emplace_back(strHolder->levelUpMenu);
		}
		*/
		if (settings->GetSetting(settings->unpausedMenus, "console")->GetBool()) {
			_whiteList.emplace_back(strHolder->console);
		}
		if (settings->GetSetting(settings->unpausedMenus, "bethesdaModMenu")->GetBool()) {
			_whiteList.emplace_back(strHolder->modManagerMenu);
		}
		if (settings->GetSetting(settings->unpausedMenus, "customMenu")->GetBool()) {
			_whiteList.emplace_back(customMenu);
		}
		
	}


	bool MenuOpenCloseEventHandler::BlockInput()
	{
		static RE::MenuManager* mm = RE::MenuManager::GetSingleton();
		static RE::UIStringHolder* strHolder = RE::UIStringHolder::GetSingleton();

		for (auto& menuName : _whiteList) {
			if (mm->GetMovieView(menuName)) {
				if (menuName == strHolder->console && !(mm->IsMenuOpen(strHolder->console)))
				{
					return false;
				}
				return true;
			}
		}
		return false;
	}


	bool MenuOpenCloseEventHandler::IsInWhiteList(RE::BSFixedString& a_name)
	{
		for (auto& menu : _whiteList) {
			if (menu == a_name) {
				return true;
			}
		}
		return false;
	}

	std::vector<RE::BSFixedString> MenuOpenCloseEventHandler::_whiteList;

	MenuOpenCloseEventHandler g_menuOpenCloseEventHandler;

}
