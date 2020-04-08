#include "Hooks.h"

#include "SKSE/API.h"

#include "xbyak/xbyak.h"

#include "RE/Skyrim.h"

#include "Events.h"
#include "Offsets.h"
#include "Settings.h"
#include "Tasks.h"
#include "Utility.h" //strToInt

#include "Hooks_Input.h"
#include "Hooks_Papyrus.h"

#include "Hooks_BookMenu.h"
#include "Hooks_DialogueMenu.h"
#include "Hooks_FavoritesMenu.h"
#include "Hooks_LockpickingMenu.h"
#include "Hooks_InventoryMenu.h"
#include "Hooks_ContainerMenu.h"
#include "Hooks_JournalMenu.h"
#include "Hooks_TweenMenu.h"
#include "Hooks_SleepWaitMenu.h"
#include "Hooks_Console.h"
#include "Hooks_MessageBoxMenu.h"
#include "Hooks_Console.h"

#include "BGSSaveLoadManagerEx.h"

#include <thread> //std::this_thread::sleep_for
#include <chrono> //std::chrono::seconds

namespace SkyrimSoulsRE::Hooks
{
	class DrawNextFrameEx
	{
	public:
		//Initial position of player and the container
		static bool containerTooFarWhenOpened;
		static bool lockpickingTooFarWhenOpened;
		static bool bookTooFarWhenOpened;

		static float containerInitialDistance;
		static float lockpickingInitialDistance;
		static float bookInitialDistance;

		static bool containerInitiallyDisabled;
		static bool lockpickingInitiallyDisabled;
		static bool bookInitiallyDisabled;

		inline static float GetDistance(RE::NiPoint3 a_playerPos, float a_playerHeight, RE::NiPoint3 a_refPos)
		{
			//Get distance from feet and head, return the smaller
			float distanceHead = sqrt(pow(a_playerPos.x - a_refPos.x, 2) + pow(a_playerPos.y - a_refPos.y, 2) + pow((a_playerPos.z + a_playerHeight) - a_refPos.z, 2));
			float distanceFeet = sqrt(pow(a_playerPos.x - a_refPos.x, 2) + pow(a_playerPos.y - a_refPos.y, 2) + pow(a_playerPos.z - a_refPos.z, 2));
			if (distanceHead < distanceFeet) {
				return distanceHead;
			}
			return distanceFeet;
		}

		static void UpdateClock()
		{
			RE::Calendar* calendar = RE::Calendar::GetSingleton();
			RE::UI* ui = RE::UI::GetSingleton();
			RE::InterfaceStrings* interfaceStrings = RE::InterfaceStrings::GetSingleton();

			void(*GetTimeString)(RE::Calendar* a_this, char* a_str, UInt64 a_bufferSize, bool a_showYear);
			GetTimeString = reinterpret_cast<void(*)(RE::Calendar*, char*, UInt64, bool)>(Offsets::GetFormattedTime_Original.GetUIntPtr());;

			//Tween menu clock
			if (ui->IsMenuOpen(interfaceStrings->tweenMenu) && !(ui->GameIsPaused()))
			{
				RE::IMenu* tweenMenu = ui->GetMenu(interfaceStrings->tweenMenu).get();
				if (tweenMenu)
				{
					char buf[200];
					GetTimeString(calendar, buf, 200, true);

					RE::GFxValue dateText;
					tweenMenu->view->GetVariable(&dateText, "_root.TweenMenu_mc.BottomBarTweener_mc.BottomBar_mc.DateText");
					RE::GFxValue newDate(buf);
					dateText.SetMember("htmlText", newDate);
				}
			}

			//Journal Menu clock
			if (ui->IsMenuOpen(interfaceStrings->journalMenu) && !(ui->GameIsPaused()))
			{
				RE::IMenu* journalMenu = ui->GetMenu(interfaceStrings->journalMenu).get();
				if (journalMenu)
				{
					char buf[200];
					GetTimeString(calendar, buf, 200, true);

					RE::GFxValue dateText;
					journalMenu->view->GetVariable(&dateText, "_root.QuestJournalFader.Menu_mc.BottomBar_mc.DateText");
					RE::GFxValue newDate(buf);
					dateText.SetMember("htmlText", newDate);
				}
			}

			//Sleep/Wait menu clock
			if (ui->IsMenuOpen(interfaceStrings->sleepWaitMenu) && !(ui->GameIsPaused()))
			{
				RE::IMenu* sleepWaitMenu = ui->GetMenu(interfaceStrings->sleepWaitMenu).get();
				if (sleepWaitMenu)
				{
					char buf[200];
					GetTimeString(calendar, buf, 200, false);

					RE::GFxValue dateText;
					sleepWaitMenu->view->GetVariable(&dateText, "_root.SleepWaitMenu_mc.CurrentTime");
					RE::GFxValue newDate(buf);
					dateText.SetMember("htmlText", newDate);
				}
			}
		}

