#include "Hooks.h"

#include "skse64_common/BranchTrampoline.h"  // g_trampoline
#include "skse64_common/Relocation.h"  // RelocAddr, RelocPtr
#include "skse64_common/SafeWrite.h"  // SafeWrite

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
#include "RE/TESObjectREFR.h" //RE::TESObjectREFR
#include "RE/PlayerCharacter.h" //PlayerCharacter
#include "RE/UIManager.h" //UIManager
#include "RE/FormTypes.h" //FormType::ActorCharacter

#include "Events.h"  // MenuOpenCloseEventHandler::BlockInput()
#include "Offsets.h"
#include "Settings.h" //unpausedMenus
#include "Tasks.h" //SleepWaitDelegate, SaveGameDelegate, ServeTimeDelegate

namespace Hooks
{

	HookShare::result_type _PlayerInputHandler_CanProcess(RE::PlayerInputHandler* a_this, RE::InputEvent* a_event)
	{
		using SkyrimSoulsRE::MenuOpenCloseEventHandler;
		using HookShare::result_type;

		if (MenuOpenCloseEventHandler::BlockInput()) {
			return result_type::kFalse;
		}
		else {
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

		static RE::TESObjectREFR* GetLockpickingTarget()
		{
			RE::TESObjectREFRPtr* refptr = reinterpret_cast<RE::TESObjectREFRPtr*>(Offsets::LockpickingMenu_Target.GetUIntPtr());
			return refptr->get();
		}

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

	class ContainerMenuEx
	{
	public:
		enum ContainerMode
		{
			kMode_Loot = 0,
			kMode_Steal = 1,
			kMode_Pickpocket = 2,
			kMode_FollowerTrade = 3
		};

		static ContainerMode GetContainerMode()
		{
			UInt8* mode = reinterpret_cast<UInt8*>(Offsets::ContainerMenu_Mode.GetUIntPtr());
			return static_cast<ContainerMode>(*mode);
		}

		static RE::TESObjectREFR* GetContainerRef()
		{

			UInt32* handle = reinterpret_cast<UInt32*>(Offsets::ContainerMenu_Target.GetUIntPtr());
			RE::TESObjectREFRPtr refptr = nullptr;
			if (RE::TESObjectREFR::LookupByHandle(*handle, refptr))
			{
				RE::TESObjectREFR * ref = refptr.get();
				return ref;
			};
			return nullptr;
		}
	};

	class SleepWaitMenuEx
	{
	public:

		static bool RegisterForSleepWait(RE::FxDelegateArgs * a_args) {
			if (Tasks::SleepWaitDelegate::RegisterTask(a_args))
			{
				static RE::MenuManager * mm = RE::MenuManager::GetSingleton();
				static RE::UIStringHolder * strHolder = RE::UIStringHolder::GetSingleton();
				mm->GetMenu(strHolder->sleepWaitMenu)->flags |= RE::IMenu::Flag::kPauseGame;
				mm->numPauseGame++;
				return true;
			}
			return false;
		}

		static void InstallHook()
		{
			//fix for controls not working
			SafeWrite16(Offsets::SleepWaitMenuControls_Hook.GetUIntPtr(), 0x9090);

			struct RequestSleepWait_Code : Xbyak::CodeGenerator
			{
				RequestSleepWait_Code(void * buf, UInt64 a_registerForSleepWait) : Xbyak::CodeGenerator(4096, buf)
				{
					Xbyak::Label returnAddress;
					Xbyak::Label registerForSleepWaitAddress;
					Xbyak::Label continueWait;

					sub(rsp, 0x20);
					call(ptr[rip + registerForSleepWaitAddress]);
					add(rsp, 0x20);
					cmp(al, 0x0);
					je(continueWait);
					ret();
					L(continueWait);

					//overwritten code
					mov(qword[rsp + 8], rbx);
					push(rdi);
					jmp(ptr[rip + returnAddress]);

					L(returnAddress);
					dq(Offsets::StartSleepWait_Original.GetUIntPtr() + 0x6);

					L(registerForSleepWaitAddress);
					dq(a_registerForSleepWait);
				}
			};

			void * codeBuf = g_localTrampoline.StartAlloc();
			RequestSleepWait_Code code(codeBuf, uintptr_t(RegisterForSleepWait));
			g_localTrampoline.EndAlloc(code.getCurr());

			g_branchTrampoline.Write6Branch(Offsets::StartSleepWait_Original.GetUIntPtr(), uintptr_t(code.getCode()));
		}
	};

	class MessageBoxMenuEx
	{
	public:
		static void RegisterForServeTime()
		{
			Tasks::ServeTimeDelegate::RegisterTask();
		}

		static void InstallHook() {
			//Fix for serving time in jail
			g_branchTrampoline.Write5Branch(Offsets::MessageBoxMenu_ServeTime_Hook.GetUIntPtr(), (uintptr_t)RegisterForServeTime);
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
			isInMenuMode_1 = reinterpret_cast<bool*>(Offsets::IsInMenuMode_Original.GetUIntPtr());
			isInMenuMode_2 = reinterpret_cast<bool*>(Offsets::IsInMenuMode_Original.GetUIntPtr() + 0x1);
			g_branchTrampoline.Write5Branch(Offsets::IsInMenuMode_Hook.GetUIntPtr(), (uintptr_t)IsInMenuMode);
			SafeWrite16(Offsets::IsInMenuMode_Hook.GetUIntPtr() + 0x5, 0x9090);
		}
	};
	bool * PapyrusEx::isInMenuMode_1 = nullptr;
	bool * PapyrusEx::isInMenuMode_2 = nullptr;

	class JournalMenuEx
	{
	public:
		//Saving from the Journal Menu causes the game to hang. (Quicksaves not affected)
		//As a workaround we don't allow the JournalMenu to save, instead we register the attempt and save from elsewhere later.

		static bool RegisterForSave(int saveMode, const char * name) {
			if ((saveMode == BGSSaveLoadManager::kEvent_Save)) {
				if (Tasks::SaveGameDelegate::RegisterTask(name))
				{
					return true;
				}
				return false;
			}
			//continue saving
			return false;
		}

		static void InstallHook() {
			struct SaveHook_Code : Xbyak::CodeGenerator
			{
				SaveHook_Code(void * buf, UInt64 a_registerForSave) : Xbyak::CodeGenerator(4096, buf)
				{
					Xbyak::Label returnAddress;
					Xbyak::Label continueSave;
					Xbyak::Label registerForSaveAddress;

					push(rcx);
					push(rdx);
					mov(rcx, rdx); //save mode
					mov(rdx, r9); //save name
					sub(rsp, 0x20); //parameter stack space 
					call(ptr[rip + registerForSaveAddress]);
					add(rsp, 0x20);
					pop(rdx);
					pop(rcx);
					cmp(al, 0x0); //should delay save?
					je(continueSave);
					ret(); //abort save
					L(continueSave);

					//overwritten code:
					mov(dword[rsp + 0x18], r8d);
					push(rbp);
					jmp(ptr[rip + returnAddress]);

					L(returnAddress);
					dq(Offsets::JournalMenu_Hook.GetUIntPtr() + 0x6);

					L(registerForSaveAddress);
					dq(a_registerForSave);
				}
			};

			void * codeBuf = g_localTrampoline.StartAlloc();
			SaveHook_Code code(codeBuf, uintptr_t(RegisterForSave));
			g_localTrampoline.EndAlloc(code.getCurr());

			g_branchTrampoline.Write6Branch(Offsets::JournalMenu_Hook.GetUIntPtr(), uintptr_t(code.getCode()));
		}
	};

	class AutoCloseHandler
	{
	public:
		static void CheckShouldClose()
		{
			static RE::MenuManager * mm = RE::MenuManager::GetSingleton();
			static RE::UIStringHolder * strHolder = RE::UIStringHolder::GetSingleton();
			static RE::PlayerCharacter * player = RE::PlayerCharacter::GetSingleton();
			static RE::UIManager * uiManager = RE::UIManager::GetSingleton();
			static SkyrimSoulsRE::SettingStore * settings = SkyrimSoulsRE::SettingStore::GetSingleton();

			if (mm->IsMenuOpen(strHolder->containerMenu) && settings->GetSetting("containerMenu"))
			{
				RE::TESObjectREFR * ref = ContainerMenuEx::GetContainerRef();

				if (ref) {
					if (ref->IsDisabled() || ref->IsMarkedForDeletion())
					{
						uiManager->AddMessage(strHolder->containerMenu, RE::UIMessage::Message::kClose, 0);
					}

					if (ref->Is(RE::FormType::ActorCharacter))
					{
						if (ContainerMenuEx::GetContainerMode() != ContainerMenuEx::ContainerMode::kMode_Loot && ref->IsDead(true))
						{
							uiManager->AddMessage(strHolder->containerMenu, RE::UIMessage::Message::kClose, 0);
						}
					}

					if (settings->GetSetting("autoClose"))
					{
						//sqrt((x2 - x1)^2 + (y2 - y1)^2 + (z2 - z1)^2)
						float distance = sqrt(pow(player->GetPositionX() - ref->GetPositionX(), 2) + pow(player->GetPositionY() - ref->GetPositionY(), 2) + pow(player->GetPositionZ() - ref->GetPositionZ(), 2));
						if (distance > settings->GetSetting("autoCloseDistance"))
						{
							uiManager->AddMessage(strHolder->containerMenu, RE::UIMessage::Message::kClose, 0);
						}
					}
				}
			}
			if (mm->IsMenuOpen(strHolder->lockpickingMenu) && settings->GetSetting("lockpickingMenu"))
			{
				RE::TESObjectREFR * ref = LockpickingMenuEx::GetLockpickingTarget();
				if (ref)
				{
					if (ref->IsDisabled() || ref->IsMarkedForDeletion())
					{
						uiManager->AddMessage(strHolder->lockpickingMenu, RE::UIMessage::Message::kClose, 0);
					}

					if (settings->GetSetting("autoClose"))
					{
						float distance = sqrt(pow(player->GetPositionX() - ref->GetPositionX(), 2) + pow(player->GetPositionY() - ref->GetPositionY(), 2) + pow(player->GetPositionZ() - ref->GetPositionZ(), 2));
						if (distance > settings->GetSetting("autoCloseDistance"))
						{
							uiManager->AddMessage(strHolder->lockpickingMenu, RE::UIMessage::Message::kClose, 0);
						}
					}
				}
			}
		}

		static void InstallHook()
		{
			struct DrawNextFrame_Code : Xbyak::CodeGenerator
			{
				DrawNextFrame_Code(void * buf, UInt64 a_checkShouldClose) : Xbyak::CodeGenerator(4096, buf)
				{
					Xbyak::Label checkShouldCloseAddress;

					call(ptr[rip + checkShouldCloseAddress]);
					add(rsp, 0x50);
					pop(rbx);
					ret();

					L(checkShouldCloseAddress);
					dq(a_checkShouldClose);
				}
			};

			void * codeBuf = g_localTrampoline.StartAlloc();
			DrawNextFrame_Code code(codeBuf, uintptr_t(CheckShouldClose));
			g_localTrampoline.EndAlloc(code.getCurr());

			g_branchTrampoline.Write5Branch(Offsets::DrawNextFrame_Hook.GetUIntPtr(), uintptr_t(code.getCode()));
		}
	};


	void InstallHooks(HookShare::RegisterForCanProcess_t* a_register)
	{
		using HookShare::Hook;

		a_register(Hook::kFirstPersonState, _PlayerInputHandler_CanProcess);
		a_register(Hook::kThirdPersonState, _PlayerInputHandler_CanProcess);
		a_register(Hook::kFavorites, _PlayerInputHandler_CanProcess);
		a_register(Hook::kMovement, _PlayerInputHandler_CanProcess);
		a_register(Hook::kLook, _PlayerInputHandler_CanProcess);
		a_register(Hook::kSprint, _PlayerInputHandler_CanProcess);
		a_register(Hook::kReadyWeapon, _PlayerInputHandler_CanProcess);
		a_register(Hook::kAutoMove, _PlayerInputHandler_CanProcess);
		a_register(Hook::kToggleRun, _PlayerInputHandler_CanProcess);
		a_register(Hook::kActivate, _PlayerInputHandler_CanProcess);
		a_register(Hook::kJump, _PlayerInputHandler_CanProcess);
		a_register(Hook::kShout, _PlayerInputHandler_CanProcess);
		a_register(Hook::kAttackBlock, _PlayerInputHandler_CanProcess);
		a_register(Hook::kRun, _PlayerInputHandler_CanProcess);
		a_register(Hook::kSneak, _PlayerInputHandler_CanProcess);

		SkyrimSoulsRE::SettingStore* settings = SkyrimSoulsRE::SettingStore::GetSingleton();

		if (settings->GetSetting("tweenMenu")) {
			TweenMenuEx::InstallHook();
		}
		if (settings->GetSetting("favoritesMenu")) {
			FavoritesMenuEx::InstallHook();
		}
		if (settings->GetSetting("bookMenu")) {
			BookMenuEx::InstallHook();
		}
		if (settings->GetSetting("lockpickingMenu")) {
			LockpickingMenuEx::InstallHook();
		}
		if (settings->GetSetting("journalMenu")) {
			JournalMenuEx::InstallHook();
		}
		if (settings->GetSetting("sleepWaitMenu")) {
			SleepWaitMenuEx::InstallHook();
		}
		if (settings->GetSetting("messageBoxMenu")) {
			MessageBoxMenuEx::InstallHook();
		}

		AutoCloseHandler::InstallHook();
		PapyrusEx::InstallHook();
	}
}
