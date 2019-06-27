#include "Hooks.h"

#include "skse64_common/BranchTrampoline.h"  // g_trampoline
#include "skse64_common/Relocation.h"  // RelocAddr, RelocPtr
#include "skse64_common/SafeWrite.h"  // SafeWrite64

#include "skse64/GameData.h" //BGSSaveLoadManager

#include "xbyak/xbyak.h"

#include "HookShare.h"  // _RegisterHook_t

#include "RE/IMenu.h"  // IMenu
#include "RE/InputEvent.h"  // InputEvent
#include "RE/MenuEventHandler.h"  // MenuEventHandler
#include "RE/MenuManager.h"  // MenuManager
#include "RE/Offsets.h"
#include "RE/PlayerInputHandler.h"  // PlayerInputHandler
#include "RE/UISaveLoadManager.h"  // UISaveLoadManager
#include "RE/UIStringHolder.h"  // UIStringHolder
#include "RE/FxDelegateArgs.h" //FxDelegateArgs
#include "RE/GFxValue.h" //GFxValue

#include "Events.h"  // MenuOpenCloseEventHandler::BlockInput()
#include "Offsets.h"
#include "Settings.h" //unpausedMenus

namespace SkyrimSoulsRE
{
	class EventDelayer
	{

		static EventDelayer* singleton;
		const char * saveName = nullptr;
		RE::FxDelegateArgs * sleepWaitArgs;
		UInt32 sleepWaitTime;

		void(*RequestSleepWait_Callback)(RE::FxDelegateArgs*);

	public:
		bool saveRequested = false;
		bool sleepWaitRequested = false;

		static EventDelayer* GetSingleton()
		{
			if (singleton) {
				return singleton;
			}
			singleton = new EventDelayer();
			return singleton;
		}

		void RequestSleepWait(RE::FxDelegateArgs * p_args) {
			sleepWaitRequested = true;

			this->sleepWaitTime = static_cast<UInt32>((p_args->operator[](0)).GetNumber());

			RE::GFxValue responseID;
			responseID.SetNumber(0);
			RE::GFxValue time;
			time.SetNumber(this->sleepWaitTime);

			RE::FxDelegateArgs* args = new RE::FxDelegateArgs(responseID, p_args->GetHandler(), p_args->GetMovie(), &time, 1);
			this->sleepWaitArgs = args;
		}



		void StartSleepWait() {
			RequestSleepWait_Callback = reinterpret_cast<void(*)(RE::FxDelegateArgs*)>(Offsets::SleepWaitMenuStart_Hook.GetUIntPtr());
			RequestSleepWait_Callback(this->sleepWaitArgs);
			delete(sleepWaitArgs);
		}

		void RequestSave(const char * name) {
			saveRequested = true;

			if (name) {
				const size_t len = strlen(name);
				char * tmp = new char[len + 1];
				strncpy(tmp, name, len);
				tmp[len] = '\0';
				saveName = tmp;
			} else {
				saveName = nullptr;
			}
		}

		void Save() {
			BGSSaveLoadManager* slm = BGSSaveLoadManager::GetSingleton();
			slm->Save(saveName);
		}
	};
	EventDelayer* EventDelayer::singleton = nullptr;
}

namespace Hooks
{

	template <Menu menu>
	HookShare::result_type _PlayerInputHandler_CanProcess(RE::PlayerInputHandler* a_this, RE::InputEvent* a_event)
	{
		using SkyrimSoulsRE::MenuOpenCloseEventHandler;
		using HookShare::result_type;

		if (MenuOpenCloseEventHandler::BlockInput(GetMenuName(menu))) {
			return result_type::kFalse;
		} else {
			return result_type::kContinue;
		}
	}

	class FavoritesMenuEx
	{
	public:
		static void InstallHook()
		{
			//Fix for hotkeys
			SafeWrite16(Offsets::FavoritesMenu_Hook.GetUIntPtr(), 0x9090);
		}
	};

	class BookMenuEx
	{
	public:
		static void InstallHook()
		{
			//Fix for book menu not appearing
			SafeWrite16(Offsets::BookMenu_Hook.GetUIntPtr(), 0x9090);
		}
	};

	class LockpickingMenuEx
	{
	public:
		static void InstallHook()
		{
			//Fix for lockpicking menu not appearing
			SafeWrite16(Offsets::LockpickingMenu_Hook.GetUIntPtr(), 0x9090);
		}
	};


	class TweenMenuEx
	{
	public:
		static void InstallHook()
		{
			//Fix for camera movement
			UInt8 codes[] = { 0x90, 0x90, 0x90, 0x90, 0x90 };
			SafeWriteBuf(Offsets::TweenMenu_Hook.GetUIntPtr(), codes, sizeof(codes));
		}
	};

	class SleepWaitMenuEx
	{
	public:

