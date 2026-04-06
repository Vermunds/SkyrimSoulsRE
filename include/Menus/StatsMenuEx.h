#pragma once
#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	struct StatsMenuState
	{
		float health = -1.0f;
		float maxHealth = -1.0f;
		float stamina = -1.0f;
		float magicka = -1.0f;
		float maxMagicka = -1.0f;
		float maxStamina = -1.0f;

		RE::PlayerCharacter::PlayerSkills::Data playerData;
	};

	// menuDepth = 3
	// flags = kPausesGame | kUsesMenuContext | kDisablePauseMenu | kRequiresUpdate | kCustomRendering
	// kUsesCursor if gamepad disabled
	// context = kStats
	class StatsMenuEx : public RE::StatsMenu
	{
	public:
		RE::UI_MESSAGE_RESULTS ProcessMessage_Hook(RE::UIMessage& a_message);  // 04

		StatsMenuState GetUpdatedState();
		void Update();

		static RE::IMenu* Creator();
		static void InstallHook();

		static inline bool isSleeping = false;

		using ProcessMessage_t = decltype(&RE::StatsMenu::ProcessMessage);
		static inline REL::Relocation<ProcessMessage_t> _ProcessMessage;

		static inline StatsMenuState lastState;
	};
}
