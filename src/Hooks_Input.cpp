#include "Hooks_Input.h"
#include "Settings.h"
#include "Events.h"

#include "RE/Skyrim.h"
#include "xbyak/xbyak.h"
#include "Offsets.h" //todo

#include "skse64_common/Utilities.h"
#include "SKSE/API.h"


namespace SkyrimSoulsRE::Hooks
{
	namespace Input
	{
		namespace MenuControls
		{
			bool IsMappedToSameButton(UInt32 a_keyMask, RE::INPUT_DEVICE a_deviceType, RE::BSFixedString a_controlName, RE::UserEvents::INPUT_CONTEXT_ID a_context = RE::UserEvents::INPUT_CONTEXT_ID::kGameplay)
			{
				RE::ControlMap* controlMap = RE::ControlMap::GetSingleton();

				if (a_deviceType == RE::INPUT_DEVICE::kKeyboard)
				{
					UInt32 keyMask = controlMap->GetMappedKey(a_controlName, RE::INPUT_DEVICE::kKeyboard, a_context);
					return a_keyMask == keyMask;
				}
				else if (a_deviceType == RE::INPUT_DEVICE::kMouse)
				{
					UInt32 keyMask = controlMap->GetMappedKey(a_controlName, RE::INPUT_DEVICE::kMouse, a_context);
					return a_keyMask == keyMask;
				}
				return false;
			}

			RE::BSEventNotifyControl ReceiveEvent_Hook(RE::MenuControls* a_this, RE::InputEvent** a_event, RE::BSTEventSource<RE::InputEvent*>* a_source)
			{
				RE::UI* ui = RE::UI::GetSingleton();
				RE::InterfaceStrings* interfaceStrings = RE::InterfaceStrings::GetSingleton();
				RE::UserEvents* inputStrings = RE::UserEvents::GetSingleton();
				SkyrimSoulsRE::Settings* settings = SkyrimSoulsRE::Settings::GetSingleton();

				if (a_event && *a_event && !a_this->remapMode && !(ui->GameIsPaused()) && ((SkyrimSoulsRE::unpausedMenuCount) || (ui->IsMenuOpen(interfaceStrings->dialogueMenu) && settings->enableMovementDialogueMenu)))
				{
					for (RE::InputEvent* evn = *a_event; evn; evn = evn->next)
					{
						if (evn && evn->HasIDCode())
						{
							RE::ButtonEvent* buttonEvn = reinterpret_cast<RE::ButtonEvent*>(evn);
							RE::ThumbstickEvent* thumbEvn = reinterpret_cast<RE::ThumbstickEvent*>(evn);

							if (settings->enableMovementInMenus)
							{
								//Forward
								if (buttonEvn->userEvent == inputStrings->up && IsMappedToSameButton(buttonEvn->idCode, buttonEvn->device, inputStrings->forward))
								{
									buttonEvn->userEvent = inputStrings->forward;
								}
								//Back
								if (buttonEvn->userEvent == inputStrings->down && IsMappedToSameButton(buttonEvn->idCode, buttonEvn->device, inputStrings->back))
								{
									buttonEvn->userEvent = inputStrings->back;
								}
								//Left
								if (buttonEvn->userEvent == inputStrings->left && IsMappedToSameButton(buttonEvn->idCode, buttonEvn->device, inputStrings->strafeLeft))
								{
									buttonEvn->userEvent = inputStrings->strafeLeft;
								}
								//Right
								if (buttonEvn->userEvent == inputStrings->right && IsMappedToSameButton(buttonEvn->idCode, buttonEvn->device, inputStrings->strafeRight))
								{
									buttonEvn->userEvent = inputStrings->strafeRight;
								}
								//SkyUI Favorites menu fix
								if (ui->IsMenuOpen(interfaceStrings->favoritesMenu))
								{
									//Prevent SkyUI from detecting the key mask
									//Left
									if (buttonEvn->userEvent == inputStrings->strafeLeft && IsMappedToSameButton(buttonEvn->idCode, buttonEvn->device, inputStrings->strafeLeft))
									{
										buttonEvn->idCode = 0;
									}
									//Left
									if (buttonEvn->userEvent == inputStrings->strafeRight && IsMappedToSameButton(buttonEvn->idCode, buttonEvn->device, inputStrings->strafeRight))
									{
										buttonEvn->idCode = 0;
									}
									//Allow category change with LB and RB when using controllers
									if (buttonEvn->device == RE::INPUT_DEVICE::kGamepad)
									{
										if (buttonEvn->idCode == 0x100) //LB
										{
											buttonEvn->userEvent = inputStrings->left;
										}
										if (buttonEvn->idCode == 0x200) //RB
										{
											buttonEvn->userEvent = inputStrings->right;
										}
									}
								}
								//Book menu fix
								if (ui->IsMenuOpen(interfaceStrings->bookMenu))
								{
									//Left
									if (buttonEvn->userEvent == inputStrings->prevPage && IsMappedToSameButton(buttonEvn->idCode, buttonEvn->device, inputStrings->strafeLeft))
									{
										buttonEvn->userEvent = inputStrings->strafeLeft;
									}
									//Right
									if (buttonEvn->userEvent == inputStrings->nextPage && IsMappedToSameButton(buttonEvn->idCode, buttonEvn->device, inputStrings->strafeRight))
									{
										buttonEvn->userEvent = inputStrings->strafeRight;
									}
								}

								//Controllers
								if (thumbEvn->userEvent == inputStrings->leftStick)
								{
									thumbEvn->userEvent = inputStrings->move;
								}
							}

							if (settings->enableGamepadCameraMove)
							{
								//Look controls for controllers - do not allow when an item preview is maximized, so it is still possible to rotate it somehow
								if (thumbEvn->userEvent == inputStrings->rotate && !(*(reinterpret_cast<float*>(Offsets::ItemMenu_MaximizeStatus.GetUIntPtr())) == 1.0))
								{
									thumbEvn->userEvent = inputStrings->look;
								}
							}

						}
					}
				}

				RE::BSEventNotifyControl(*ReceiveEvent_Original)(RE::MenuControls*, RE::InputEvent**, RE::BSTEventSource<RE::InputEvent*>*);
				ReceiveEvent_Original = reinterpret_cast<RE::BSEventNotifyControl(*)(RE::MenuControls*, RE::InputEvent**, RE::BSTEventSource<RE::InputEvent*>*)>(Offsets::MenuControls_ReceiveEvent_Original.GetUIntPtr());;
				return ReceiveEvent_Original(a_this, a_event, a_source);
			}
		}

