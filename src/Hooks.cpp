#include "Hooks.h"

#include "skse64_common/BranchTrampoline.h"  // g_trampoline
#include "skse64_common/Relocation.h"  // RelocAddr, RelocPtr
#include "skse64_common/SafeWrite.h"  // SafeWrite64

#include "Events.h"  // MenuOpenCloseEventHandler::BlockInput()

#include "HookShare.h"  // _RegisterHook_t

#include "RE/IMenu.h"  // IMenu
#include "RE/InputEvent.h"  // InputEvent
#include "RE/MenuEventHandler.h"  // MenuEventHandler
#include "RE/MenuManager.h"  // MenuManager
#include "RE/Offsets.h"
#include "RE/PlayerInputHandler.h"  // PlayerInputHandler
#include "RE/UISaveLoadManager.h"  // UISaveLoadManager
#include "RE/UIStringHolder.h"  // UIStringHolder

namespace Hooks
{
	RelocAddr <uintptr_t *> FavoritesMenu_Hook = 0x8781D5; //1.5.73
	RelocAddr <uintptr_t *> BookMenu_Hook = 0x85764E; //1.5.73
	RelocAddr <uintptr_t *> LockpickingMenu_Hook = 0x8963E0; //1.5.73
	RelocAddr <uintptr_t *> TweenMenu_Hook = 0x8D0C85; //1.5.73
	RelocAddr <uintptr_t *> Papyrus_Hook = 0x9B8750; //1.5.73
	RelocAddr <uintptr_t *> IsInMenuMode_original = 0x2F26B75; //1.5.73

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
			SafeWrite16(FavoritesMenu_Hook.GetUIntPtr(), 0x9090);
		}
	};

	class BookMenuEx
	{
	public:
		static void InstallHook()
		{
			//Fix for book menu not appearing
			SafeWrite16(BookMenu_Hook.GetUIntPtr(), 0x9090);
		}
	};

	class LockpickingMenuEx
	{
	public:
		static void InstallHook()
		{
			//Fix for lockpicking menu not appearing
			SafeWrite16(LockpickingMenu_Hook.GetUIntPtr(), 0x9090);
		}
	};


	class TweenMenuEx
	{
	public:
		static void InstallHook()
		{
			//Fix for camera movement
			UInt8 codes[] = { 0x90, 0x90, 0x90, 0x90, 0x90 };
			SafeWriteBuf(TweenMenu_Hook.GetUIntPtr(), codes, sizeof(codes));
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
			isInMenuMode_1 = reinterpret_cast<bool*>(IsInMenuMode_original.GetUIntPtr());
			isInMenuMode_2 = reinterpret_cast<bool*>(IsInMenuMode_original.GetUIntPtr() + 0x1);
			g_branchTrampoline.Write5Branch(Papyrus_Hook.GetUIntPtr(), (uintptr_t)IsInMenuMode);
			SafeWrite16(Papyrus_Hook.GetUIntPtr() + 0x5, 0x9090);
		}
	};
	bool * PapyrusEx::isInMenuMode_1 = nullptr;
	bool * PapyrusEx::isInMenuMode_2 = nullptr;

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
		a_register(Hook::kFavorites, _PlayerInputHandler_CanProcess<kMenu_Favorites>);
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

		FavoritesMenuEx::InstallHook();
		BookMenuEx::InstallHook();
		LockpickingMenuEx::InstallHook();
		TweenMenuEx::InstallHook();
		PapyrusEx::InstallHook();
	}
}
