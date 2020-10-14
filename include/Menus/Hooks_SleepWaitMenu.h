#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	// menuDepth = 4
	// flags = kPausesGame | kRequiresUpdate | kAllowSaving | kApplicationMenu
	// kUsesCursor if gamepad disabled
	// context = kMenuMode
	class SleepWaitMenuEx : public RE::SleepWaitMenu
	{
	public:
		RE::UI_MESSAGE_RESULTS	ProcessMessage_Hook(RE::UIMessage& a_message);
		void					AdvanceMovie_Hook(float a_interval, std::uint32_t a_currentTime);

		void UpdateClock();

		static void StartSleepWait_Hook(const RE::FxDelegateArgs& a_args);

		static RE::IMenu* Creator();
		static void InstallHook();

		using AdvanceMovie_t = decltype(&RE::SleepWaitMenu::AdvanceMovie);
		using ProcessMessage_t = decltype(&RE::SleepWaitMenu::ProcessMessage);

		static inline REL::Relocation<AdvanceMovie_t> _AdvanceMovie;
		static inline REL::Relocation<ProcessMessage_t> _ProcessMessage;

		static inline RE::FxDelegateHandler::CallbackFn* _StartSleepWait;
	};

}