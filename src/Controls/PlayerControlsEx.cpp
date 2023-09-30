#include "Controls/PlayerControlsEx.h"
#include "Controls/InputHandlerEx.h"

namespace SkyrimSoulsRE
{
	void PlayerControlsEx::InstallHook()
	{
		InputHandlerEx<RE::ActivateHandler>::InstallHook(REL::Relocation<std::uintptr_t>(RE::VTABLE_ActivateHandler[0]), 0x1);
		InputHandlerEx<RE::AttackBlockHandler>::InstallHook(REL::Relocation<std::uintptr_t>(RE::VTABLE_AttackBlockHandler[0]), 0x1);
		InputHandlerEx<RE::AutoMoveHandler>::InstallHook(REL::Relocation<std::uintptr_t>(RE::VTABLE_AutoMoveHandler[0]), 0x1);
		InputHandlerEx<RE::FirstPersonState>::InstallHook(REL::Relocation<std::uintptr_t>(RE::VTABLE_FirstPersonState[0]), 0xB);
		InputHandlerEx<RE::JumpHandler>::InstallHook(REL::Relocation<std::uintptr_t>(RE::VTABLE_JumpHandler[0]), 0x1);
		InputHandlerEx<RE::LookHandler>::InstallHook(REL::Relocation<std::uintptr_t>(RE::VTABLE_LookHandler[0]), 0x1);
		InputHandlerEx<RE::MovementHandler>::InstallHook(REL::Relocation<std::uintptr_t>(RE::VTABLE_MovementHandler[0]), 0x1);
		InputHandlerEx<RE::ReadyWeaponHandler>::InstallHook(REL::Relocation<std::uintptr_t>(RE::VTABLE_ReadyWeaponHandler[0]), 0x1);
		InputHandlerEx<RE::RunHandler>::InstallHook(REL::Relocation<std::uintptr_t>(RE::VTABLE_RunHandler[0]), 0x1);
		InputHandlerEx<RE::ShoutHandler>::InstallHook(REL::Relocation<std::uintptr_t>(RE::VTABLE_ShoutHandler[0]), 0x1);
		InputHandlerEx<RE::SneakHandler>::InstallHook(REL::Relocation<std::uintptr_t>(RE::VTABLE_SneakHandler[0]), 0x1);
		InputHandlerEx<RE::SprintHandler>::InstallHook(REL::Relocation<std::uintptr_t>(RE::VTABLE_SprintHandler[0]), 0x1);
		InputHandlerEx<RE::ThirdPersonState>::InstallHook(REL::Relocation<std::uintptr_t>(RE::VTABLE_ThirdPersonState[0]), 0x12);
		InputHandlerEx<RE::TogglePOVHandler>::InstallHook(REL::Relocation<std::uintptr_t>(RE::VTABLE_TogglePOVHandler[0]), 0x1);
		InputHandlerEx<RE::ToggleRunHandler>::InstallHook(REL::Relocation<std::uintptr_t>(RE::VTABLE_ToggleRunHandler[0]), 0x1);
	}
}
