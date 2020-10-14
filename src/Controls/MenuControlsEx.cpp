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
		RE::UserEvents* userEvents = RE::UserEvents::GetSingleton();
		Settings* settings = Settings::GetSingleton();

		if (a_event && *a_event && !this->remapMode && !ui->GameIsPaused() && !IsFullScreenMenuOpen() && (GetUnpausedMenuCount() || (ui->IsMenuOpen(RE::DialogueMenu::MENU_NAME) && settings->enableMovementDialogueMenu)))
		{
			for (RE::InputEvent* evn = *a_event; evn; evn = evn->next)
			{
				if (evn && evn->HasIDCode())
				{
					RE::ButtonEvent* buttonEvent = static_cast<RE::ButtonEvent*>(evn);
					RE::ThumbstickEvent* thumbEvent = static_cast<RE::ThumbstickEvent*>(evn);

					if (settings->enableMovementInMenus)
					{
						//Forward
						if (buttonEvent->userEvent == userEvents->up && IsMappedToSameButton(buttonEvent->idCode, buttonEvent->device.get(), userEvents->forward))
						{
							buttonEvent->userEvent = userEvents->forward;
						}
						//Back
						if (buttonEvent->userEvent == userEvents->down && IsMappedToSameButton(buttonEvent->idCode, buttonEvent->device.get(), userEvents->back))
						{
							buttonEvent->userEvent = userEvents->back;
						}
						//Left
						if (buttonEvent->userEvent == userEvents->left && IsMappedToSameButton(buttonEvent->idCode, buttonEvent->device.get(), userEvents->strafeLeft))
						{
							buttonEvent->userEvent = userEvents->strafeLeft;
						}
						//Right
						if (buttonEvent->userEvent == userEvents->right && IsMappedToSameButton(buttonEvent->idCode, buttonEvent->device.get(), userEvents->strafeRight))
						{
							buttonEvent->userEvent = userEvents->strafeRight;
						}
						//SkyUI Favorites menu fix
						if (ui->IsMenuOpen(RE::FavoritesMenu::MENU_NAME))
						{
							//Prevent SkyUI from detecting the key mask
							//Left
							if (buttonEvent->userEvent == userEvents->strafeLeft && IsMappedToSameButton(buttonEvent->idCode, buttonEvent->device.get(), userEvents->strafeLeft))
							{
								buttonEvent->idCode = 0;
							}
							//Left
							if (buttonEvent->userEvent == userEvents->strafeRight && IsMappedToSameButton(buttonEvent->idCode, buttonEvent->device.get(), userEvents->strafeRight))
							{
								buttonEvent->idCode = 0;
							}
							//Allow category change with LB and RB when using controllers
							if (buttonEvent->device == RE::INPUT_DEVICE::kGamepad)
							{
								if (buttonEvent->idCode == 0x100) //LB
								{
									buttonEvent->userEvent = userEvents->left;
								}
								if (buttonEvent->idCode == 0x200) //RB
								{
									buttonEvent->userEvent = userEvents->right;
								}
							}
						}
						//Book menu fix
						if (ui->IsMenuOpen(RE::BookMenu::MENU_NAME))
						{
							//Left
							if (buttonEvent->userEvent == userEvents->prevPage && IsMappedToSameButton(buttonEvent->idCode, buttonEvent->device.get(), userEvents->strafeLeft))
							{
								buttonEvent->userEvent = userEvents->strafeLeft;
							}
							//Right
							if (buttonEvent->userEvent == userEvents->nextPage && IsMappedToSameButton(buttonEvent->idCode, buttonEvent->device.get(), userEvents->strafeRight))
							{
								buttonEvent->userEvent = userEvents->strafeRight;
							}
						}

						//Controllers
						if (thumbEvent->userEvent == userEvents->leftStick)
						{
							thumbEvent->userEvent = userEvents->move;
						}
					}

					if (settings->enableGamepadCameraMove)
					{
						//Look controls for controllers - do not allow when an item preview is maximized, so it is still possible to rotate it somehow
						RE::Inventory3DManager* inventory3DManager = RE::Inventory3DManager::GetSingleton();
						//if (thumbEvent->userEvent == userEvents->rotate && !(*(reinterpret_cast<float*>(Offsets::ItemMenu_MaximizeStatus.address())) == 1.0))
						//{
						//	thumbEvent->userEvent = userEvents->look;
						//}
					}
				}
			}
		}

		return _ProcessEvent(this, a_event, a_source);
	}

	void MenuControlsEx::InstallHook()
	{
		REL::Relocation<std::uintptr_t> vTable(Offsets::MenuControls::Vtbl);
		_ProcessEvent = vTable.write_vfunc(0x1, &MenuControlsEx::ProcessEvent_Hook);

		InputHandlerEx<RE::FavoritesHandler>::InstallHook(RE::Offset::FavoritesHandler::Vtbl, 0x1);
		InputHandlerEx<RE::MenuOpenHandler>::InstallHook(RE::Offset::MenuOpenHandler::Vtbl, 0x1);
	}
}