#include "Events.h"

#include "skse64/GameEvents.h"  // EventResult, MenuOpenCloseEvent, EventDispatcher
#include "skse64/GameTypes.h"  // BSFixedString

#include <vector>  // vector

#include "Settings.h"

#include "RE/IMenu.h"  // IMenu
#include "RE/MenuManager.h"  // MenuManager
#include "RE/UIManager.h"  // UIManager


namespace SkyrimSoulsRE
{
	EventResult MenuOpenCloseEventHandler::ReceiveEvent(MenuOpenCloseEvent* a_event, EventDispatcher<MenuOpenCloseEvent>* a_dispatcher)
	{
		typedef RE::IMenu::Flag Flag;

		static RE::MenuManager*	mm = RE::MenuManager::GetSingleton();
		static UIStringHolder*	strHolder = UIStringHolder::GetSingleton();
		static RE::UIManager*	uiManager = RE::UIManager::GetSingleton();

		if (!a_event || !IsInWhiteList(a_event->menuName)) {
			return kEvent_Continue;
		}

		if (a_event->opening) {
			++_numPauseGame;
		} else {
			--_numPauseGame;
			return kEvent_Continue;
		}

		RE::IMenu* menu = mm->GetMenu(a_event->menuName);
		if (menu) {
			if (menu->PausesGame()) {
				menu->flags = Flag(menu->flags & ~Flag::kFlag_PauseGame);
				if (mm->GameIsPaused()) {
					mm->numPauseGame--;
				}
			}
			if (menu->StopsDrawingWorld()) {
				menu->flags = Flag(menu->flags & ~Flag::kFlag_StopDrawingWorld);
			}
		}

		if (a_event->menuName == strHolder->barterMenu) {
			uiManager->AddMessage(strHolder->dialogueMenu, UIMessage::kMessage_Close, 0);
		}

		return kEvent_Continue;
	}


	bool MenuOpenCloseEventHandler::BlockInput()
	{
		return _numPauseGame > 0;
	}


	void MenuOpenCloseEventHandler::Init()
	{
		static UIStringHolder* strHolder = UIStringHolder::GetSingleton();

		bool unpauseTween = false;

		for (auto& menu : Settings::unpausedMenus) {
			if (menu == "inventoryMenu") {
				_whiteList.emplace_back(strHolder->inventoryMenu);
				unpauseTween = true;
			} else if (menu == "magicMenu") {
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


	bool MenuOpenCloseEventHandler::IsInWhiteList(BSFixedString& a_name)
	{
		for (auto& menu : _whiteList) {
			if (menu == a_name) {
				return true;
			}
		}
		return false;
	}


	std::vector<BSFixedString>	MenuOpenCloseEventHandler::_whiteList;
	UInt32						MenuOpenCloseEventHandler::_numPauseGame = 0;
	UInt32						MenuOpenCloseEventHandler::_numPauseGameBuffer = 0;
	bool						MenuOpenCloseEventHandler::_cleanUp = false;


	MenuOpenCloseEventHandler g_menuOpenCloseEventHandler;
}
