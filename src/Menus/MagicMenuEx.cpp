#include "Menus/MagicMenuEx.h"
#include "Util.h"

namespace SkyrimSoulsRE
{
	bool MagicMenuEx::IsViewingActiveEffects()
	{
		RE::GFxValue inventoryLists;
		if (this->root.GetMember("inventoryLists", &inventoryLists))  // SkyUI
		{
			RE::GFxValue categoryList;
			RE::GFxValue selectedIndex;

			if (!inventoryLists.GetMember("_currCategoryIndex", &selectedIndex))
			{
				return false;
			}

			return static_cast<int>(std::round(selectedIndex.GetNumber())) == 9;
		}
		else if (this->root.GetMember("InventoryLists_mc", &inventoryLists))  // Vanilla
		{
			RE::GFxValue currentCategoryIndex;
			if (!inventoryLists.GetMember("iCurrCategoryIndex", &currentCategoryIndex))
			{
				return false;
			}

			return static_cast<int>(std::round(currentCategoryIndex.GetNumber())) == 9;
		}

		return false;
	}

	MagicMenuState MagicMenuEx::GetUpdatedState()
	{
		RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
		MagicMenuState state{};
		state.health = player->GetActorValue(RE::ActorValue::kHealth);
		state.maxHealth = state.health - player->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIER::kDamage, RE::ActorValue::kHealth);
		state.stamina = player->GetActorValue(RE::ActorValue::kStamina);
		state.maxStamina = state.stamina - player->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIER::kDamage, RE::ActorValue::kStamina);
		state.magicka = player->GetActorValue(RE::ActorValue::kMagicka);
		state.maxMagicka = state.magicka - player->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIER::kDamage, RE::ActorValue::kMagicka);

		RE::BSSimpleList<RE::ActiveEffect*>* activeEffects = player->GetActiveEffectList();
		for (RE::ActiveEffect* activeEffect : *activeEffects)
		{
			if (activeEffect->flags.any(RE::ActiveEffect::Flag::kInactive) || activeEffect->GetBaseObject()->data.flags.any(RE::EffectSetting::EffectSettingData::Flag::kHideInUI))
			{
				continue;
			}

			uint64_t h = std::hash<RE::ActiveEffect*>()(activeEffect);
			state.activeEffectsHash ^= h + 0x9e3779b97f4a7c15ULL + (state.activeEffectsHash << 6) + (state.activeEffectsHash >> 2);
		}

		return state;
	}

	RE::UI_MESSAGE_RESULTS MagicMenuEx::ProcessMessage_Hook(RE::UIMessage& a_message)
	{
		switch (a_message.type.get())
		{
		case RE::UI_MESSAGE_TYPE::kShow:
			{
				RE::GFxValue skyuiVersion;

				isSkyUI6 = this->uiMovie->GetVariable(&skyuiVersion, "_global.MagicMenu.SKYUI_VERSION_MAJOR") && skyuiVersion.IsNumber() && static_cast<int32_t>(skyuiVersion.GetNumber()) >= 6;
				lastState = MagicMenuState{};
			}
			break;

		case RE::UI_MESSAGE_TYPE::kUpdate:
			Update();
			break;
		}

		return _ProcessMessage(this, a_message);
	}

	// Called during itemlist update
	RE::BSContainer::ForEachResult MagicMenuEx::MagicMenuAddActiveEffectVisitor_Visit_Hook(RE::MagicMenuAddActiveEffectVisitor* a_this, RE::ActiveEffect* a_effect)
	{
		if (a_effect->flags.any(RE::ActiveEffect::Flag::kInactive) || a_effect->GetBaseObject()->data.flags.any(RE::EffectSetting::EffectSettingData::Flag::kHideInUI))
		{
			return RE::BSContainer::ForEachResult::kContinue;
		}

		RE::BSContainer::ForEachResult result = _MagicMenuAddActiveEffectVisitor_Visit(a_this, a_effect);

		if (a_this->itemList->items.empty())
		{
			return result;
		}

		RE::MagicItemList::Item* lastEntry = a_this->itemList->items.back();

		if (lastEntry)
		{
			activeEffectMappings.emplace(lastEntry, ActiveEffectData{ a_effect, std::string{} });
		}

		return result;
	}

	// Called during itemlist reset - clear the map here to account for cases where the itemlist is reset without an inventory update
	bool MagicMenuEx::MagicItemList_Reset_Hook(RE::GFxValue::ObjectInterface* a_this, void* a_pdata, std::uint32_t a_idx, std::uint32_t a_count)
	{
		activeEffectMappings.clear();

		return a_this->RemoveElements(a_pdata, a_idx, a_count);
	}

	void MagicMenuEx::UpdateActiveEffectTimers()
	{
		RE::MagicItemList::Item* selectedItem = this->itemList->GetSelectedItem();

		RE::GFxValue entryClipManager;
		bool entryClipManagerFetched = false;
		bool entryClipManagerNotFound = false;

		for (RE::MagicItemList::Item* entry : this->itemList->items)
		{
			if (!entry)
			{
				continue;
			}

			auto it = activeEffectMappings.find(entry);
			if (it == activeEffectMappings.end())
			{
				continue;
			}
			ActiveEffectData& activeEffectData = it->second;

			float remaining = std::max(0.0f, activeEffectData.effect->duration - activeEffectData.effect->elapsedSeconds);

			static const std::string dayLabel = isSkyUI6 ? Util::TranslateSkyUIString(this->uiMovie.get(), "$d") : "d";
			static const std::string hourLabel = isSkyUI6 ? Util::TranslateSkyUIString(this->uiMovie.get(), "$h") : "h";
			static const std::string minuteLabel = isSkyUI6 ? Util::TranslateSkyUIString(this->uiMovie.get(), "$m") : "m";
			static const std::string secondLabel = isSkyUI6 ? Util::TranslateSkyUIString(this->uiMovie.get(), "$s") : "s";

			std::string newDisplay = Util::FormatTimeRemaining(remaining, dayLabel, hourLabel, minuteLabel, secondLabel);
			std::string& prevDisplay = activeEffectData.lastDisplay;

			if (!prevDisplay.empty() && prevDisplay == newDisplay)
			{
				continue;
			}

			prevDisplay = newDisplay;

			RE::GFxValue clipIndex;
			entry->obj.GetMember("clipIndex", &clipIndex);
			if (!clipIndex.IsNumber())
			{
				continue;
			}

			// Update entry data
			RE::GFxValue val(remaining);
			RE::GFxValue textVal;
			textVal.SetString(newDisplay.c_str());
			entry->obj.SetMember("timeRemaining", val);
			entry->obj.SetMember("timeRemainingDisplay", textVal);

			// Update item card if selected
			if (selectedItem == entry)
			{
				this->itemCard->obj.SetMember("timeRemaining", remaining);
				this->root.Invoke("UpdateItemCardInfo", nullptr, &this->itemCard->obj, 1);
			}

			// Update list entry (SkyUI)
			if (entryClipManagerNotFound)
			{
				continue;
			}

			if (!entryClipManagerFetched)
			{
				this->itemList->root.GetMember("_entryClipManager", &entryClipManager);
				if (!entryClipManager.IsObject())
				{
					entryClipManagerNotFound = true;
					continue;
				}

				entryClipManagerFetched = true;
			}

			RE::GFxValue clip;
			entryClipManager.Invoke("getClip", &clip, &clipIndex, 1);
			if (!clip.IsObject())
			{
				continue;
			}

			RE::GFxValue args[2];
			args[0] = entry->obj;
			this->itemList->root.GetMember("listState", &args[1]);
			if (!args[1].IsObject())
			{
				continue;
			}

			clip.Invoke("setEntry", nullptr, args, 2);
		}
	}

	void MagicMenuEx::Update()
	{
		Settings* settings = Settings::GetSingleton();

		MagicMenuState newState = GetUpdatedState();

		if (settings->updateMagicMenuBottomBar)
		{
			bool meterUpdateRequired = Util::IsActorValueMeterUpdateNeeded(lastState.health, lastState.maxHealth, newState.health, newState.maxHealth, settings->bottomBarMeterUpdateSteps) ||
			                           Util::IsActorValueMeterUpdateNeeded(lastState.stamina, lastState.maxStamina, newState.stamina, newState.maxStamina, settings->bottomBarMeterUpdateSteps) ||
			                           Util::IsActorValueMeterUpdateNeeded(lastState.magicka, lastState.maxMagicka, newState.magicka, newState.maxMagicka, settings->bottomBarMeterUpdateSteps);

			if (meterUpdateRequired)
			{
				using func_t = void (*)(RE::MagicMenu*);
				REL::Relocation<func_t> func(Offsets::Menus::MagicMenu::UpdateBottomBar);
				func(this);
			}
		}

		if (settings->updateMagicMenuActiveEffectTimers && IsViewingActiveEffects())
		{
			bool listUpdateRequired = (newState.activeEffectsHash != lastState.activeEffectsHash);
			if (listUpdateRequired)
			{
				itemList->Update();
			}
			else
			{
				UpdateActiveEffectTimers();
			}
		}

		lastState = newState;
	}

	RE::IMenu* MagicMenuEx::Creator()
	{
		return CreateMenu(RE::MagicMenu::MENU_NAME);
	}

	void MagicMenuEx::InstallHook()
	{
		REL::Relocation<std::uintptr_t> vTable(RE::VTABLE_MagicMenu[0]);
		_ProcessMessage = vTable.write_vfunc(0x4, &MagicMenuEx::ProcessMessage_Hook);

		REL::Relocation<std::uintptr_t> vTableAddActiveEffectVisitor(RE::VTABLE___MagicMenuAddActiveEffectVisitor[0]);
		_MagicMenuAddActiveEffectVisitor_Visit = vTableAddActiveEffectVisitor.write_vfunc(0x1, &MagicMenuEx::MagicMenuAddActiveEffectVisitor_Visit_Hook);

		SKSE::Trampoline& trampoline = SKSE::GetTrampoline();
		trampoline.write_call<5>(Offsets::MagicItemList::Reset.address() + 0x3B, (uintptr_t)MagicItemList_Reset_Hook);

		activeEffectMappings.reserve(128);
	}
}
