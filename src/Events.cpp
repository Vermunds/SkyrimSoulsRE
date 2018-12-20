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
	RE::EventResult MenuOpenCloseEventHandler::ReceiveEvent(RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_dispatcher)
	{
		using RE::EventResult;
		typedef RE::IMenu::Flag Flag;

		static RE::MenuManager*		mm = RE::MenuManager::GetSingleton();
		static RE::UIStringHolder*	strHolder = RE::UIStringHolder::GetSingleton();
		static RE::UIManager*		uiManager = RE::UIManager::GetSingleton();

		if (!a_event || !IsInWhiteList(a_event->menuName)) {
			return EventResult::kContinue;
		}

		if (a_event->isOpening) {
			if (a_event->menuName == strHolder->barterMenu) {
				uiManager->AddMessage(strHolder->dialogueMenu, UIMessage::kMessage_Close, 0);
			}
		} else {
			return EventResult::kContinue;
		}

		RE::IMenu* menu = mm->GetMenu(a_event->menuName);
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
			if (mm->GetMovieView(menuName) && std::strcmp(a_exclude, menuName) != 0) {
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
