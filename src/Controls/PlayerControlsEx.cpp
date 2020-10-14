#include "Controls/PlayerControlsEx.h"
#include "Controls/InputHandlerEx.h"

namespace SkyrimSoulsRE
{
	void PlayerControlsEx::InstallHook()
	{
		//PlayerInputHandlers
		InputHandlerEx<RE::FirstPersonState>::InstallHook(RE::Offset::FirstPersonState::Vtbl, 0xB);
		InputHandlerEx<RE::ThirdPersonState>::InstallHook(RE::Offset::ThirdPersonState::Vtbl, 0x12);
		InputHandlerEx<RE::SprintHandler>::InstallHook(RE::Offset::SprintHandler::Vtbl, 0x1);
		InputHandlerEx<RE::ReadyWeaponHandler>::InstallHook(RE::Offset::ReadyWeaponHandler::Vtbl, 0x1);
		InputHandlerEx<RE::AutoMoveHandler>::InstallHook(RE::Offset::AutoMoveHandler::Vtbl, 0x1);
		InputHandlerEx<RE::ToggleRunHandler>::InstallHook(RE::Offset::ToggleRunHandler::Vtbl, 0x1);
		InputHandlerEx<RE::ActivateHandler>::InstallHook(RE::Offset::ActivateHandler::Vtbl, 0x1);
		InputHandlerEx<RE::ShoutHandler>::InstallHook(RE::Offset::ShoutHandler::Vtbl, 0x1);
		InputHandlerEx<RE::AttackBlockHandler>::InstallHook(RE::Offset::AttackBlockHandler::Vtbl, 0x1);
		InputHandlerEx<RE::RunHandler>::InstallHook(RE::Offset::RunHandler::Vtbl, 0x1);
		InputHandlerEx<RE::SneakHandler>::InstallHook(RE::Offset::SneakHandler::Vtbl, 0x1);
		InputHandlerEx<RE::TogglePOVHandler>::InstallHook(RE::Offset::TogglePOVHandler::Vtbl, 0x1);

		//movemet in menus
		InputHandlerEx<RE::MovementHandler>::InstallHook(RE::Offset::MovementHandler::Vtbl, 0x1);

		//gamepadcameramove
		InputHandlerEx<RE::LookHandler>::InstallHook(RE::Offset::LookHandler::Vtbl, 0x1);
	}
}