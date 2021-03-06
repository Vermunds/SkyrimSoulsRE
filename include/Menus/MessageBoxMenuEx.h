#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	class MessageBoxMenuEx : public RE::MessageBoxMenu
	{
	public:
		void AdvanceMovie_Hook(float a_interval, std::uint32_t a_currentTime);

		static RE::IMenu* Creator();
		static void InstallHook();

		static void ButtonPress_Hook(const RE::FxDelegateArgs& a_args);

		static inline RE::FxDelegateHandler::CallbackFn* _ButtonPress;

		using AdvanceMovie_t = decltype(&RE::MessageBoxMenu::AdvanceMovie);
		static inline REL::Relocation<AdvanceMovie_t> _AdvanceMovie;
	};
}