#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	// menuDepth = 0
	// flags = kPausesGame | kDisablePauseMenu | kUpdateUsesCursor | kInventoryItemMenu | kCustomRendering
	// context = kNone
	class InventoryMenuEx : public RE::InventoryMenu
	{
	public:

		RE::UI_MESSAGE_RESULTS ProcessMessage_Hook(RE::UIMessage& a_message); //04
		void AdvanceMovie_Hook(float a_interval, std::uint32_t a_currentTime);	 // 05

		void UpdateBottomBar();

		static void ItemDrop_Hook(const RE::FxDelegateArgs& a_args);
		static void ItemSelect_Hook(const RE::FxDelegateArgs& a_args);

		static RE::IMenu* Creator();
		static void InstallHook();

		using ProcessMessage_t = decltype(&RE::InventoryMenu::ProcessMessage);
		static inline REL::Relocation<ProcessMessage_t> _ProcessMessage;

		using AdvanceMovie_t = decltype(&RE::InventoryMenu::AdvanceMovie);
		static inline REL::Relocation<AdvanceMovie_t> _AdvanceMovie;

		static inline RE::FxDelegateHandler::CallbackFn* _ItemSelect;
		static inline RE::FxDelegateHandler::CallbackFn* _ItemDrop;
	};
}