		namespace CameraMove
		{
			bool CameraMove_Hook(bool a_retVal)
			{
				RE::UI* ui = RE::UI::GetSingleton();
				RE::InterfaceStrings* interfaceStrings = RE::InterfaceStrings::GetSingleton();
				RE::PlayerControls* pc = RE::PlayerControls::GetSingleton();
				RE::MenuControls* mc = RE::MenuControls::GetSingleton();
				SkyrimSoulsRE::Settings* settings = SkyrimSoulsRE::Settings::GetSingleton();

				if (SkyrimSoulsRE::unpausedMenuCount && !mc->remapMode && !(ui->GameIsPaused()) && !(ui->IsMenuOpen(interfaceStrings->dialogueMenu)))
				{
					RE::NiPoint2* cursorPosition = reinterpret_cast<RE::NiPoint2*>(Offsets::CursorPosition.GetUIntPtr());
					RE::INIPrefSettingCollection* pref = RE::INIPrefSettingCollection::GetSingleton();

					UInt32 resX = pref->GetSetting("iSize W:DISPLAY")->GetUInt();
					UInt32 resY = pref->GetSetting("iSize H:DISPLAY")->GetUInt();

					float speedX = settings->cursorCameraHorizontalSpeed;
					float speedY = settings->cursorCameraVerticalSpeed;

					if (cursorPosition->x == 0)
					{
						pc->data.lookInputVec.x = -speedX;
					}
					else if (cursorPosition->x == resX)
					{
						pc->data.lookInputVec.x = speedX;
					}

					if (cursorPosition->y == 0)
					{
						pc->data.lookInputVec.y = speedY;
					}
					else if (cursorPosition->y == resY)
					{
						pc->data.lookInputVec.y = -speedY;
					}
				}
				return a_retVal;
			}
		}

