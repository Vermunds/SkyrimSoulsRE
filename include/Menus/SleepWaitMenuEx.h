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
		inline constexpr static RE::UI_MESSAGE_TYPE SET_BED_REFERENCE_MESSAGE_TYPE = static_cast<RE::UI_MESSAGE_TYPE>(11000);

		RE::UI_MESSAGE_RESULTS ProcessMessage_Hook(RE::UIMessage& a_message);

		void UpdateClock();

		static bool CanSleep_Hook(RE::PlayerCharacter* a_player, RE::TESObjectREFR* a_bedRef);
		static void StartSleepWait_Hook(const RE::FxDelegateArgs& a_args);

		static RE::IMenu* Creator();
		static void InstallHook();

		using ProcessMessage_t = decltype(&RE::SleepWaitMenu::ProcessMessage);

		static inline REL::Relocation<ProcessMessage_t> _ProcessMessage;

		static inline RE::FxDelegateHandler::CallbackFn* _StartSleepWait;

		using CanSleep_t = decltype(&SleepWaitMenuEx::CanSleep_Hook);
		static inline REL::Relocation<CanSleep_t> _CanSleep;

		static inline char lastTimeDateString[200] = { 0 };
		static inline bool bedReferenceMessageReceived = false;
	};
}
