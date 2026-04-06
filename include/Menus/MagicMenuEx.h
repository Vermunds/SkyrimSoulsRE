#pragma once

#include "SkyrimSoulsRE.h"

#include <unordered_set>

namespace SkyrimSoulsRE
{
	struct MagicMenuState
	{
		float health = -1.0f;
		float maxHealth = -1.0f;
		float stamina = -1.0f;
		float maxStamina = -1.0f;
		float magicka = -1.0f;
		float maxMagicka = -1.0f;

		std::uint64_t activeEffectsHash = 0;
	};
	static_assert(std::is_trivially_copyable_v<MagicMenuState>);

	struct ActiveEffectData
	{
		RE::ActiveEffect* effect = nullptr;
		std::string lastDisplay;
	};

	// menuDepth = 0
	// flags = kPausesGame | kUsesMenuContext | kDisablePauseMenu | kUpdateUsesCursor | kInventoryItemMenu | kCustomRendering
	// context = kItemMenu
	class MagicMenuEx : public RE::MagicMenu
	{
	public:
		RE::UI_MESSAGE_RESULTS ProcessMessage_Hook(RE::UIMessage& a_message);

		static RE::BSContainer::ForEachResult MagicMenuAddActiveEffectVisitor_Visit_Hook(RE::MagicMenuAddActiveEffectVisitor* a_this, RE::ActiveEffect* a_effect);
		static bool MagicItemList_Reset_Hook(RE::GFxValue::ObjectInterface* a_this, void* a_pdata, std::uint32_t a_idx, std::uint32_t a_count);  // GFxValue::ObjectInterface::RemoveElements

		bool IsViewingActiveEffects();

		MagicMenuState GetUpdatedState();
		void Update();
		void UpdateActiveEffectTimers();

		static RE::IMenu* Creator();
		static void InstallHook();

		using ProcessMessage_t = decltype(&RE::MagicMenu::ProcessMessage);
		static inline REL::Relocation<ProcessMessage_t> _ProcessMessage;

		using MagicMenuAddActiveEffectVisitor_Visit_t = decltype(&RE::MagicMenuAddActiveEffectVisitor::Visit);
		static inline REL::Relocation<MagicMenuAddActiveEffectVisitor_Visit_t> _MagicMenuAddActiveEffectVisitor_Visit;

		inline static MagicMenuState lastState = {};
		inline static std::unordered_map<RE::MagicItemList::Item*, ActiveEffectData> activeEffectMappings = {};

		inline static bool isSkyUI6 = false;
	};
}
