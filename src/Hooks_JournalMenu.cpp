#include "RE/Skyrim.h"

#include "skse64/InputMap.h" //InputMap::GamepadMaskToKeycode

#include "Settings.h"
#include "Offsets.h"
#include "Tasks.h"

#include "BGSSaveLoadManagerEx.h"

#include "SKSE/API.h"

namespace SkyrimSoulsRE::Hooks
{
	namespace JournalMenu
	{
		void SetJournalPaused(bool a_paused)
		{
			RE::UI* ui = RE::UI::GetSingleton();
			RE::InterfaceStrings* interfaceStrings = RE::InterfaceStrings::GetSingleton();
			SkyrimSoulsRE::Settings* settings = SkyrimSoulsRE::Settings::GetSingleton();

			RE::IMenu* journalMenu = ui->GetMenu(interfaceStrings->journalMenu).get();

			if (journalMenu && settings->unpausedMenus["Journal Menu"])
			{
				if (a_paused)
				{
					journalMenu->flags |= RE::IMenu::Flag::kPausesGame;
					ui->numPausesGame++;
				}
				else
				{
					journalMenu->flags &= ~RE::IMenu::Flag::kPausesGame;
					ui->numPausesGame--;
				}
			}
		}

		//Copied from SKSE64
		class SKSEScaleform_StartRemapMode : public RE::GFxFunctionHandler
		{
			class RemapHandler : public RE::BSTEventSink<RE::InputEvent*>
			{
			public:

				//End remap mode - controls menu
				virtual RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>* a_eventSource) override
				{
					RE::ButtonEvent* evn = (RE::ButtonEvent*) * a_event;

					// Make sure this is really a button event
					if (!evn || evn->eventType != RE::INPUT_EVENT_TYPE::kButton)
						return RE::BSEventNotifyControl::kContinue;

					RE::INPUT_DEVICE deviceType = evn->device;

					RE::BSInputDeviceManager* idm = static_cast<RE::BSInputDeviceManager*>(a_eventSource);

					if ((idm->IsGamepadEnabled() ^ (deviceType == RE::INPUT_DEVICE::kGamepad)) || evn->value == 0 || evn->heldDownSecs != 0.0)
					{
						return RE::BSEventNotifyControl::kContinue;
					}


					UInt32 keyMask = evn->idCode;
					UInt32 keyCode;

					// Mouse
					if (deviceType == RE::INPUT_DEVICE::kMouse)
						keyCode = InputMap::kMacro_MouseButtonOffset + keyMask;
					// Gamepad
					else if (deviceType == RE::INPUT_DEVICE::kGamepad)
						keyCode = InputMap::GamepadMaskToKeycode(keyMask);
					// Keyboard
					else
						keyCode = keyMask;

					// Valid scancode?
					if (keyCode >= InputMap::kMaxMacros)
						keyCode = -1;

					RE::GFxValue arg;
					arg.SetNumber(keyCode);
					scope.Invoke("EndRemapMode", NULL, &arg, 1);
					SetJournalPaused(false);

					RE::MenuControls::GetSingleton()->remapMode = false;
					RE::PlayerControls::GetSingleton()->data.remapMode = false;

					a_eventSource->RemoveEventSink(this);
					return RE::BSEventNotifyControl::kContinue;
				}

				RE::GFxValue scope;
			};

			RemapHandler remapHandler;

			//Start remap mode - MCM menu
			virtual void Call(RE::GFxFunctionHandler::Params& a_args) override
			{

				SetJournalPaused(true);

				remapHandler.scope = a_args.args[0];

				RE::PlayerControls* playerControls = RE::PlayerControls::GetSingleton();
				if (!playerControls)
					return;

				RE::MenuControls* menuControls = RE::MenuControls::GetSingleton();
				if (!menuControls)
					return;
				RE::BSInputDeviceManager* pInputEventDispatcher = RE::BSInputDeviceManager::GetSingleton();
				if (!(pInputEventDispatcher))
					return;

				pInputEventDispatcher->AddEventSink(&remapHandler);
				menuControls->remapMode = true;
				playerControls->data.remapMode = true;
			}
		};

		//Start remap mode - controls menu
		void StartRemapMode_Hook(const RE::FxDelegateArgs& a_args)
		{
			SetJournalPaused(true);

			void(*StartRemapMode_Original)(const RE::FxDelegateArgs&);
			StartRemapMode_Original = reinterpret_cast<void(*)(const RE::FxDelegateArgs&)>(Offsets::JournalMenu_StartRemapMode_Original.GetUIntPtr());
			return StartRemapMode_Original(a_args);
		}

		//End remap mode - controls menu
		void FinishRemapMode_Hook(RE::GFxMovieView* a_movieView, const char* a_methodName, RE::FxResponseArgsBase& a_args)
		{

			RE::FxDelegate::Invoke(a_movieView, a_methodName, a_args);

			SetJournalPaused(false);
		}

		bool SaveGame_Hook(RE::BGSSaveLoadManager* a_this, BGSSaveLoadManagerEx::SaveType a_saveType, BGSSaveLoadManagerEx::OutputFlag a_outputFlag, const char* a_saveName)
		{
			Tasks::SaveGameDelegate::RegisterTask(a_outputFlag, a_saveName);
			return true;
		}

		void Register_Func(RE::JournalMenu* a_journalMenu)
		{
			RE::FxDelegate* dlg = a_journalMenu->fxDelegate.get();

			//Temporary fix for remapping from controls menu
			dlg->callbacks.GetAlt("StartRemapMode")->callback = JournalMenu::StartRemapMode_Hook;

			//Temporary fix for remapping from MCM menu
			RE::GFxValue globals, skse;

			bool result = a_journalMenu->view->GetVariable(&globals, "_global");
			if (!result)
			{
				_ERROR("Couldn't get _global");
				return;
			}

			result = globals.GetMember("skse", &skse);
			if (!result)
			{
				_ERROR("Couldn't get skse");
				return;
			}

			RE::GFxFunctionHandler* fn = nullptr;

			fn = new JournalMenu::SKSEScaleform_StartRemapMode();
			RE::GFxValue fnValue;
			a_journalMenu->view.get()->CreateFunction(&fnValue, fn);
			skse.SetMember("StartRemapMode", fnValue);
		}

		void InstallHook()
		{
			SKSE::GetTrampoline()->Write5Branch(Offsets::JournalMenu_SaveGame_Hook.GetUIntPtr(), (uintptr_t)SaveGame_Hook);
			SKSE::GetTrampoline()->Write5Call(Offsets::JournalMenu_SaveGame_Overwrite_Hook.GetUIntPtr(), (uintptr_t)SaveGame_Hook);
			SKSE::GetTrampoline()->Write5Call(Offsets::JournalMenu_SaveGame_Overwrite_Hook.GetUIntPtr() + 0x22, (uintptr_t)SaveGame_Hook);

			SKSE::GetTrampoline()->Write5Call(Offsets::JournalMenu_FinishRemapMode_Hook.GetUIntPtr(), (uintptr_t)FinishRemapMode_Hook);
		}
	}


};