		static void UpdateBottomBar()
		{
			RE::UI* ui = RE::UI::GetSingleton();
			RE::InterfaceStrings* interfaceStrings = RE::InterfaceStrings::GetSingleton();

			//Inventory Menu
			if (ui->IsMenuOpen(interfaceStrings->inventoryMenu) && !(ui->GameIsPaused()))
			{
				RE::IMenu* inventoryMenu = ui->GetMenu(interfaceStrings->inventoryMenu).get();
				if (inventoryMenu)
				{
					void(*UpdateBottomBar)(RE::IMenu*);
					UpdateBottomBar = reinterpret_cast<void(*)(RE::IMenu*)>(Offsets::InventoryMenu_UpdateBottomBarInfo.GetUIntPtr());;
					UpdateBottomBar(inventoryMenu);
				}
			}

			//Magic Menu
			if (ui->IsMenuOpen(interfaceStrings->magicMenu) && !(ui->GameIsPaused()))
			{
				RE::IMenu* magicMenu = ui->GetMenu(interfaceStrings->magicMenu).get();
				if (magicMenu)
				{
					void(*UpdateBottomBar)(RE::IMenu*);
					UpdateBottomBar = reinterpret_cast<void(*)(RE::IMenu*)>(Offsets::MagicMenu_UpdateBottomBarInfo.GetUIntPtr());;
					UpdateBottomBar(magicMenu);
				}
			}

			//Container Menu
			if (ui->IsMenuOpen(interfaceStrings->containerMenu) && !(ui->GameIsPaused()))
			{
				RE::IMenu* containerMenu = ui->GetMenu(interfaceStrings->containerMenu).get();
				if (containerMenu)
				{
					void(*UpdateBottomBar)(RE::IMenu*);
					UpdateBottomBar = reinterpret_cast<void(*)(RE::IMenu*)>(Offsets::ContainerMenu_UpdateBottomBarInfo.GetUIntPtr());;
					UpdateBottomBar(containerMenu);
				}
			}
		}

