#include "Menus/ContainerMenuEx.h"
#include "Util.h"

namespace SkyrimSoulsRE
{
	RE::UI_MESSAGE_RESULTS ContainerMenuEx::ProcessMessage_Hook(RE::UIMessage& a_message)
	{
		AutoCloseManager* autoCloseManager = AutoCloseManager::GetSingleton();

		switch (a_message.type.get())
		{
		case RE::UI_MESSAGE_TYPE::kShow:
			{
				lastState = GetUpdatedState();

				bool checkForDeath = GetContainerMode() == RE::ContainerMenu::ContainerMode::kPickpocket;
				autoCloseManager->InitAutoClose(RE::ContainerMenu::MENU_NAME, GetTargetRefHandle(), checkForDeath);
			}
			break;

		case RE::UI_MESSAGE_TYPE::kUpdate:

			Update();
			autoCloseManager->CheckAutoClose(RE::ContainerMenu::MENU_NAME);

			break;
		}
		return _ProcessMessage(this, a_message);
	}

	ContainerMenuState ContainerMenuEx::GetUpdatedState() const
	{
		RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
		ContainerMenuState state{};
		state.health = player->GetActorValue(RE::ActorValue::kHealth);
		state.maxHealth = state.health - player->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIER::kDamage, RE::ActorValue::kHealth);
		state.stamina = player->GetActorValue(RE::ActorValue::kStamina);
		state.maxStamina = state.stamina - player->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIER::kDamage, RE::ActorValue::kStamina);
		state.magicka = player->GetActorValue(RE::ActorValue::kMagicka);
		state.maxMagicka = state.magicka - player->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIER::kDamage, RE::ActorValue::kMagicka);

		state.selectedItemData = itemList && itemList->GetSelectedItem() ? &itemList->GetSelectedItem()->data : nullptr;
		state.pickpocketChance = CalcPickPocketChance();

		return state;
	}

	std::int32_t ContainerMenuEx::CalcPickPocketChance() const
	{
		if (!itemList || GetContainerMode() != RE::ContainerMenu::ContainerMode::kPickpocket)
		{
			return -1;
		}

		RE::ItemList::Item* item = itemList->GetSelectedItem();
		if (!item)
		{
			return -1;
		}
		RE::StandardItemData& itemData = item->data;

		RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
		RE::TESObjectREFRPtr containerRef = RE::TESObjectREFR::LookupByHandle(GetTargetRefHandle());

		if (!containerRef)
		{
			return -1;
		}

		RE::Actor* targetActor = static_cast<RE::Actor*>(containerRef.get());

		float itemWeight = itemData.objDesc->GetWeight();
		std::uint32_t count = 1;

		if (itemData.objDesc->object->IsAmmo() || itemData.objDesc->object->IsLockpick() || itemData.objDesc->object->IsGold())
		{
			count = itemData.GetCount();
			itemWeight = -1.0f;
		}

		std::uint32_t stealValue = targetActor->GetStealValue(itemData.objDesc, count, true);

		bool isDetected = targetActor->RequestDetectionLevel(player, RE::DETECTION_PRIORITY::kNormal) > 0;
		float playerSkill = player->GetClampedActorValue(RE::ActorValue::kPickpocket);
		float targetSkill = targetActor->GetActorValue(RE::ActorValue::kPickpocket);

		return RE::AIFormulas::ComputePickpocketSuccess(playerSkill, targetSkill, stealValue, itemWeight, player, targetActor, isDetected, itemData.objDesc->object);
	}

	void ContainerMenuEx::Update()
	{
		Settings* settings = Settings::GetSingleton();

		ContainerMenuState newState = GetUpdatedState();

		if (settings->updateContainerMenuPickpocketChance && GetContainerMode() == RE::ContainerMenu::ContainerMode::kPickpocket)
		{
			if (lastState.selectedItemData == newState.selectedItemData)
			{
				bool pickpocketChanceUpdateRequired = (newState.pickpocketChance != lastState.pickpocketChance);

				if (pickpocketChanceUpdateRequired)
				{
					itemCard->obj.SetMember("pickpocketChance", newState.pickpocketChance);
					root.Invoke("UpdateItemCardInfo", nullptr, &itemCard->obj, 1);
				}
			}
		}

		if (settings->updateContainerMenuBottomBar)
		{
			bool meterUpdateRequired = Util::IsActorValueMeterUpdateNeeded(lastState.health, lastState.maxHealth, newState.health, newState.maxHealth, settings->bottomBarMeterUpdateSteps) ||
			                           Util::IsActorValueMeterUpdateNeeded(lastState.stamina, lastState.maxStamina, newState.stamina, newState.maxStamina, settings->bottomBarMeterUpdateSteps) ||
			                           Util::IsActorValueMeterUpdateNeeded(lastState.magicka, lastState.maxMagicka, newState.magicka, newState.maxMagicka, settings->bottomBarMeterUpdateSteps);

			if (meterUpdateRequired)
			{
				using func_t = void (*)(RE::ContainerMenu*);
				REL::Relocation<func_t> func(Offsets::Menus::ContainerMenu::UpdateBottomBar);
				func(this);
			}
		}

		lastState = newState;
	}

	RE::IMenu* ContainerMenuEx::Creator()
	{
		RE::ContainerMenu* menu = static_cast<RE::ContainerMenu*>(CreateMenu(RE::ContainerMenu::MENU_NAME));
		return menu;
	}

	void ContainerMenuEx::InstallHook()
	{
		REL::Relocation<std::uintptr_t> vTable(RE::VTABLE_ContainerMenu[0]);
		_ProcessMessage = vTable.write_vfunc(0x4, &ContainerMenuEx::ProcessMessage_Hook);
	}
}
