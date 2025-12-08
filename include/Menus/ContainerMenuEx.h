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
		// Virtual functions
		RE::UI_MESSAGE_RESULTS ProcessMessage_Hook(RE::UIMessage& a_message);  // 04

		void UpdatePickpocketChance();
		void UpdateBottomBar();

		std::int32_t CalcPickPocketChance(RE::StandardItemData* a_itemData);

		static RE::IMenu* Creator();
		static void InstallHook();

		using ProcessMessage_t = decltype(&RE::ContainerMenu::ProcessMessage);
		static inline REL::Relocation<ProcessMessage_t> _ProcessMessage;
	};
}
