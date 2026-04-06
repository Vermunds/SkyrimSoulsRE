#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	struct InventoryMenuState
	{
		float health = -1.0f;
		float maxHealth = -1.0f;
		float stamina = -1.0f;
		float magicka = -1.0f;
		float maxMagicka = -1.0f;
		float maxStamina = -1.0f;
	};

	// menuDepth = 0
	// flags = kPausesGame | kDisablePauseMenu | kUpdateUsesCursor | kInventoryItemMenu | kCustomRendering
	// context = kNone
	class InventoryMenuEx : public RE::InventoryMenu
	{
	public:
		RE::UI_MESSAGE_RESULTS ProcessMessage_Hook(RE::UIMessage& a_message);  // 04

		InventoryMenuState GetUpdatedState();

		void Update();

		static RE::IMenu* Creator();
		static void InstallHook();

		using ProcessMessage_t = decltype(&RE::InventoryMenu::ProcessMessage);
		static inline REL::Relocation<ProcessMessage_t> _ProcessMessage;

		inline static InventoryMenuState lastState = {};
	};
}
