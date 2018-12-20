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
	template <Menu menu>
	HookShare::ReturnType _PlayerInputHandler_CanProcess(RE::PlayerInputHandler* a_this, RE::InputEvent* a_event)
	{
		using SkyrimSoulsRE::MenuOpenCloseEventHandler;
		using HookShare::ReturnType;

		if (MenuOpenCloseEventHandler::BlockInput(GetMenuName(menu))) {
			return ReturnType::kFalse;
		} else {
			return ReturnType::kContinue;
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

			if (a_event && a_event->eventType == EventType::kButton) {
				return true;
			} else {
				return (this->*orig_CanProcess)(a_event);
			}
		}


		static void InstallHook()
		{
			RelocPtr<_CanProcess_t> vtbl_CanProcess(RE::FAVORITES_MENU_VTBL + (0x8 * 0x8) + 0x10 + (0x1 * 0x8));
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


	void InstallHooks(HookShare::_RegisterForCanProcess_t* a_register)
	{
		using HookShare::Hook;

		a_register(_PlayerInputHandler_CanProcess<kMenu_None>, Hook::kFirstPersonState);
		a_register(_PlayerInputHandler_CanProcess<kMenu_None>, Hook::kThirdPersonState);
		a_register(_PlayerInputHandler_CanProcess<kMenu_Favorites>, Hook::kFavorites);
		a_register(_PlayerInputHandler_CanProcess<kMenu_None>, Hook::kMovement);
		a_register(_PlayerInputHandler_CanProcess<kMenu_None>, Hook::kLook);
		a_register(_PlayerInputHandler_CanProcess<kMenu_None>, Hook::kSprint);
		a_register(_PlayerInputHandler_CanProcess<kMenu_None>, Hook::kReadyWeapon);
		a_register(_PlayerInputHandler_CanProcess<kMenu_None>, Hook::kAutoMove);
		a_register(_PlayerInputHandler_CanProcess<kMenu_None>, Hook::kToggleRun);
		a_register(_PlayerInputHandler_CanProcess<kMenu_None>, Hook::kActivate);
		a_register(_PlayerInputHandler_CanProcess<kMenu_None>, Hook::kJump);
		a_register(_PlayerInputHandler_CanProcess<kMenu_None>, Hook::kShout);
		a_register(_PlayerInputHandler_CanProcess<kMenu_None>, Hook::kAttackBlock);
		a_register(_PlayerInputHandler_CanProcess<kMenu_None>, Hook::kRun);
		a_register(_PlayerInputHandler_CanProcess<kMenu_None>, Hook::kSneak);

		FavoritesMenuEx::InstallHook();
	}
}
