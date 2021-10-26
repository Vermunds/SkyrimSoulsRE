#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	// menuDepth = 0
	// flags = kPausesGame | kUpdateUsesCursor | kAllowSaving | kDontHideCursorWhenTopmost | kCustomRendering
	// context = kMenuMode
	class TweenMenuEx : public RE::TweenMenu
	{
	public:
		// override (IMenu)
		void AdvanceMovie_Hook(float a_interval, std::uint32_t a_currentTime);  // 05

		void UpdateClock();

		static RE::IMenu* Creator();
		static void InstallHook();

		using AdvanceMovie_t = decltype(&RE::TweenMenu::AdvanceMovie);
		static inline REL::Relocation<AdvanceMovie_t> _AdvanceMovie;
	};
}
