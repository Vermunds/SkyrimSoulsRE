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

#pragma optimize("", off)

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

		RE::IMenu* menu = mm->GetMenu(a_event->menuName);
		if (menu && menu->menuDepth == 0 && (a_event->menuName != strHolder->tweenMenu)) {
			menu->menuDepth++;
		}

		if (a_event->isOpening) {
			if (a_event->menuName == strHolder->dialogueMenu) {
				//Close tween and lockpicking menu if open
				uiManager->AddMessage(strHolder->tweenMenu, RE::UIMessage::Message::kClose, 0);
				uiManager->AddMessage(strHolder->lockpickingMenu, RE::UIMessage::Message::kClose, 0);
			}
			//Forcegreet fix
			if ((a_event->menuName == strHolder->dialogueMenu) && (mm->IsMenuOpen(strHolder->barterMenu) || mm->IsMenuOpen(strHolder->giftMenu) || mm->IsMenuOpen(strHolder->containerMenu)
				|| mm->IsMenuOpen(strHolder->inventoryMenu) || mm->IsMenuOpen(strHolder->magicMenu) || mm->IsMenuOpen(strHolder->trainingMenu) || mm->IsMenuOpen(strHolder->bookMenu)
				|| mm->IsMenuOpen(strHolder->favoritesMenu))) {

				RE::GFxMovieView* view = mm->GetMovieView(strHolder->dialogueMenu);
				view->SetVisible(false);
				mm->GetMenu(strHolder->dialogueMenu)->menuDepth = 0;
			}
			//Open menu from dialogue (barter, training, container, gift menu)
			if (mm->IsMenuOpen(strHolder->dialogueMenu) && (a_event->menuName == strHolder->barterMenu || a_event->menuName == strHolder->giftMenu || a_event->menuName == strHolder->containerMenu
				|| a_event->menuName == strHolder->trainingMenu)) {

				RE::GFxMovieView* view = mm->GetMovieView(strHolder->dialogueMenu);
				view->SetVisible(false);
				mm->GetMenu(strHolder->dialogueMenu)->menuDepth = 0;
			}
		} else {
			//Closing menu on top of dialogue
			if (mm->IsMenuOpen(strHolder->dialogueMenu) && (a_event->menuName == strHolder->barterMenu || a_event->menuName == strHolder->giftMenu || a_event->menuName == strHolder->containerMenu
				|| a_event->menuName == strHolder->inventoryMenu || a_event->menuName == strHolder->magicMenu || a_event->menuName == strHolder->trainingMenu || a_event->menuName == strHolder->bookMenu
				|| a_event->menuName == strHolder->favoritesMenu)) {

				RE::GFxMovieView* view = mm->GetMovieView(strHolder->dialogueMenu);
				view->SetVisible(true);
				mm->GetMenu(strHolder->dialogueMenu)->menuDepth = 3;
			}
		}

		if (IsInWhiteList(a_event->menuName)) {
			if (a_event->isOpening) {
				unpausedMenuCount++;
			} else {
				unpausedMenuCount--;
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
			} /*else if (menu == "sleepWaitMenu") {
				_whiteList.emplace_back(strHolder->sleepWaitMenu);
			} else if (menu == "journalMenu") {
				_whiteList.emplace_back(strHolder->journalMenu);
			}*/ else if (menu == "messageBoxMenu") {
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
