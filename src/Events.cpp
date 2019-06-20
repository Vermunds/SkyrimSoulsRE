#include "Events.h"

#include <vector>  // vector

#include "Settings.h"

#include "RE/BSFixedString.h"  // BSFixedString
#include "RE/BSTEvent.h"  // EventResult, BSTEventSource
#include "RE/IMenu.h"  // IMenu
#include "RE/MenuManager.h"  // MenuManager
#include "RE/MenuOpenCloseEvent.h"  // MenuOpenCloseEvent
#include "RE/UIManager.h"  // UIManager
#include "RE/UIStringHolder.h"  // UIStringHolder

namespace SkyrimSoulsRE
{
	UInt8 unpausedMenuCount = 0;

	RE::EventResult MenuOpenCloseEventHandler::ReceiveEvent(RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_dispatcher)
	{
		using RE::EventResult;
		typedef RE::IMenu::Flag Flag;

		static RE::MenuManager*		mm = RE::MenuManager::GetSingleton();
		static RE::UIStringHolder*	strHolder = RE::UIStringHolder::GetSingleton();
		static RE::UIManager*		uiManager = RE::UIManager::GetSingleton();

		RE::BSFixedString lootMenuStr = "LootMenu";

		RE::IMenu* menu = mm->GetMenu(a_event->menuName);
		if (menu && menu->menuDepth == 0 && (a_event->menuName != strHolder->tweenMenu)) {
			menu->menuDepth++;
		}

		if (IsInWhiteList(a_event->menuName)) {
			(a_event->isOpening) ? (unpausedMenuCount++) : (unpausedMenuCount--);
		}

		if (a_event->isOpening) {
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
				mm->GetMenu(strHolder->dialogueMenu)->menuDepth = 0;
			}
		}

		//Fix for QuickLoot
		if (unpausedMenuCount) {
			RE::IMenu* lootMenu = mm->GetMenu(lootMenuStr);
			if (lootMenu) {
				lootMenu->view->SetVisible(false);
			}
		}

		if (!a_event || !IsInWhiteList(a_event->menuName)) {
			return EventResult::kContinue;
		}

		if (menu) {
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
		static RE::UIStringHolder* strHolder = RE::UIStringHolder::GetSingleton();

		bool unpauseTween = false;

		for (auto& menu : Settings::unpausedMenus) {
			if (menu == "inventoryMenu") {
				_whiteList.emplace_back(strHolder->inventoryMenu);
				unpauseTween = true;
			} if (menu == "magicMenu") {
				_whiteList.emplace_back(strHolder->magicMenu);
				unpauseTween = true;
			} else if (menu == "barterMenu") {
				_whiteList.emplace_back(strHolder->barterMenu);
			} else if (menu == "containerMenu") {
				_whiteList.emplace_back(strHolder->containerMenu);
			} else if (menu == "favoritesMenu") {
				_whiteList.emplace_back(strHolder->favoritesMenu);
			} else if (menu == "tutorialMenu") {
				_whiteList.emplace_back(strHolder->tutorialMenu);
			} else if (menu == "bookMenu") {
				_whiteList.emplace_back(strHolder->bookMenu);
			} else if (menu == "lockpickingMenu") {
				_whiteList.emplace_back(strHolder->lockpickingMenu);
			} else if (menu == "messageBoxMenu") {
				_whiteList.emplace_back(strHolder->messageBoxMenu);
			} else if (menu == "trainingMenu") {
				_whiteList.emplace_back(strHolder->trainingMenu);
			}
		}

		if (unpauseTween) {
			_whiteList.emplace_back(strHolder->tweenMenu);
		}
	}


	bool MenuOpenCloseEventHandler::BlockInput(const char* a_exclude)
	{
		static RE::MenuManager* mm = RE::MenuManager::GetSingleton();

		for (auto& menuName : _whiteList) {
			if (mm->GetMovieView(menuName) && std::strcmp(a_exclude, menuName.c_str()) != 0) {
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
