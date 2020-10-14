#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	class BarterMenuEx : public RE::BarterMenu
	{
	public:
		void AdvanceMovie_Hook(float a_interval, std::uint32_t a_currentTime); // 05

		static void ItemSelect_Hook(const RE::FxDelegateArgs& a_args);

		void UpdateBottomBar();

		static RE::IMenu* Creator();
		static void InstallHook();

		using AdvanceMovie_t = decltype(&RE::BarterMenu::AdvanceMovie);
		static inline REL::Relocation<AdvanceMovie_t> _AdvanceMovie;

		static inline RE::FxDelegateHandler::CallbackFn* _ItemSelect;
	};
}