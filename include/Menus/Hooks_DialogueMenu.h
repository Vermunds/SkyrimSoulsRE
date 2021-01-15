#pragma once

namespace SkyrimSoulsRE
{
	// menuDepth = 3
	// flags = kUpdateUsesCursor | kDontHideCursorWhenTopmost
	// context = kMenuMode
	class DialogueMenuEx : public RE::DialogueMenu
	{
	public:
		void AdvanceMovie_Hook(float a_interval, std::uint32_t a_currentTime);	// 05

		static void UpdateAutoCloseTimer_Hook(RE::AIProcess* a_process, float a_delta);	 // This runs every frame

		static void InstallHook();

		using AdvanceMovie_t = decltype(&RE::DialogueMenu::AdvanceMovie);
		static inline REL::Relocation<AdvanceMovie_t> _AdvanceMovie;
	};
}