		static bool delaySleepWait(RE::FxDelegateArgs * a_args) {
			SkyrimSoulsRE::EventDelayer * eventDelayer = SkyrimSoulsRE::EventDelayer::GetSingleton();
			if (eventDelayer->sleepWaitRequested) {
				eventDelayer->sleepWaitRequested = false;
				//Can wait now
				return true;
			}
			RE::MenuManager * mm = RE::MenuManager::GetSingleton();
			RE::UIStringHolder * strHolder = RE::UIStringHolder::GetSingleton();
			mm->GetMenu(strHolder->sleepWaitMenu)->flags |= RE::IMenu::Flag::kPauseGame;
			mm->numPauseGame++;
			eventDelayer->RequestSleepWait(a_args);
			return false;
		}

		static void InstallHook()
		{
			//fix for controls not working
			SafeWrite16(Offsets::SleepWaitMenuControls_Hook.GetUIntPtr(), 0x9090);

			struct RequestSleepWait_Code : Xbyak::CodeGenerator
			{
				RequestSleepWait_Code(void * buf, UInt64 testAddress) : Xbyak::CodeGenerator(4096, buf)
				{
					Xbyak::Label returnAddress;
					Xbyak::Label delaySleepWaitAddress;
					Xbyak::Label continueWait;

					push(rax);
					push(rcx);
					push(rdx);
					push(r8);
					push(r9);
					push(r10);
					push(r11);
					call(ptr[rip + delaySleepWaitAddress]);
					pop(r11);
					pop(r10);
					pop(r9);
					pop(r8);
					pop(rdx);
					pop(rcx);
					cmp(al, 0x1);
					je(continueWait);
					pop(rax);
					ret();
					L(continueWait);
					pop(rax);

					//overwritten code
					mov(qword[rsp + 8], rbx);
					push(rdi);
					jmp(ptr[rip + returnAddress]);

					L(returnAddress);
					dq(Offsets::SleepWaitMenuStart_Hook.GetUIntPtr() + 0x6);

					L(delaySleepWaitAddress);
					dq(testAddress);
				}
			};

			void * codeBuf = g_localTrampoline.StartAlloc();
			RequestSleepWait_Code code(codeBuf, uintptr_t(delaySleepWait));
			g_localTrampoline.EndAlloc(code.getCurr());

			g_branchTrampoline.Write6Branch(Offsets::SleepWaitMenuStart_Hook.GetUIntPtr(), uintptr_t(code.getCode()));
		}
	};

	class PapyrusEx
	{
	public:
		static bool * isInMenuMode_1;
		static bool * isInMenuMode_2;

		static bool IsInMenuMode()
		{
			if (*isInMenuMode_1 || *isInMenuMode_2 || SkyrimSoulsRE::unpausedMenuCount) {
				return true;
			}
			return false;
		}

		static void InstallHook() {
			//IsInMenuMode hook
			isInMenuMode_1 = reinterpret_cast<bool*>(Offsets::IsInMenuMode_Hook.GetUIntPtr());
			isInMenuMode_2 = reinterpret_cast<bool*>(Offsets::IsInMenuMode_Hook.GetUIntPtr() + 0x1);
			g_branchTrampoline.Write5Branch(Offsets::Papyrus_Hook.GetUIntPtr(), (uintptr_t)IsInMenuMode);
			SafeWrite16(Offsets::Papyrus_Hook.GetUIntPtr() + 0x5, 0x9090);
		}
	};
	bool * PapyrusEx::isInMenuMode_1 = nullptr;
	bool * PapyrusEx::isInMenuMode_2 = nullptr;

	class JournalMenuEx
	{
	public:
		//Saving from the Journal Menu causes the game to hang. (Quicksaves not affected)
		//As a workaround we don't allow the JournalMenu to save, instead we register the attempt and save from elsewhere later.

		static bool delaySave(int saveMode, const char * name) {
			SkyrimSoulsRE::EventDelayer* eventDelayer = SkyrimSoulsRE::EventDelayer::GetSingleton();
			if ((saveMode == BGSSaveLoadManager::kEvent_Save)) {
				if (eventDelayer->saveRequested) {
					eventDelayer->saveRequested = false;
					//Saving was delayed, can save now
					return true;
				}
				//Delay saving
				eventDelayer->RequestSave(name);
				return false;
			}
			//continue saving
			return true;
		}

		static void InstallHook() {
			struct SaveHook_Code : Xbyak::CodeGenerator
			{
				SaveHook_Code(void * buf, UInt64 delaySaveAddress) : Xbyak::CodeGenerator(4096, buf)
				{
					Xbyak::Label returnAddress;
					Xbyak::Label continueSave;
					Xbyak::Label delaySaveLabel;

					push(rax);
					push(rcx);
					push(rdx);
					mov(rcx, rdx); //save mode
					mov(rdx, r9); //save name
					sub(rsp, 0x20); //parameter stack space 
					call(ptr[rip + delaySaveLabel]);
					add(rsp, 0x20);
					cmp(al, 0x1); //should delay save?
					je(continueSave);
					pop(rdx);
					pop(rcx);
					pop(rax);
					ret(); //abort save
					L(continueSave);
					pop(rdx);
					pop(rcx);
					pop(rax);

					//overwritten code:
					mov(dword[rsp + 0x18], r8d);
					push(rbp);
					jmp(ptr[rip + returnAddress]);

					L(returnAddress);
					dq(Offsets::JournalMenu_Hook.GetUIntPtr() + 0x6);

					L(delaySaveLabel);
					dq(delaySaveAddress);
				}
			};

			void * codeBuf = g_localTrampoline.StartAlloc();
			SaveHook_Code code(codeBuf, uintptr_t(delaySave));
			g_localTrampoline.EndAlloc(code.getCurr());

			g_branchTrampoline.Write6Branch(Offsets::JournalMenu_Hook.GetUIntPtr(), uintptr_t(code.getCode()));
		}
	};

