#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{

	// menuDepth = 3
	// flags = kPausesGame | kUsesMenuContext | kDisablePauseMenu | kUpdateUsesCursor | kInventoryItemMenu | kCustomRendering
	// context = kItemMenu
	class GiftMenuEx : public RE::GiftMenu
	{
	public:

		void AdvanceMovie_Hook(float a_interval, std::uint32_t a_currentTime); // 05

		void UpdateBottomBar();

		static RE::IMenu* Creator();
		static void InstallHook();

		static void ItemSelect_Hook(const RE::FxDelegateArgs& a_args);

		using AdvanceMovie_t = decltype(&RE::BarterMenu::AdvanceMovie);
		static inline REL::Relocation<AdvanceMovie_t> _AdvanceMovie;

		static inline RE::FxDelegateHandler::CallbackFn* _ItemSelect;
	};
}