		static void CheckShouldClose()
		{
			RE::UI* ui = RE::UI::GetSingleton();
			RE::InterfaceStrings* interfaceStrings = RE::InterfaceStrings::GetSingleton();
			RE::PlayerCharacter * player = RE::PlayerCharacter::GetSingleton();
			RE::UIMessageQueue* uiMessageQueue = RE::UIMessageQueue::GetSingleton();
			SkyrimSoulsRE::Settings * settings = SkyrimSoulsRE::Settings::GetSingleton();

			float autoCloseDistance = settings->autoCloseDistance;

			//Auto-close Container menu
			if (ui->IsMenuOpen(interfaceStrings->containerMenu) && settings->unpausedMenus["ContainerMenu"])
			{
				RE::TESObjectREFR * ref = ContainerMenu::GetContainerRef();

				ContainerMenu::ContainerMode mode = ContainerMenu::GetContainerMode();

				if (ref) {
					float currentDistance = GetDistance(player->GetPosition(), player->GetHeight(), ref->GetPosition());

					if (SkyrimSoulsRE::justOpenedContainer)
					{
						containerInitialDistance = currentDistance;
						containerInitiallyDisabled = ref->IsDisabled();
						SkyrimSoulsRE::justOpenedContainer = false;

						containerTooFarWhenOpened = (containerInitialDistance > autoCloseDistance) ? true : false;
					}

					if ((ref->IsDisabled() && !containerInitiallyDisabled) || ref->IsMarkedForDeletion())
					{
						uiMessageQueue->AddMessage(interfaceStrings->containerMenu, RE::UI_MESSAGE_TYPE::kHide, 0);
					}

					if (ref->Is(RE::FormType::ActorCharacter))
					{
						if (ContainerMenu::GetContainerMode() == ContainerMenu::ContainerMode::kMode_Pickpocket && ref->IsDead(true))
						{
							uiMessageQueue->AddMessage(interfaceStrings->containerMenu, RE::UI_MESSAGE_TYPE::kHide, 0);
						}
					}
					
					if (settings->autoCloseMenus) {

						if (containerTooFarWhenOpened)
						{
							//Check if the distance is increasing
							if (currentDistance > (containerInitialDistance + 50))
							{
								uiMessageQueue->AddMessage(interfaceStrings->containerMenu, RE::UI_MESSAGE_TYPE::kHide, 0);
							}
							else if ((containerInitialDistance - 50) > currentDistance) {
								//Check if it's already in range
								if (currentDistance < autoCloseDistance)
								{
									containerTooFarWhenOpened = false;
								}
							}
						}
						else
						{
							if (currentDistance > autoCloseDistance)
							{
								uiMessageQueue->AddMessage(interfaceStrings->containerMenu, RE::UI_MESSAGE_TYPE::kHide, 0);
							}
						}
					}
				}
			}

			//Auto-close Lockpicking menu
			if (ui->IsMenuOpen(interfaceStrings->lockpickingMenu) && settings->unpausedMenus["Lockpicking Menu"])
			{
				RE::TESObjectREFR * ref = LockpickingMenu::GetLockpickingTarget();
				if (ref)
				{
					float currentDistance = GetDistance(player->GetPosition(), player->GetHeight(), ref->GetPosition());

					if (SkyrimSoulsRE::justOpenedLockpicking)
					{
						lockpickingInitialDistance = currentDistance;
						lockpickingInitiallyDisabled = ref->IsDisabled();
						SkyrimSoulsRE::justOpenedLockpicking = false;

						lockpickingTooFarWhenOpened = (lockpickingInitialDistance > autoCloseDistance) ? true : false;
					}

					if ((ref->IsDisabled() && !lockpickingInitiallyDisabled) || ref->IsMarkedForDeletion())
					{
						uiMessageQueue->AddMessage(interfaceStrings->lockpickingMenu, RE::UI_MESSAGE_TYPE::kHide, 0);
					}

					if (settings->autoCloseMenus) {

						if (lockpickingTooFarWhenOpened)
						{
							//Check if the distance is increasing
							if (currentDistance > (lockpickingInitialDistance + 50))
							{
								uiMessageQueue->AddMessage(interfaceStrings->lockpickingMenu, RE::UI_MESSAGE_TYPE::kHide, 0);
							}
							else if ((lockpickingInitialDistance - 50) > currentDistance) {
								//Check if it's already in range
								if (currentDistance < autoCloseDistance)
								{
									lockpickingTooFarWhenOpened = false;
								}
							}
						}
						else
						{
							if (currentDistance > autoCloseDistance)
							{
								uiMessageQueue->AddMessage(interfaceStrings->lockpickingMenu, RE::UI_MESSAGE_TYPE::kHide, 0);
							}
						}
					}
				}
			}

			//Auto-close Book menu
			if (ui->IsMenuOpen(interfaceStrings->bookMenu) && settings->unpausedMenus["Book Menu"])
			{
				RE::TESObjectREFR* ref = BookMenu::GetBookReference();
				if (ref)
				{
					float currentDistance = GetDistance(player->GetPosition(), player->GetHeight(), ref->GetPosition());

					if (SkyrimSoulsRE::justOpenedBook)
					{
						bookInitialDistance = currentDistance;
						bookInitiallyDisabled = ref->IsDisabled();
						SkyrimSoulsRE::justOpenedBook = false;

						bookTooFarWhenOpened = (bookInitialDistance > autoCloseDistance) ? true : false;
					}

					if ((ref->IsDisabled() && !bookInitiallyDisabled) || ref->IsMarkedForDeletion())
					{
						uiMessageQueue->AddMessage(interfaceStrings->lockpickingMenu, RE::UI_MESSAGE_TYPE::kHide, 0);
					}

					if (settings->autoCloseMenus) {

						if (bookTooFarWhenOpened)
						{
							//Check if the distance is increasing
							if (currentDistance > (bookInitialDistance + 50))
							{
								uiMessageQueue->AddMessage(interfaceStrings->bookMenu, RE::UI_MESSAGE_TYPE::kHide, 0);
							}
							else if ((bookInitialDistance - 50) > currentDistance) {
								//Check if it's already in range
								if (currentDistance < autoCloseDistance)
								{
									bookTooFarWhenOpened = false;
								}
							}
						}
						else
						{
							if (currentDistance > autoCloseDistance)
							{
								uiMessageQueue->AddMessage(interfaceStrings->bookMenu, RE::UI_MESSAGE_TYPE::kHide, 0);
							}
						}
					}
				}
			}
		}