		using PlayerFn = bool(*)(RE::PlayerInputHandler*, RE::InputEvent*);
		using MenuFn = bool(*)(RE::MenuEventHandler*, RE::InputEvent*);

		namespace Funcs
		{
			//MenuControls
			MenuFn favoritesHandler_original;
			MenuFn menuOpenHandler_original;

			//PlayerControls
			PlayerFn firstPersonState_original;
			PlayerFn thirdPersonState_original;
			PlayerFn movementHandler_original;
			PlayerFn lookHandler_original;
			PlayerFn sprintHandler_original;
			PlayerFn readyWeaponHandler_original;
			PlayerFn autoMoveHandler_original;
			PlayerFn toggleRunHandler_original;
			PlayerFn activateHandler_original;
			PlayerFn jumpHandler_original;
			PlayerFn shoutHandler_original;
			PlayerFn attackBlockHandler_original;
			PlayerFn runHandler_original;
			PlayerFn sneakHandler_original;
			PlayerFn togglePOVHandler_original;
		}

		namespace PlayerInputHandlers
		{
			bool FirstPersonState_CanProcess_Hook(RE::FirstPersonState* a_this, RE::InputEvent* a_event)
			{
				if (SkyrimSoulsRE::MenuOpenCloseEventHandler::BlockInput())
				{
					return false;
				}
				return Funcs::firstPersonState_original(a_this, a_event);
			}

			bool ThirdPersonState_CanProcess_Hook(RE::ThirdPersonState* a_this, RE::InputEvent* a_event)
			{
				if (SkyrimSoulsRE::MenuOpenCloseEventHandler::BlockInput())
				{
					return false;
				}
				return Funcs::thirdPersonState_original(a_this, a_event);
			}

			bool MovementHandler_CanProcess_Hook(RE::MovementHandler* a_this, RE::InputEvent* a_event)
			{
				if (SkyrimSoulsRE::MenuOpenCloseEventHandler::BlockInput())
				{
					return false;
				}
				return Funcs::movementHandler_original(a_this, a_event);
			}

			bool LookHandler_CanProcess_Hook(RE::LookHandler* a_this, RE::InputEvent* a_event)
			{
				if (SkyrimSoulsRE::MenuOpenCloseEventHandler::BlockInput())
				{
					return false;
				}
				return Funcs::lookHandler_original(a_this, a_event);
			}

			bool SprintHandler_CanProcess_Hook(RE::SprintHandler* a_this, RE::InputEvent* a_event)
			{
				if (SkyrimSoulsRE::MenuOpenCloseEventHandler::BlockInput())
				{
					return false;
				}
				return Funcs::sprintHandler_original(a_this, a_event);
			}

			bool ReadyWeaponHandler_CanProcess_Hook(RE::ReadyWeaponHandler* a_this, RE::InputEvent* a_event)
			{
				if (SkyrimSoulsRE::MenuOpenCloseEventHandler::BlockInput())
				{
					return false;
				}
				return Funcs::readyWeaponHandler_original(a_this, a_event);
			}

			bool AutoMoveHandler_CanProcess_Hook(RE::AutoMoveHandler* a_this, RE::InputEvent* a_event)
			{
				if (SkyrimSoulsRE::MenuOpenCloseEventHandler::BlockInput())
				{
					return false;
				}
				return Funcs::autoMoveHandler_original(a_this, a_event);
			}

			bool ToggleRunHandler_CanProcess_Hook(RE::ToggleRunHandler* a_this, RE::InputEvent* a_event)
			{
				if (SkyrimSoulsRE::MenuOpenCloseEventHandler::BlockInput())
				{
					return false;
				}
				return Funcs::toggleRunHandler_original(a_this, a_event);
			}

