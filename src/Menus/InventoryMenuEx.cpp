#include "Menus/InventoryMenuEx.h"
#include "Util.h"

namespace SkyrimSoulsRE
{
	RE::UI_MESSAGE_RESULTS InventoryMenuEx::ProcessMessage_Hook(RE::UIMessage& a_message)
	{
		switch (a_message.type.get())
		{
		case RE::UI_MESSAGE_TYPE::kShow:
			lastState = GetUpdatedState();
			break;

		case RE::UI_MESSAGE_TYPE::kUpdate:
			Update();
			break;
		}

		return _ProcessMessage(this, a_message);
	}

	InventoryMenuState InventoryMenuEx::GetUpdatedState()
	{
		RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
		InventoryMenuState state{};
		state.health = player->GetActorValue(RE::ActorValue::kHealth);
		state.maxHealth = state.health - player->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIER::kDamage, RE::ActorValue::kHealth);
		state.stamina = player->GetActorValue(RE::ActorValue::kStamina);
		state.maxStamina = state.stamina - player->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIER::kDamage, RE::ActorValue::kStamina);
		state.magicka = player->GetActorValue(RE::ActorValue::kMagicka);
		state.maxMagicka = state.magicka - player->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIER::kDamage, RE::ActorValue::kMagicka);

		return state;
	}

	void InventoryMenuEx::Update()
	{
		Settings* settings = Settings::GetSingleton();

		InventoryMenuState newState = GetUpdatedState();

		if (settings->updateInventoryMenuBottomBar)
		{
			bool meterUpdateRequired = Util::IsActorValueMeterUpdateNeeded(lastState.health, lastState.maxHealth, newState.health, newState.maxHealth, settings->bottomBarMeterUpdateSteps) ||
			                           Util::IsActorValueMeterUpdateNeeded(lastState.stamina, lastState.maxStamina, newState.stamina, newState.maxStamina, settings->bottomBarMeterUpdateSteps) ||
			                           Util::IsActorValueMeterUpdateNeeded(lastState.magicka, lastState.maxMagicka, newState.magicka, newState.maxMagicka, settings->bottomBarMeterUpdateSteps);

			if (meterUpdateRequired)
			{
				using func_t = void (*)(RE::InventoryMenu*);
				REL::Relocation<func_t> func(Offsets::Menus::InventoryMenu::UpdateBottomBar);
				func(this);
			}
		}

		lastState = newState;
	}

	RE::IMenu* InventoryMenuEx::Creator()
	{
		RE::InventoryMenu* menu = static_cast<RE::InventoryMenu*>(CreateMenu(RE::InventoryMenu::MENU_NAME));
		return menu;
	}

	void InventoryMenuEx::InstallHook()
	{
		REL::Relocation<std::uintptr_t> vTable(RE::VTABLE_InventoryMenu[0]);
		_ProcessMessage = vTable.write_vfunc(0x4, &InventoryMenuEx::ProcessMessage_Hook);
	}
};
