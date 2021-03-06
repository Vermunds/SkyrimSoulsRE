#include "Menus/JournalMenuEx.h"

namespace SkyrimSoulsRE
{
	std::uint32_t GamepadMaskToKeycode(std::uint32_t a_keyMask)
	{
		using Key = RE::BSWin32GamepadDevice::Key;

		switch (a_keyMask)
		{
			case Key::kUp:				return 266;
			case Key::kDown:			return 267;
			case Key::kLeft:			return 268;
			case Key::kRight:			return 269;
			case Key::kStart:			return 270;
			case Key::kBack:			return 271;
			case Key::kLeftThumb:		return 272;
			case Key::kRightThumb:		return 273;
			case Key::kLeftShoulder:	return 274;
			case Key::kRightShoulder:	return 275;
			case Key::kA:				return 276;
			case Key::kB:				return 277;
			case Key::kX:				return 278;
			case Key::kY:				return 279;
			case Key::kLeftTrigger:		return 280;
			case Key::kRightTrigger:	return 281;
			default:					return 282; // Invalid
		}
	}

	void JournalMenuEx::AdvanceMovie_Hook(float a_interval, std::uint32_t a_currentTime)
	{
		HUDMenuEx* hudMenu = static_cast<HUDMenuEx*>(RE::UI::GetSingleton()->GetMenu(RE::HUDMenu::MENU_NAME).get());
		if (hudMenu)
		{
			hudMenu->UpdateHUD();
		}
		this->UpdateClock();
		return _AdvanceMovie(this, a_interval, a_currentTime);
	}

	RE::UI_MESSAGE_RESULTS JournalMenuEx::ProcessMessage_Hook(RE::UIMessage& a_message)
	{
		if (a_message.type != RE::UI_MESSAGE_TYPE::kScaleformEvent)
		{
			return _ProcessMessage(this, a_message);
		}
		
		RE::BSUIScaleformData* data = static_cast<RE::BSUIScaleformData*>(a_message.data);

		if (JournalMenuEx::isSaving && data->scaleformEvent->type != RE::GFxEvent::EventType::kMouseMove)
		{
			//Block all input when saving, so the menu can't get closed, but let the cursor move around so users don't freak out
			return RE::UI_MESSAGE_RESULTS::kIgnore;
		}

		return _ProcessMessage(this, a_message);
	}

	void JournalMenuEx::UpdateClock()
	{
		char timeDateString[200];
		RE::Calendar::GetSingleton()->GetTimeDateString(timeDateString, 200, true);

		RE::GFxValue dateText;
		this->uiMovie->GetVariable(&dateText, "_root.QuestJournalFader.Menu_mc.BottomBar_mc.DateText");
		RE::GFxValue newDate(timeDateString);
		dateText.SetMember("htmlText", newDate);
	}

	RE::IMenu* JournalMenuEx::Creator()
	{
		class MCMRemapHandler : public RE::GFxFunctionHandler, public RE::BSTEventSink<RE::InputEvent*>, public UnpausedTask
		{
		private:
			RE::GFxValue	scope;
			std::uint32_t	keyCode;
		public:
			RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>* a_eventSource) override
			{
				RE::ButtonEvent* evn = (RE::ButtonEvent*) * a_event;

				if (!evn || evn->eventType != RE::INPUT_EVENT_TYPE::kButton)
					return RE::BSEventNotifyControl::kContinue;

				RE::INPUT_DEVICE deviceType = evn->device.get();

				RE::BSInputDeviceManager* idm = static_cast<RE::BSInputDeviceManager*>(a_eventSource);

				if ((idm->IsGamepadEnabled() ^ (deviceType == RE::INPUT_DEVICE::kGamepad)) || evn->value == 0 || evn->heldDownSecs != 0.0)
				{
					return RE::BSEventNotifyControl::kContinue;
				}

				a_eventSource->RemoveEventSink(this);

				std::uint32_t keyMask = evn->idCode;
				std::uint32_t keyCode;

				// Mouse
				switch (deviceType)
				{
				case RE::INPUT_DEVICE::kMouse:
					keyCode = keyMask + 256;
					break;
				case RE::INPUT_DEVICE::kGamepad:
					keyCode = GamepadMaskToKeycode(keyMask);
					break;
				default:
					keyCode = keyMask;
				}

				// Valid scan code?
				if (keyCode >= 282)
				{
					keyCode = -1;
				}	

				this->keyCode = keyCode;

				UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
				queue->AddTask(this);

				RE::MenuControls::GetSingleton()->remapMode = false;
				RE::PlayerControls::GetSingleton()->data.remapMode = false;

				return RE::BSEventNotifyControl::kContinue;
			}

			void Call(RE::GFxFunctionHandler::Params& a_args) override
			{
				scope = a_args.args[0];

				RE::PlayerControls* playerControls = RE::PlayerControls::GetSingleton();
				RE::MenuControls* menuControls = RE::MenuControls::GetSingleton();
				RE::BSInputDeviceManager* inputDeviceManager = RE::BSInputDeviceManager::GetSingleton();

				inputDeviceManager->AddEventSink(this);
				menuControls->remapMode = true;
				playerControls->data.remapMode = true;
			}

			void Run() override
			{
				RE::GFxValue arg;
				arg.SetNumber(this->keyCode);
				scope.Invoke("EndRemapMode", nullptr, &arg, 1);
			}

			void Dispose() override {}
		};

