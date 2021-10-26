#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	// menuDepth = 1
	// flags = kPausesGame | kUsesMenuContext | kDisablePauseMenu | kRequiresUpdate | kTopmostRenderedMenu | kRendersOffscreenTargets
	// context = kBook
	class BookMenuEx : public RE::BookMenu
	{
	public:
		RE::UI_MESSAGE_RESULTS ProcessMessage_Hook(RE::UIMessage& a_message);   // 04
		void AdvanceMovie_Hook(float a_interval, std::uint32_t a_currentTime);  // 05

		static RE::IMenu* Creator();
		static void InstallHook();

		using AdvanceMovie_t = decltype(&RE::BookMenu::AdvanceMovie);
		static inline REL::Relocation<AdvanceMovie_t> _AdvanceMovie;

		using ProcessMessage_t = decltype(&RE::BookMenu::ProcessMessage);
		static inline REL::Relocation<ProcessMessage_t> _ProcessMessage;
	};
}