			bool ActivateHandler_CanProcess_Hook(RE::ActivateHandler* a_this, RE::InputEvent* a_event)
			{
				if (SkyrimSoulsRE::MenuOpenCloseEventHandler::BlockInput())
				{
					return false;
				}
				return Funcs::activateHandler_original(a_this, a_event);
			}

			bool JumpHandler_CanProcess_Hook(RE::JumpHandler* a_this, RE::InputEvent* a_event)
			{
				if (SkyrimSoulsRE::MenuOpenCloseEventHandler::BlockInput())
				{
					return false;
				}
				return Funcs::jumpHandler_original(a_this, a_event);
			}

			bool ShoutHandler_CanProcess_Hook(RE::ShoutHandler* a_this, RE::InputEvent* a_event)
			{
				if (SkyrimSoulsRE::MenuOpenCloseEventHandler::BlockInput())
				{
					return false;
				}
				return Funcs::shoutHandler_original(a_this, a_event);
			}

			bool AttackBlockHandler_CanProcess_Hook(RE::AttackBlockHandler* a_this, RE::InputEvent* a_event)
			{
				if (SkyrimSoulsRE::MenuOpenCloseEventHandler::BlockInput())
				{
					return false;
				}
				return Funcs::attackBlockHandler_original(a_this, a_event);
			}

			bool RunHandler_CanProcess_Hook(RE::RunHandler* a_this, RE::InputEvent* a_event)
			{
				if (SkyrimSoulsRE::MenuOpenCloseEventHandler::BlockInput())
				{
					return false;
				}
				return Funcs::runHandler_original(a_this, a_event);
			}

			bool SneakHandler_CanProcess_Hook(RE::SneakHandler* a_this, RE::InputEvent* a_event)
			{
				if (SkyrimSoulsRE::MenuOpenCloseEventHandler::BlockInput())
				{
					return false;
				}
				return Funcs::sneakHandler_original(a_this, a_event);
			}

			bool TogglePOVHandler_CanProcess_Hook(RE::TogglePOVHandler* a_this, RE::InputEvent* a_event)
			{
				if (SkyrimSoulsRE::MenuOpenCloseEventHandler::BlockInput())
				{
					return false;
				}
				return Funcs::togglePOVHandler_original(a_this, a_event);
			}
		}

		namespace MenuEventHandlers
		{
			bool MenuOpenHandler_CanProcess_Hook(RE::MenuOpenHandler* a_this, RE::InputEvent* a_event)
			{
				if (SkyrimSoulsRE::MenuOpenCloseEventHandler::BlockInput())
				{
					return false;
				}
				return Funcs::menuOpenHandler_original(a_this, a_event);
			}

			bool FavoritesHandler_CanProcess_Hook(RE::FavoritesHandler* a_this, RE::InputEvent* a_event)
			{
				if (SkyrimSoulsRE::MenuOpenCloseEventHandler::BlockInput())
				{
					return false;
				}
				return Funcs::favoritesHandler_original(a_this, a_event);
			}
		}

