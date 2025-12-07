#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	struct TweenMenuState
	{
		char timeDateString[200] = { 0 };
		bool canLevelUp = false;
		float level = 0.0f;
		float xp = 0.0f;
		float levelUpThreshold = 0.0f;
	};

	// menuDepth = 0
	// flags = kPausesGame | kUpdateUsesCursor | kAllowSaving | kDontHideCursorWhenTopmost | kCustomRendering
	// context = kMenuMode
	class TweenMenuEx : public RE::TweenMenu
	{
	public:
		// override (IMenu)
		RE::UI_MESSAGE_RESULTS ProcessMessage_Hook(RE::UIMessage& a_message);  // 05

		TweenMenuState GetUpdatedState();
		void UpdateInfo();

		static RE::IMenu* Creator();
		static void InstallHook();

		using ProcessMessage_t = decltype(&RE::TweenMenu::ProcessMessage);
		static inline REL::Relocation<ProcessMessage_t> _ProcessMessage;

		static inline TweenMenuState lastState = {};
	};
}