		class SaveGameHandler : public RE::GFxFunctionHandler, public UnpausedTask
		{
		private:
			double selectedIndex;
		public:
			void Call(Params& params) override
			{
				RE::UI* ui = RE::UI::GetSingleton();
				RE::InterfaceStrings* interfaceStrings = RE::InterfaceStrings::GetSingleton();

				RE::JournalMenu* menu = static_cast<RE::JournalMenu*>(ui->GetMenu(interfaceStrings->journalMenu).get());
				assert(menu);

				RE::GFxValue iSaveDelayTimerID;
				menu->uiMovie->GetVariable(&iSaveDelayTimerID, "_root.QuestJournalFader.Menu_mc.SystemFader.Page_mc.iSaveDelayTimerID");
				assert(iSaveDelayTimerID.GetType() != RE::GFxValue::ValueType::kUndefined);

				RE::GFxValue result;
				bool success = menu->uiMovie->Invoke("clearInterval", &result, &iSaveDelayTimerID, 1); // Not sure if this actually does something
				assert(success);

				// This function is normally supposed to close the menu, and it can get called multiple times. Make sure we only save once.
				if (!isSaving)
				{
					isSaving = true;
					RE::GFxValue selectedIndex;
					menu->uiMovie->GetVariable(&selectedIndex, "_root.QuestJournalFader.Menu_mc.SystemFader.Page_mc.SaveLoadListHolder.selectedIndex");
					assert(selectedIndex.GetType() != RE::GFxValue::ValueType::kUndefined);

					if (!menu->PausesGame())
					{
						menu->menuFlags |= RE::IMenu::Flag::kPausesGame;
						RE::UI::GetSingleton()->numPausesGame++;
					}

					//Create save screenshot
					reinterpret_cast<void(*)()>(Offsets::Misc::CreateSaveScreenshot.address())();

					this->selectedIndex = selectedIndex.GetNumber();
					this->usesDelay = true;
					this->beginTime = std::chrono::high_resolution_clock::now();
					this->delayTimeMS = std::chrono::milliseconds(Settings::GetSingleton()->saveDelayMS);
					UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
					queue->AddTask(this);

				}
			}

			void Run() override
			{
				RE::UI* ui = RE::UI::GetSingleton();

				if (ui->IsMenuOpen(RE::JournalMenu::MENU_NAME))
				{
					RE::JournalMenu* menu = static_cast<RE::JournalMenu*>(ui->GetMenu(RE::JournalMenu::MENU_NAME).get());

					RE::GFxValue selectedIndex = this->selectedIndex;

					RE::FxDelegateArgs args(0, menu, menu->uiMovie.get(), &selectedIndex, 1);
					menu->fxDelegate->callbacks.GetAlt("SaveGame")->callback(args);

					RE::UIMessageQueue* uiMessageQueue = RE::UIMessageQueue::GetSingleton();
					uiMessageQueue->AddMessage(RE::JournalMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kForceHide, nullptr);
					isSaving = false;
					RE::PlaySound("UIMenuCancel");
				}
			}

