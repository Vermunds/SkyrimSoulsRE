#include "Controls/MenuControlsEx.h"
#include "Controls/InputHandlerEx.h"

namespace SkyrimSoulsRE
{
	bool MenuControlsEx::IsMappedToSameButton(std::uint32_t a_keyMask, RE::INPUT_DEVICE a_deviceType, RE::BSFixedString a_controlName, RE::UserEvents::INPUT_CONTEXT_ID a_context)
	{
		RE::ControlMap* controlMap = RE::ControlMap::GetSingleton();

		if (a_deviceType == RE::INPUT_DEVICE::kKeyboard)
		{
			std::uint32_t keyMask = controlMap->GetMappedKey(a_controlName, RE::INPUT_DEVICE::kKeyboard, a_context);
			return a_keyMask == keyMask;
		}
		else if (a_deviceType == RE::INPUT_DEVICE::kMouse)
		{
			std::uint32_t keyMask = controlMap->GetMappedKey(a_controlName, RE::INPUT_DEVICE::kMouse, a_context);
			return a_keyMask == keyMask;
		}
		return false;
	}

	RE::BSEventNotifyControl MenuControlsEx::ProcessEvent_Hook(RE::InputEvent** a_event, RE::BSTEventSource<RE::InputEvent*>* a_source)
	{
		RE::UI* ui = RE::UI::GetSingleton();
		RE::PlayerControls* pc = RE::PlayerControls::GetSingleton();
		RE::ControlMap* controlMap = RE::ControlMap::GetSingleton();
		RE::UserEvents* userEvents = RE::UserEvents::GetSingleton();
		Settings* settings = Settings::GetSingleton();

		bool dialogueMode = ui->IsMenuOpen(RE::DialogueMenu::MENU_NAME) && !settings->isUsingDME;
		bool lookControlsEnabled = pc->lookHandler->IsInputEventHandlingEnabled() && controlMap->IsLookingControlsEnabled() && !dialogueMode;
		bool movementControlsEnabled = pc->movementHandler->IsInputEventHandlingEnabled() && controlMap->IsMovementControlsEnabled() && !dialogueMode;

		if (a_event && *a_event && !this->remapMode && !ui->GameIsPaused() && !IsFullScreenMenuOpen() && GetUnpausedMenuCount())
		{
			for (RE::InputEvent* evn = *a_event; evn; evn = evn->next)
			{
				if (evn && evn->HasIDCode())
				{
					RE::IDEvent* idEvent = static_cast<RE::ButtonEvent*>(evn);

					if (settings->enableMovementInMenus)
					{
						//Forward
						if (idEvent->userEvent == userEvents->up && IsMappedToSameButton(idEvent->idCode, idEvent->device.get(), userEvents->forward))
						{
							idEvent->userEvent = movementControlsEnabled ? userEvents->forward : "";
						}
						//Back
						if (idEvent->userEvent == userEvents->down && IsMappedToSameButton(idEvent->idCode, idEvent->device.get(), userEvents->back))
						{
							idEvent->userEvent = movementControlsEnabled ? userEvents->back : "";
						}
						//Left
						if (idEvent->userEvent == userEvents->left && IsMappedToSameButton(idEvent->idCode, idEvent->device.get(), userEvents->strafeLeft))
						{
							idEvent->userEvent = movementControlsEnabled ? userEvents->strafeLeft : "";
						}
						//Right
						if (idEvent->userEvent == userEvents->right && IsMappedToSameButton(idEvent->idCode, idEvent->device.get(), userEvents->strafeRight))
						{
							idEvent->userEvent = movementControlsEnabled ? userEvents->strafeRight : "";
						}
						//SkyUI Favorites menu fix
						if (ui->IsMenuOpen(RE::FavoritesMenu::MENU_NAME))
						{
							//Prevent SkyUI from detecting the key mask
							//Left
							if (idEvent->userEvent == userEvents->strafeLeft && IsMappedToSameButton(idEvent->idCode, idEvent->device.get(), userEvents->strafeLeft))
							{
								idEvent->idCode = 0;
							}
							//Left
							if (idEvent->userEvent == userEvents->strafeRight && IsMappedToSameButton(idEvent->idCode, idEvent->device.get(), userEvents->strafeRight))
							{
								idEvent->idCode = 0;
							}
							//Allow category change with LB and RB when using controllers
							if (idEvent->device == RE::INPUT_DEVICE::kGamepad)
							{
								if (idEvent->idCode == 0x100)  //LB
								{
									idEvent->userEvent = userEvents->left;
								}
								if (idEvent->idCode == 0x200)  //RB
								{
									idEvent->userEvent = userEvents->right;
								}
							}
						}
						//Book menu fix
						if (ui->IsMenuOpen(RE::BookMenu::MENU_NAME))
						{
							//Left
							if (idEvent->userEvent == userEvents->prevPage && IsMappedToSameButton(idEvent->idCode, idEvent->device.get(), userEvents->strafeLeft))
							{
								idEvent->userEvent = movementControlsEnabled ? userEvents->strafeLeft : "";
							}
							//Right
							if (idEvent->userEvent == userEvents->nextPage && IsMappedToSameButton(idEvent->idCode, idEvent->device.get(), userEvents->strafeRight))
							{
								idEvent->userEvent = movementControlsEnabled ? userEvents->strafeRight : "";
							}
						}

						//Toggle Walk/Run
						if (settings->enableToggleRun && IsMappedToSameButton(idEvent->idCode, idEvent->device.get(), userEvents->toggleRun))
						{
							idEvent->userEvent = movementControlsEnabled ? userEvents->toggleRun : "";
						}

						//Controllers
						if (idEvent->userEvent == userEvents->leftStick)
						{
							idEvent->userEvent = movementControlsEnabled ? userEvents->move : "";
						}
					}

					if (settings->enableGamepadCameraMove)
					{
						//Look controls for controllers - do not allow when an item preview is maximized, so it is still possible to rotate it somehow
						RE::Inventory3DManager* inventory3DManager = RE::Inventory3DManager::GetSingleton();
						if (idEvent->userEvent == userEvents->rotate && inventory3DManager->zoomProgress == 0.0f)
						{
							idEvent->userEvent = lookControlsEnabled ? userEvents->look : "";
						}
					}
				}
			}
		}

		return _ProcessEvent(this, a_event, a_source);
	}

	void MenuControlsEx::InstallHook()
	{
		REL::Relocation<std::uintptr_t> vTable(RE::VTABLE_MenuControls[0]);
		_ProcessEvent = vTable.write_vfunc(0x1, &MenuControlsEx::ProcessEvent_Hook);

		InputHandlerEx<RE::FavoritesHandler>::InstallHook(REL::Relocation<std::uintptr_t>(RE::VTABLE_FavoritesHandler[0]), 0x1);
		InputHandlerEx<RE::MenuOpenHandler>::InstallHook(REL::Relocation<std::uintptr_t>(RE::VTABLE_MenuOpenHandler[0]), 0x1);
	}
}
