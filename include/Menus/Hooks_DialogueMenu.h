#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	// menuDepth = 3
	// flags = kUpdateUsesCursor | kDontHideCursorWhenTopmost
	// context = kMenuMode
	class DialogueMenuEx : public RE::DialogueMenu
	{
	public:

		void AdvanceMovie_Hook(float a_interval, std::uint32_t  a_currentTime);

		static void UpdateAutoCloseTimer_Hook(uintptr_t a_unk, float a_delta); // This runs every frame

		static RE::IMenu* Creator();
		static void InstallHook();
		
		using AdvanceMovie_t = decltype(&RE::DialogueMenu::AdvanceMovie);
		static inline REL::Relocation<AdvanceMovie_t> _AdvanceMovie;
	};
}