			void Dispose() override {}
		};

		RE::JournalMenu* menu = static_cast<RE::JournalMenu*>(CreateMenu(RE::JournalMenu::MENU_NAME.data()));

		//fix for remapping from MCM menu
		RE::GFxValue globals, skse;
		
		bool result = menu->uiMovie->GetVariable(&globals, "_global");
		if (result)
		{
			result = globals.GetMember("skse", &skse);
			if (result)
			{
				RE::GFxFunctionHandler* fn = nullptr;
				fn = new MCMRemapHandler();
				RE::GFxValue fnValue;
				menu->uiMovie.get()->CreateFunction(&fnValue, fn);
				skse.SetMember("StartRemapMode", fnValue);
			}
		}

		// Fix for game saves
		RE::GFxValue obj;
		bool success = menu->uiMovie->GetVariable(&obj, "_root.QuestJournalFader.Menu_mc.SystemFader.Page_mc");
		assert(success);

		RE::GFxValue func, func2;
		menu->uiMovie->CreateFunction(&func, new SaveGameHandler());
		obj.SetMember("DoSaveGame", func);

		return menu;
	}

	RE::BSEventNotifyControl JournalMenuEx::RemapHandler::ProcessEvent_Hook(RE::InputEvent** a_event, RE::BSTEventSource<RE::InputEvent**>* a_eventSource)
	{
		if (!*a_event || (*a_event)->eventType != RE::INPUT_EVENT_TYPE::kButton)
		{
			return RE::BSEventNotifyControl::kContinue;
		}

		RE::INPUT_DEVICE deviceType = (*a_event)->device.get();

		RE::BSInputDeviceManager* idm = RE::BSInputDeviceManager::GetSingleton();

		if ((idm->IsGamepadEnabled() ^ (deviceType == RE::INPUT_DEVICE::kGamepad)) || (*a_event)->AsButtonEvent()->value == 0 || (*a_event)->AsButtonEvent()->heldDownSecs != 0.0)
		{
			return RE::BSEventNotifyControl::kContinue;
		}

		idm->RemoveEventSink(this);

		class RemapTask : public UnpausedTask
		{
		private:
			FakeButtonEvent*	evn;
			RemapHandler*		handler;
		public:
			RemapTask(RemapHandler* a_handler, FakeButtonEvent* a_evn)
			{
				this->evn = a_evn;
				this->handler = a_handler;
			}

			void Run() override
			{
				RE::InputEvent* inputEvent = reinterpret_cast<RE::InputEvent*>(evn);
				_ProcessEvent(handler, &inputEvent, nullptr);
			}
		};

		RE::ButtonEvent* buttonEvent = (*a_event)->AsButtonEvent();
		FakeButtonEvent* fakeEvent = new FakeButtonEvent();
		fakeEvent->device = buttonEvent->device.get();
		fakeEvent->eventType = buttonEvent->eventType.get();
		fakeEvent->next = nullptr;
		fakeEvent->userEvent = buttonEvent->userEvent;
		fakeEvent->idCode = buttonEvent->idCode;
		fakeEvent->value = buttonEvent->value;
		fakeEvent->heldDownSecs = buttonEvent->heldDownSecs;

		UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
		queue->AddTask(new RemapTask(this, fakeEvent));

		return RE::BSEventNotifyControl::kContinue;
	}

	void JournalMenuEx::InstallHook()
	{
		//Hook ProcessMessage
		REL::Relocation<std::uintptr_t> vTable(Offsets::Menus::JournalMenu::Vtbl);
		_ProcessMessage = vTable.write_vfunc(0x4, &JournalMenuEx::ProcessMessage_Hook);

		//Hook AdvanceMovie
		_AdvanceMovie = vTable.write_vfunc(0x5, &JournalMenuEx::AdvanceMovie_Hook);

		REL::Relocation<std::uintptr_t> vTable_remapHandler(Offsets::Menus::JournalMenu::RemapHandler_Vtbl);
		RemapHandler::_ProcessEvent = vTable_remapHandler.write_vfunc(0x1, &JournalMenuEx::RemapHandler::ProcessEvent_Hook);
	}
};
