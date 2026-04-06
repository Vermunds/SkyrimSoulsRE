#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	struct ContainerMenuState
	{
		float health = -1.0f;
		float maxHealth = -1.0f;
		float stamina = -1.0f;
		float magicka = -1.0f;
		float maxMagicka = -1.0f;
		float maxStamina = -1.0f;

		RE::StandardItemData* selectedItemData = nullptr;
		std::int32_t pickpocketChance = -1;
	};

	// menuDepth = 0
	// flags = kPausesGame | kUsesMenuContext | kDisablePauseMenu | kUpdateUsesCursor | kInventoryItemMenu | kCustomRendering
	// context = kItemMenu
	class ContainerMenuEx : public RE::ContainerMenu
	{
	public:
		RE::UI_MESSAGE_RESULTS ProcessMessage_Hook(RE::UIMessage& a_message);  // 04

		ContainerMenuState GetUpdatedState() const;
		void Update();

		std::int32_t CalcPickPocketChance() const;

		static RE::IMenu* Creator();
		static void InstallHook();

		using ProcessMessage_t = decltype(&RE::ContainerMenu::ProcessMessage);
		static inline REL::Relocation<ProcessMessage_t> _ProcessMessage;

		inline static ContainerMenuState lastState = {};
	};
}
