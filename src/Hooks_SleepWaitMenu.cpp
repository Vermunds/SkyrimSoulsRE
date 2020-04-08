#include "RE/Skyrim.h"
#include "Offsets.h"
#include "Tasks.h"
#include "Settings.h"
#include "Events.h"

namespace SkyrimSoulsRE::Hooks
{
	namespace SleepWaitMenu
	{
		//Fix mouse controls in sleep/wait menu
		RE::UI_MESSAGE_RESULTS ProcessMessage(RE::IMenu* a_thisMenu, RE::UIMessage& a_message)
		{
			if (a_message.type == RE::UI_MESSAGE_TYPE::kScaleformEvent)
			{
				return a_thisMenu->RE::IMenu::ProcessMessage(a_message);
			}
			RE::UI_MESSAGE_RESULTS(*ProcessMessage_Original)(RE::IMenu*, RE::UIMessage&);
			ProcessMessage_Original = reinterpret_cast<RE::UI_MESSAGE_RESULTS(*)(RE::IMenu*, RE::UIMessage&)>(Offsets::SleepWaitMenu_ProcessMessage_Original.GetUIntPtr());
			return ProcessMessage_Original(a_thisMenu, a_message);
		}

		void StartSleepWait_Hook(const RE::FxDelegateArgs& a_args) {
			RE::UI* ui = RE::UI::GetSingleton();
			RE::InterfaceStrings* interfaceStrings = RE::InterfaceStrings::GetSingleton();
			SkyrimSoulsRE::Settings* settings = SkyrimSoulsRE::Settings::GetSingleton();

			ui->GetMenu(interfaceStrings->sleepWaitMenu)->flags |= RE::IMenu::Flag::kPausesGame;
			ui->numPausesGame++;

			if (SkyrimSoulsRE::isInSlowMotion)
			{
				float slowMotionMultiplier = settings->slowMotionMultiplier;
				float* globalTimescale = reinterpret_cast<float*>(Offsets::GlobalTimescaleMultipler.GetUIntPtr());
				float* globalTimescaleHavok = reinterpret_cast<float*>(Offsets::GlobalTimescaleMultipler_Havok.GetUIntPtr());

				float multiplier;
				if (slowMotionMultiplier >= 0.1 && 1.0 >= slowMotionMultiplier)
				{
					multiplier = slowMotionMultiplier;
				}
				else {
					multiplier = 1.0;
				}

				SkyrimSoulsRE::isInSlowMotion = false;
				*globalTimescale = (1.0 / multiplier) * (*globalTimescale);
				*globalTimescaleHavok = (1.0 / multiplier) * (*globalTimescaleHavok);
			}

			Tasks::SleepWaitDelegate::RegisterTask(a_args);
		}

		void Register_Func(RE::SleepWaitMenu* a_sleepWaitMenu)
		{
			RE::FxDelegate* dlg = a_sleepWaitMenu->fxDelegate.get();
			dlg->callbacks.GetAlt("OK")->callback = StartSleepWait_Hook;
		}

		void InstallHook()
		{
			//fix for controls not working
			SKSE::SafeWrite64(Offsets::SleepWaitMenu_ProcessMessage_Hook.GetUIntPtr(), (uintptr_t)ProcessMessage);

		}
	}
}