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
	// Some other thread owns it when doing a manual saved in an unpaused menu
	// It's important to let the LeaveCriticalSection call go through, even though this might fail, otherwise the game will hang
	void Hook_Abstract_EnterCriticalSection(__int64 a_this)
	{
		TryEnterCriticalSection((LPCRITICAL_SECTION)(a_this + 0x2790));
	}


	template <Menu menu>
	HookShare::ReturnType _PlayerInputHandler_CanProcess(RE::PlayerInputHandler* a_this, RE::InputEvent* a_event)
	{
		using SkyrimSoulsRE::MenuOpenCloseEventHandler;
		using HookShare::ReturnType;

		if (MenuOpenCloseEventHandler::BlockInput(GetMenuName(menu))) {
			return ReturnType::kReturnType_False;
		} else {
			return ReturnType::kReturnType_Continue;
		}
	}


	class FavoritesMenuEx :
		public RE::IMenu,
		public RE::MenuEventHandler
	{
	public:
		typedef bool(FavoritesMenuEx::*_CanProcess_t)(RE::InputEvent* a_event);
		static _CanProcess_t orig_CanProcess;


		bool Hook_CanProcess(RE::InputEvent* a_event)
		{
			typedef RE::InputEvent::EventType EventType;

			if (a_event && a_event->eventType == EventType::kEventType_Button) {
				return true;
			} else {
				return (this->*orig_CanProcess)(a_event);
			}
		}


		static void InstallHook()
		{
			RelocPtr<_CanProcess_t> vtbl_CanProcess(RE::FAVORITES_MENU_VTBL_META + 0x60);
			orig_CanProcess = *vtbl_CanProcess;
			SafeWrite64(vtbl_CanProcess.GetUIntPtr(), GetFnAddr(&Hook_CanProcess));
		}
	};


	FavoritesMenuEx::_CanProcess_t FavoritesMenuEx::orig_CanProcess;


	const char* GetMenuName(Menu a_menu)
	{
		static RE::UIStringHolder* strHolder = RE::UIStringHolder::GetSingleton();

		switch (a_menu) {
		case kMenu_Favorites:
			return strHolder->favoritesMenu;
		case kMenu_None:
		default:
			return "";
		}
	}


	void InstallHooks(HookShare::_RegisterHook_t* a_register)
	{
		using HookShare::Hook;

		a_register(_PlayerInputHandler_CanProcess<kMenu_None>, Hook::kHook_FirstPersonState);
		a_register(_PlayerInputHandler_CanProcess<kMenu_None>, Hook::kHook_ThirdPersonState);
		a_register(_PlayerInputHandler_CanProcess<kMenu_Favorites>, Hook::kHook_Favorites);
		a_register(_PlayerInputHandler_CanProcess<kMenu_None>, Hook::kHook_Movement);
		a_register(_PlayerInputHandler_CanProcess<kMenu_None>, Hook::kHook_Look);
		a_register(_PlayerInputHandler_CanProcess<kMenu_None>, Hook::kHook_Sprint);
		a_register(_PlayerInputHandler_CanProcess<kMenu_None>, Hook::kHook_ReadyWeapon);
		a_register(_PlayerInputHandler_CanProcess<kMenu_None>, Hook::kHook_AutoMove);
		a_register(_PlayerInputHandler_CanProcess<kMenu_None>, Hook::kHook_ToggleRun);
		a_register(_PlayerInputHandler_CanProcess<kMenu_None>, Hook::kHook_Activate);
		a_register(_PlayerInputHandler_CanProcess<kMenu_None>, Hook::kHook_Jump);
		a_register(_PlayerInputHandler_CanProcess<kMenu_None>, Hook::kHook_Shout);
		a_register(_PlayerInputHandler_CanProcess<kMenu_None>, Hook::kHook_AttackBlock);
		a_register(_PlayerInputHandler_CanProcess<kMenu_None>, Hook::kHook_Run);
		a_register(_PlayerInputHandler_CanProcess<kMenu_None>, Hook::kHook_Sneak);

		RelocAddr<uintptr_t> HookTarget_Abstract_EnterCriticalSection_Enter(0x005C0900 + 0x25);
		g_branchTrampoline.Write5Call(HookTarget_Abstract_EnterCriticalSection_Enter, GetFnAddr(Hook_Abstract_EnterCriticalSection));

		FavoritesMenuEx::InstallHook();
	}
}