		static void DrawNextFrame_Hook()
		{
			//Slow motion
			SkyrimSoulsRE::Settings* settings = SkyrimSoulsRE::Settings::GetSingleton();

			if (settings->enableSlowMotion && SkyrimSoulsRE::isInSlowMotion)
			{
				float slowMotionMultiplier = settings->slowMotionMultiplier;

				float multiplier;
				if (slowMotionMultiplier >= 0.1 && 1.0 >= slowMotionMultiplier)
				{
					multiplier = slowMotionMultiplier;
				}
				else {
					multiplier = 1.0;
				}

				float* globalTimescale = reinterpret_cast<float*>(Offsets::GlobalTimescaleMultipler.GetUIntPtr());

				if (multiplier < *globalTimescale)
				{
					//fix for slow motion setting back the value to 1.0 after it ends
					*globalTimescale = multiplier;
				}

			}

			UpdateClock();
			UpdateBottomBar();
			CheckShouldClose();
		}

		static void InstallHook()
		{

			struct DrawNextFrame_Code : Xbyak::CodeGenerator
			{
				DrawNextFrame_Code(void * buf, UInt64 a_hookAddr) : Xbyak::CodeGenerator(4096, buf)
				{
					Xbyak::Label hookAddress;

					call(ptr[rip + hookAddress]);
					add(rsp, 0x50);
					pop(rbx);
					ret();

					L(hookAddress);
					dq(a_hookAddr);
				}
			};

			void * codeBuf = SKSE::GetTrampoline()->StartAlloc();
			DrawNextFrame_Code code(codeBuf, uintptr_t(DrawNextFrame_Hook));
			SKSE::GetTrampoline()->EndAlloc(code.getCurr());

			SKSE::GetTrampoline()->Write5Branch(Offsets::DrawNextFrame_Hook.GetUIntPtr(), uintptr_t(code.getCode()));
		}
	};
	bool DrawNextFrameEx::containerTooFarWhenOpened = false;
	bool DrawNextFrameEx::lockpickingTooFarWhenOpened = false;
	bool DrawNextFrameEx::bookTooFarWhenOpened = false;

	float DrawNextFrameEx::containerInitialDistance = 0.0;
	float DrawNextFrameEx::lockpickingInitialDistance = 0.0;
	float DrawNextFrameEx::bookInitialDistance = 0.0;

	bool DrawNextFrameEx::containerInitiallyDisabled = false;
	bool DrawNextFrameEx::lockpickingInitiallyDisabled = false;
	bool DrawNextFrameEx::bookInitiallyDisabled = false;

	void InstallHooks()
	{
		SkyrimSoulsRE::Settings* settings = SkyrimSoulsRE::Settings::GetSingleton();

		Input::InstallHooks(settings->enableMovementInMenus, settings->enableGamepadCameraMove, settings->enableCursorCameraMove);

		DialogueMenu::InstallHook();

		if (settings->unpausedMenus["TweenMenu"]) {
			TweenMenu::InstallHook();
		}
		if (settings->unpausedMenus["FavoritesMenu"]) {
			FavoritesMenu::InstallHook();
		}
		if (settings->unpausedMenus["Book Menu"]) {
			BookMenu::InstallHook();
		}
		if (settings->unpausedMenus["Lockpicking Menu"]) {
			LockpickingMenu::InstallHook();
		}
		if (settings->unpausedMenus["Sleep/Wait Menu"]) {
			SleepWaitMenu::InstallHook();
		}
		if (settings->unpausedMenus["ContainerMenu"]) {
			ContainerMenu::InstallHook();
		}
		if (settings->unpausedMenus["InventoryMenu"]) {
			InventoryMenu::InstallHook();
		}
		if (settings->unpausedMenus["Journal Menu"]) {
			JournalMenu::InstallHook();
		}

		DrawNextFrameEx::InstallHook();
		Papyrus::InstallHook();
	}
}
