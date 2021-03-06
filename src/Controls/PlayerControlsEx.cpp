#include "Controls/PlayerControlsEx.h"
#include "Controls/InputHandlerEx.h"

namespace SkyrimSoulsRE
{
	void PlayerControlsEx::InstallHook()
	{
		InputHandlerEx<RE::ActivateHandler>::InstallHook(REL::Relocation<std::uintptr_t>(RE::Offset::ActivateHandler::Vtbl), 0x1);
		InputHandlerEx<RE::AttackBlockHandler>::InstallHook(REL::Relocation<std::uintptr_t>(RE::Offset::AttackBlockHandler::Vtbl), 0x1);
		InputHandlerEx<RE::AutoMoveHandler>::InstallHook(REL::Relocation<std::uintptr_t>(RE::Offset::AutoMoveHandler::Vtbl), 0x1);
		InputHandlerEx<RE::FirstPersonState>::InstallHook(REL::Relocation<std::uintptr_t>(RE::Offset::FirstPersonState::Vtbl), 0xB);
		InputHandlerEx<RE::JumpHandler>::InstallHook(REL::Relocation<std::uintptr_t>(RE::Offset::JumpHandler::Vtbl), 0x1);
		InputHandlerEx<RE::LookHandler>::InstallHook(REL::Relocation<std::uintptr_t>(RE::Offset::LookHandler::Vtbl), 0x1);
		InputHandlerEx<RE::MovementHandler>::InstallHook(REL::Relocation<std::uintptr_t>(RE::Offset::MovementHandler::Vtbl), 0x1);
		InputHandlerEx<RE::ReadyWeaponHandler>::InstallHook(REL::Relocation<std::uintptr_t>(RE::Offset::ReadyWeaponHandler::Vtbl), 0x1);
		InputHandlerEx<RE::RunHandler>::InstallHook(REL::Relocation<std::uintptr_t>(RE::Offset::RunHandler::Vtbl), 0x1);
		InputHandlerEx<RE::ShoutHandler>::InstallHook(REL::Relocation<std::uintptr_t>(RE::Offset::ShoutHandler::Vtbl), 0x1);
		InputHandlerEx<RE::SneakHandler>::InstallHook(REL::Relocation<std::uintptr_t>(RE::Offset::SneakHandler::Vtbl), 0x1);
		InputHandlerEx<RE::SprintHandler>::InstallHook(REL::Relocation<std::uintptr_t>(RE::Offset::SprintHandler::Vtbl), 0x1);
		InputHandlerEx<RE::ThirdPersonState>::InstallHook(REL::Relocation<std::uintptr_t>(RE::Offset::ThirdPersonState::Vtbl), 0x12);
		InputHandlerEx<RE::TogglePOVHandler>::InstallHook(REL::Relocation<std::uintptr_t>(RE::Offset::TogglePOVHandler::Vtbl), 0x1);
		InputHandlerEx<RE::ToggleRunHandler>::InstallHook(REL::Relocation<std::uintptr_t>(RE::Offset::ToggleRunHandler::Vtbl), 0x1);
	}
}
