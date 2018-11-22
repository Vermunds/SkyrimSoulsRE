#include "Hooks.h"

#include "skse64_common/BranchTrampoline.h"  // g_trampoline
#include "skse64/GameInput.h"  // InputEvent

#include "Events.h"  // MenuOpenCloseEventHandler::BlockInput()

#include "HookShare.h"  // _RegisterHook_t

#include "RE/BGSSaveLoadManager.h"  // BGSSaveLoadManager
#include "RE/PlayerInputHandler.h"  // PlayerInputHandler


namespace Hooks
{
	HookShare::ReturnType _PlayerInputHandler_CanProcess(RE::PlayerInputHandler* a_this, InputEvent* a_event)
	{
		using SkyrimSoulsRE::MenuOpenCloseEventHandler;
		using HookShare::ReturnType;

		if (MenuOpenCloseEventHandler::BlockInput()) {
			return ReturnType::kReturnType_False;
		} else {
			return ReturnType::kReturnType_Continue;
		}
	}


	void InstallHooks(HookShare::_RegisterHook_t* a_register)
	{
		using HookShare::Hook;

		a_register(_PlayerInputHandler_CanProcess, Hook::kHook_FirstPersonState);
		a_register(_PlayerInputHandler_CanProcess, Hook::kHook_ThirdPersonState);
		a_register(_PlayerInputHandler_CanProcess, Hook::kHook_Favorites);
		a_register(_PlayerInputHandler_CanProcess, Hook::kHook_Movement);
		a_register(_PlayerInputHandler_CanProcess, Hook::kHook_Look);
		a_register(_PlayerInputHandler_CanProcess, Hook::kHook_Sprint);
		a_register(_PlayerInputHandler_CanProcess, Hook::kHook_ReadyWeapon);
		a_register(_PlayerInputHandler_CanProcess, Hook::kHook_AutoMove);
		a_register(_PlayerInputHandler_CanProcess, Hook::kHook_ToggleRun);
		a_register(_PlayerInputHandler_CanProcess, Hook::kHook_Activate);
		a_register(_PlayerInputHandler_CanProcess, Hook::kHook_Jump);
		a_register(_PlayerInputHandler_CanProcess, Hook::kHook_Shout);
		a_register(_PlayerInputHandler_CanProcess, Hook::kHook_AttackBlock);
		a_register(_PlayerInputHandler_CanProcess, Hook::kHook_Run);
		a_register(_PlayerInputHandler_CanProcess, Hook::kHook_Sneak);
	}
}