		void InstallHooks(bool a_enableMovementInMenus, bool a_enableGamepadCameraMove, bool a_enableCursorCameraMove)
		{
			//PlayerInputHandlers
			uintptr_t firstPersonState = RE::Offset::FirstPersonState::Vtbl.GetAddress() + (0x8 * 0x8) + 0x10 + (0x1 * 0x8);
			Funcs::firstPersonState_original = reinterpret_cast<PlayerFn>(*(reinterpret_cast<uintptr_t*>(firstPersonState)));
			SKSE::SafeWrite64(firstPersonState, (uintptr_t)PlayerInputHandlers::FirstPersonState_CanProcess_Hook);

			uintptr_t thirdPersonState = RE::Offset::ThirdPersonState::Vtbl.GetAddress() + (0xF * 0x8) + 0x10 + (0x1 * 0x8);
			Funcs::thirdPersonState_original = reinterpret_cast<PlayerFn>(*(reinterpret_cast<uintptr_t*>(thirdPersonState)));
			SKSE::SafeWrite64(thirdPersonState, (uintptr_t)PlayerInputHandlers::ThirdPersonState_CanProcess_Hook);

			uintptr_t sprintHandler = RE::Offset::SprintHandler::Vtbl.GetAddress() + (0x1 * 0x8);
			Funcs::sprintHandler_original = reinterpret_cast<PlayerFn>(*(reinterpret_cast<uintptr_t*>(sprintHandler)));
			SKSE::SafeWrite64(sprintHandler, (uintptr_t)PlayerInputHandlers::SprintHandler_CanProcess_Hook);

			uintptr_t readyWeaponHandler = RE::Offset::ReadyWeaponHandler::Vtbl.GetAddress() + (0x1 * 0x8);
			Funcs::readyWeaponHandler_original = reinterpret_cast<PlayerFn>(*(reinterpret_cast<uintptr_t*>(readyWeaponHandler)));
			SKSE::SafeWrite64(readyWeaponHandler, (uintptr_t)PlayerInputHandlers::ReadyWeaponHandler_CanProcess_Hook);

			uintptr_t autoMoveHandler = RE::Offset::AutoMoveHandler::Vtbl.GetAddress() + (0x1 * 0x8);
			Funcs::autoMoveHandler_original = reinterpret_cast<PlayerFn>(*(reinterpret_cast<uintptr_t*>(autoMoveHandler)));
			SKSE::SafeWrite64(autoMoveHandler, (uintptr_t)PlayerInputHandlers::AutoMoveHandler_CanProcess_Hook);

			uintptr_t toggleRunHandler = RE::Offset::ToggleRunHandler::Vtbl.GetAddress() + (0x1 * 0x8);
			Funcs::toggleRunHandler_original = reinterpret_cast<PlayerFn>(*(reinterpret_cast<uintptr_t*>(toggleRunHandler)));
			SKSE::SafeWrite64(toggleRunHandler, (uintptr_t)PlayerInputHandlers::ToggleRunHandler_CanProcess_Hook);

			uintptr_t activateHandler = RE::Offset::ActivateHandler::Vtbl.GetAddress() + (0x1 * 0x8);
			Funcs::activateHandler_original = reinterpret_cast<PlayerFn>(*(reinterpret_cast<uintptr_t*>(activateHandler)));
			SKSE::SafeWrite64(activateHandler, (uintptr_t)PlayerInputHandlers::ActivateHandler_CanProcess_Hook);

			uintptr_t jumpHandler = RE::Offset::JumpHandler::Vtbl.GetAddress() + (0x1 * 0x8);
			Funcs::jumpHandler_original = reinterpret_cast<PlayerFn>(*(reinterpret_cast<uintptr_t*>(jumpHandler)));
			SKSE::SafeWrite64(jumpHandler, (uintptr_t)PlayerInputHandlers::JumpHandler_CanProcess_Hook);

			uintptr_t shoutHandler = RE::Offset::ShoutHandler::Vtbl.GetAddress() + (0x1 * 0x8);
			Funcs::shoutHandler_original = reinterpret_cast<PlayerFn>(*(reinterpret_cast<uintptr_t*>(shoutHandler)));
			SKSE::SafeWrite64(shoutHandler, (uintptr_t)PlayerInputHandlers::ShoutHandler_CanProcess_Hook);

			uintptr_t attackBlockHandler = RE::Offset::AttackBlockHandler::Vtbl.GetAddress() + (0x1 * 0x8);
			Funcs::attackBlockHandler_original = reinterpret_cast<PlayerFn>(*(reinterpret_cast<uintptr_t*>(attackBlockHandler)));
			SKSE::SafeWrite64(attackBlockHandler, (uintptr_t)PlayerInputHandlers::AttackBlockHandler_CanProcess_Hook);

			uintptr_t runHandler = RE::Offset::RunHandler::Vtbl.GetAddress() + (0x1 * 0x8);
			Funcs::runHandler_original = reinterpret_cast<PlayerFn>(*(reinterpret_cast<uintptr_t*>(runHandler)));
			SKSE::SafeWrite64(runHandler, (uintptr_t)PlayerInputHandlers::RunHandler_CanProcess_Hook);

			uintptr_t sneakHandler = RE::Offset::SneakHandler::Vtbl.GetAddress() + (0x1 * 0x8);
			Funcs::sneakHandler_original = reinterpret_cast<PlayerFn>(*(reinterpret_cast<uintptr_t*>(sneakHandler)));
			SKSE::SafeWrite64(sneakHandler, (uintptr_t)PlayerInputHandlers::SneakHandler_CanProcess_Hook);

			uintptr_t togglePOVHandler = RE::Offset::TogglePOVHandler::Vtbl.GetAddress() + (0x1 * 0x8);
			Funcs::togglePOVHandler_original = reinterpret_cast<PlayerFn>(*(reinterpret_cast<uintptr_t*>(togglePOVHandler)));
			SKSE::SafeWrite64(togglePOVHandler, (uintptr_t)PlayerInputHandlers::TogglePOVHandler_CanProcess_Hook);

			if (!a_enableMovementInMenus)
			{
				uintptr_t movementHandler = RE::Offset::MovementHandler::Vtbl.GetAddress() + (0x1 * 0x8);
				Funcs::movementHandler_original = reinterpret_cast<PlayerFn>(*(reinterpret_cast<uintptr_t*>(movementHandler)));
				SKSE::SafeWrite64(movementHandler, (uintptr_t)PlayerInputHandlers::MovementHandler_CanProcess_Hook);
			}

			if (!a_enableGamepadCameraMove)
			{
				uintptr_t lookHandler = RE::Offset::LookHandler::Vtbl.GetAddress() + (0x1 * 0x8);
				Funcs::lookHandler_original = reinterpret_cast<PlayerFn>(*(reinterpret_cast<uintptr_t*>(lookHandler)));
				SKSE::SafeWrite64(lookHandler, (uintptr_t)PlayerInputHandlers::LookHandler_CanProcess_Hook);
			}

			//MenuEventHandlers
			uintptr_t favoritesHandler = RE::Offset::FavoritesHandler::Vtbl.GetAddress() + (0x1 * 0x8);
			Funcs::favoritesHandler_original = reinterpret_cast<MenuFn>(*(reinterpret_cast<uintptr_t*>(favoritesHandler)));
			SKSE::SafeWrite64(favoritesHandler, (uintptr_t)MenuEventHandlers::FavoritesHandler_CanProcess_Hook);

			uintptr_t menuOpenHandler = RE::Offset::MenuOpenHandler::Vtbl.GetAddress() + (0x1 * 0x8);
			Funcs::menuOpenHandler_original = reinterpret_cast<MenuFn>(*(reinterpret_cast<uintptr_t*>(menuOpenHandler)));
			SKSE::SafeWrite64(menuOpenHandler, (uintptr_t)MenuEventHandlers::MenuOpenHandler_CanProcess_Hook);

			//MenuControls ReceiveEvent hook
			if (a_enableMovementInMenus || a_enableGamepadCameraMove)
			{
				SKSE::SafeWrite64(Offsets::MenuControls_ReceiveEvent_Hook.GetUIntPtr(), (uintptr_t)MenuControls::ReceiveEvent_Hook);
			}

			//Camera movement hook
			if (a_enableCursorCameraMove)
			{
				struct CameraMove_Code : Xbyak::CodeGenerator
				{
					CameraMove_Code(void* buf, UInt64 a_hookAddr) : Xbyak::CodeGenerator(4096, buf)
					{
						Xbyak::Label hookAddress;

						add(rsp, 0x30);
						pop(rdi);
						mov(rcx, rax);
						jmp(ptr[rip + hookAddress]);

						L(hookAddress);
						dq(a_hookAddr);
					}
				};

				void* codeBuf = SKSE::GetTrampoline()->StartAlloc();
				CameraMove_Code code(codeBuf, uintptr_t(CameraMove::CameraMove_Hook));
				SKSE::GetTrampoline()->EndAlloc(code.getCurr());

				SKSE::GetTrampoline()->Write5Branch(Offsets::ScreenEdgeCameraMove_Hook.GetUIntPtr(), uintptr_t(code.getCode()));
			}
		}
	}
}