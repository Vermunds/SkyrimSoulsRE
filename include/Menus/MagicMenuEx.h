#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	// menuDepth = 0
	// flags = kPausesGame | kUsesMenuContext | kDisablePauseMenu | kUpdateUsesCursor | kInventoryItemMenu | kCustomRendering
	// context = kItemMenu
	class MagicMenuEx : public RE::MagicMenu
	{
	public:
		RE::UI_MESSAGE_RESULTS ProcessMessage_Hook(RE::UIMessage& a_message);   // 04
		void AdvanceMovie_Hook(float a_interval, std::uint32_t a_currentTime);  // 05

		bool IsViewingActiveEffects();
		void UpdateBottomBar();

		static void AddSpells_Hook(RE::PlayerCharacter* a_player, void* a_unk1);
		static void AddShouts_Hook(RE::ItemList* a_list, void* a_unk1, RE::RefHandle a_playerRefHandle);

		static RE::IMenu* Creator();
		static void InstallHook();

		static inline bool blockSpells = false;
		static inline bool wasViewingActiveEffects = false;

		using ProcessMessage_t = decltype(&RE::MagicMenu::ProcessMessage);
		static inline REL::Relocation<ProcessMessage_t> _ProcessMessage;

		using AdvanceMovie_t = decltype(&RE::MagicMenu::AdvanceMovie);
		static inline REL::Relocation<AdvanceMovie_t> _AdvanceMovie;
	};
}
