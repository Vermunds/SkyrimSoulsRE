#include "Events.h"

#include <map>

#include "Settings.h"
#include "Hooks.h"
#include "Offsets.h"

#include "RE/Skyrim.h"

namespace SkyrimSoulsRE
{
	UInt32 unpausedMenuCount = 0;

	bool justOpenedContainer = false;
	bool justOpenedLockpicking = false;
	bool justOpenedBook = false;

	bool isInSlowMotion = false;

	RE::BSEventNotifyControl MenuOpenCloseEventHandler::ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_dispatcher)
	{
		using Flag = RE::IMenu::Flag;

		RE::UI*	ui = RE::UI::GetSingleton();
		RE::InterfaceStrings* interfaceStrings = RE::InterfaceStrings::GetSingleton();
		RE::UIMessageQueue*	uiMessageQueue = RE::UIMessageQueue::GetSingleton();
		SkyrimSoulsRE::Settings* settings = SkyrimSoulsRE::Settings::GetSingleton();

		RE::IMenu* menu = ui->GetMenu(a_event->menuName).get();
		if (menu && menu->menuDepth == 0 && (a_event->menuName != interfaceStrings->tweenMenu)) {
			menu->menuDepth++;
		}

		if (IsInWhiteList(a_event->menuName) && !(a_event->menuName == interfaceStrings->console)) {
			//console might cause an underflow for some reason so it's not going to be affected by this
			(a_event->opening) ? (unpausedMenuCount++) : (unpausedMenuCount--);
		}

		if (a_event->opening) {

			//Needed for auto-close
			if (a_event->menuName == interfaceStrings->containerMenu)
			{
				justOpenedContainer = true;
			}
			else if (a_event->menuName == interfaceStrings->lockpickingMenu)
			{
				justOpenedLockpicking = true;
			}
			else if (a_event->menuName == interfaceStrings->bookMenu)
			{
				justOpenedBook = true;
			}

			//Opening dialogue when an unpaused menu is open
			if ((a_event->menuName == interfaceStrings->dialogueMenu) && unpausedMenuCount) {
				//Close tween menu if open
				uiMessageQueue->AddMessage(interfaceStrings->tweenMenu, RE::UIMessage::Message::kClose, 0);

				RE::GFxMovieView* view = ui->GetMovieView(interfaceStrings->dialogueMenu).get();
				view->SetVisible(false);
				ui->GetMenu(interfaceStrings->dialogueMenu)->menuDepth = 0;
			}
			//Opening unpaused menu when a dialogue is open
			if (ui->IsMenuOpen(interfaceStrings->dialogueMenu) && unpausedMenuCount) {

				RE::GFxMovieView* view = ui->GetMovieView(interfaceStrings->dialogueMenu).get();
				view->SetVisible(false);
				ui->GetMenu(interfaceStrings->dialogueMenu)->menuDepth = 0;
			}
		}
		else {
			//Closing menu when a dialogue is open
			if (ui->IsMenuOpen(interfaceStrings->dialogueMenu) && !unpausedMenuCount) {
				RE::GFxMovieView* view = ui->GetMovieView(interfaceStrings->dialogueMenu).get();
				view->SetVisible(true);
				ui->GetMenu(interfaceStrings->dialogueMenu)->menuDepth = 3;
			}
			if (a_event->menuName == interfaceStrings->cursorMenu && settings->autoCenterCursor)
			{
				RE::NiPoint2* cursorPosition = reinterpret_cast<RE::NiPoint2*>(Offsets::CursorPosition.GetUIntPtr());
				RE::INIPrefSettingCollection* pref = RE::INIPrefSettingCollection::GetSingleton();

				cursorPosition->x = pref->GetSetting("iSize W:DISPLAY")->GetUInt() / 2.0;
				cursorPosition->y = pref->GetSetting("iSize H:DISPLAY")->GetUInt() / 2.0;
			}
		}

		if (settings->enableSlowMotion)
		{
 			float slowMotionMultiplier = settings->slowMotionMultiplier;
			float* globalTimescale = reinterpret_cast<float*>(Offsets::GlobalTimescaleMultipler.GetUIntPtr());

			float multiplier;
			if (slowMotionMultiplier >= 0.1 && 1.0 >= slowMotionMultiplier)
			{
				multiplier = slowMotionMultiplier;
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
			RE::IMenu* lootMenu = ui->GetMenu("LootMenu").get();
			if (lootMenu) {
				lootMenu->view->SetVisible(false);
			}
		}


		if (!a_event || !IsInWhiteList(a_event->menuName)) {
			return RE::BSEventNotifyControl::kContinue;
		}
	   
		if (menu) {
			if (a_event->menuName == interfaceStrings->sleepWaitMenu)
			{
				Hooks::Register_Func(menu, Hooks::HookType::kSleepWaitMenu);
			}
			else if (a_event->menuName == interfaceStrings->console)
			{
				Hooks::Register_Func(menu, Hooks::HookType::kConsole);
			}
			else if (a_event->menuName == interfaceStrings->messageBoxMenu)
			{
				Hooks::Register_Func(menu, Hooks::HookType::kMessageBoxMenu);
			} 
			else if (a_event->menuName == interfaceStrings->journalMenu)
			{
				Hooks::Register_Func(menu, Hooks::HookType::kJournalMenu);
			}
			
			if (menu->PausesGame()) {
				menu->flags &= ~Flag::kPausesGame;
				if (ui->GameIsPaused()) {
					ui->numPausesGame--;
				}
			}
			if (menu->FreezeFrameBackground()) {
				menu->flags &= ~Flag::kFreezeFrameBackground;
			}
		}
		return RE::BSEventNotifyControl::kContinue;
	}

	bool MenuOpenCloseEventHandler::BlockInput()
	{
		static RE::UI* ui = RE::UI::GetSingleton();
		static RE::InterfaceStrings* interfaceStrings = RE::InterfaceStrings::GetSingleton();

		if (unpausedMenuCount || ui->IsMenuOpen(interfaceStrings->console))
		{
			return true;
		}
		return false;
	}

	bool MenuOpenCloseEventHandler::IsInWhiteList(const RE::BSFixedString& a_name)
	{
		SkyrimSoulsRE::Settings* settings = SkyrimSoulsRE::Settings::GetSingleton();

		std::map<std::string, bool>::const_iterator it = settings->unpausedMenus.find(a_name.c_str());
		if (it == settings->unpausedMenus.end()) {
			return false;
		}
		else {
			return it->second;
		}
	}

	MenuOpenCloseEventHandler g_menuOpenCloseEventHandler;

}
