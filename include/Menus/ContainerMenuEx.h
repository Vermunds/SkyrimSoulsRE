#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	// menuDepth = 0
	// flags = kPausesGame | kUsesMenuContext | kDisablePauseMenu | kUpdateUsesCursor | kInventoryItemMenu | kCustomRendering
	// context = kItemMenu
	class ContainerMenuEx : public RE::ContainerMenu
	{
	public:
		static inline std::wstring toSteal = L"$ TO STEAL";
		static inline std::wstring toPlace = L"$ TO PLACE";

		// Virtual functions
		RE::UI_MESSAGE_RESULTS ProcessMessage_Hook(RE::UIMessage& a_message);   // 04
		void AdvanceMovie_Hook(float a_interval, std::uint32_t a_currentTime);  // 05

		// FxDelegate callbacks
		static void TransferItem_Hook(const RE::FxDelegateArgs& a_args);
		static void EquipItem_Hook(const RE::FxDelegateArgs& a_args);
		static void TakeAllItems_Hook(const RE::FxDelegateArgs& a_args);

		void UpdatePickpocketChance();
		void UpdateBottomBar();

		std::int32_t CalcPickPocketChance(RE::StandardItemData* a_itemData);

		static RE::IMenu* Creator();
		static void InstallHook();
		static void ParseTranslations();

		using AdvanceMovie_t = decltype(&RE::ContainerMenu::AdvanceMovie);
		static inline REL::Relocation<AdvanceMovie_t> _AdvanceMovie;

		using ProcessMessage_t = decltype(&RE::ContainerMenu::ProcessMessage);
		static inline REL::Relocation<ProcessMessage_t> _ProcessMessage;

		static inline RE::FxDelegateHandler::CallbackFn* _TransferItem;
		static inline RE::FxDelegateHandler::CallbackFn* _EquipItem;
		static inline RE::FxDelegateHandler::CallbackFn* _TakeAllItems;
	};
}
