#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	// menuDepth = 3
	// flags = kPausesGame | kDisablePauseMenu | kRequiresUpdate
	// context = kLockpicking
	class LockpickingMenuEx : public RE::LockpickingMenu
	{
	public:
		RE::UI_MESSAGE_RESULTS ProcessMessage_Hook(RE::UIMessage& a_message);
		void AdvanceMovie_Hook(float a_interval, std::uint32_t a_currentTime);  // 05

		static RE::IMenu* Creator();
		static void InstallHook();

		using ProcessMessage_t = decltype(&RE::LockpickingMenu::ProcessMessage);
		static inline REL::Relocation<ProcessMessage_t> _ProcessMessage;

		using AdvanceMovie_t = decltype(&RE::LockpickingMenu::AdvanceMovie);
		static inline REL::Relocation<AdvanceMovie_t> _AdvanceMovie;
	};
}