	class EventDelayerEx
	{
	public:

		//This function runs in every frame
		static void statusCheck() {
			SkyrimSoulsRE::EventDelayer* eventDelayer = SkyrimSoulsRE::EventDelayer::GetSingleton();
			if (eventDelayer->saveRequested) {
				eventDelayer->Save();
			}
			if (eventDelayer->sleepWaitRequested) {
				eventDelayer->StartSleepWait();
			}
		}

		static void InstallHook() {
			struct SaveHook_Code : Xbyak::CodeGenerator
			{
				SaveHook_Code(void * buf, UInt64 statusCheckAddress) : Xbyak::CodeGenerator(4096, buf)
				{
					Xbyak::Label statusCheckLabel;
					Xbyak::Label returnAddress;

					push(rax);
					push(rcx);
					push(rdx);
					push(r8);
					push(r9);
					push(r10);
					push(r11);
					call(ptr[rip + statusCheckLabel]);
					pop(r11);
					pop(r10);
					pop(r9);
					pop(r8);
					pop(rdx);
					pop(rcx);
					pop(rax);

					//overwritten code:
					push(rbp);
					push(rsi);
					push(rdi);
					push(r12);
					jmp(ptr[rip + returnAddress]);

					L(returnAddress);
					dq(Offsets::EventDelayer_Hook.GetUIntPtr() + 0x6);

					L(statusCheckLabel);
					dq(statusCheckAddress);
				}
			};

			void * codeBuf = g_localTrampoline.StartAlloc();
			SaveHook_Code code(codeBuf, uintptr_t(statusCheck));
			g_localTrampoline.EndAlloc(code.getCurr());

			g_branchTrampoline.Write6Branch(Offsets::EventDelayer_Hook.GetUIntPtr(), uintptr_t(code.getCode()));
		}
	};

	const char* GetMenuName(Menu a_menu)
	{
		static RE::UIStringHolder* strHolder = RE::UIStringHolder::GetSingleton();

		switch (a_menu) {
		case kMenu_Favorites:
			return strHolder->favoritesMenu.c_str();
		case kMenu_None:
		default:
			return "";
		}
	}

	void InstallHooks(HookShare::RegisterForCanProcess_t* a_register)
	{
		using HookShare::Hook;

		a_register(Hook::kFirstPersonState, _PlayerInputHandler_CanProcess<kMenu_None>);
		a_register(Hook::kThirdPersonState, _PlayerInputHandler_CanProcess<kMenu_None>);
		a_register(Hook::kFavorites, _PlayerInputHandler_CanProcess<kMenu_None>);
		a_register(Hook::kMovement, _PlayerInputHandler_CanProcess<kMenu_None>);
		a_register(Hook::kLook, _PlayerInputHandler_CanProcess<kMenu_None>);
		a_register(Hook::kSprint, _PlayerInputHandler_CanProcess<kMenu_None>);
		a_register(Hook::kReadyWeapon, _PlayerInputHandler_CanProcess<kMenu_None>);
		a_register(Hook::kAutoMove, _PlayerInputHandler_CanProcess<kMenu_None>);
		a_register(Hook::kToggleRun, _PlayerInputHandler_CanProcess<kMenu_None>);
		a_register(Hook::kActivate, _PlayerInputHandler_CanProcess<kMenu_None>);
		a_register(Hook::kJump, _PlayerInputHandler_CanProcess<kMenu_None>);
		a_register(Hook::kShout, _PlayerInputHandler_CanProcess<kMenu_None>);
		a_register(Hook::kAttackBlock, _PlayerInputHandler_CanProcess<kMenu_None>);
		a_register(Hook::kRun, _PlayerInputHandler_CanProcess<kMenu_None>);
		a_register(Hook::kSneak, _PlayerInputHandler_CanProcess<kMenu_None>);

		for (auto& menu : SkyrimSoulsRE::Settings::unpausedMenus) {
			if (menu == "tweenMenu") {
				TweenMenuEx::InstallHook();
			} else if (menu == "favoritesMenu") {
				FavoritesMenuEx::InstallHook();
			} else if (menu == "bookMenu") {
				BookMenuEx::InstallHook();
			} else if (menu == "lockpickingMenu") {
				LockpickingMenuEx::InstallHook();
			} else if (menu == "journalMenu") {
				JournalMenuEx::InstallHook();
			} else if (menu == "sleepWaitMenu") {
				SleepWaitMenuEx::InstallHook();
			}
		}

		PapyrusEx::InstallHook();
		EventDelayerEx::InstallHook();
	}
}
