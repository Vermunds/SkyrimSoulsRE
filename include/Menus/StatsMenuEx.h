#pragma once
#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	// menuDepth = 3
	// flags = kPausesGame | kUsesMenuContext | kDisablePauseMenu | kRequiresUpdate | kCustomRendering
	// kUsesCursor if gamepad disabled
	// context = kStats
	class StatsMenuEx : public RE::StatsMenu
	{
	public:
		RE::UI_MESSAGE_RESULTS ProcessMessage_Hook(RE::UIMessage& a_message);  // 04

		static RE::IMenu* Creator();
		static void InstallHook();

		static inline bool isSleeping = false;

		using ProcessMessage_t = decltype(&RE::StatsMenu::ProcessMessage);
		static inline REL::Relocation<ProcessMessage_t> _ProcessMessage;

		using AdvanceMovie_t = decltype(&RE::StatsMenu::AdvanceMovie);
		static inline REL::Relocation<AdvanceMovie_t> _AdvanceMovie;
	};
}
