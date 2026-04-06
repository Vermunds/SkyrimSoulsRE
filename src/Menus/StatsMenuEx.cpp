#include "Menus/StatsMenuEx.h"
#include "Util.h"

#include <xbyak/xbyak.h>

namespace SkyrimSoulsRE
{
	RE::UI_MESSAGE_RESULTS StatsMenuEx::ProcessMessage_Hook(RE::UIMessage& a_message)
	{
		switch (a_message.type.get())
		{
		case RE::UI_MESSAGE_TYPE::kShow:
			lastState = GetUpdatedState();
			break;
		case RE::UI_MESSAGE_TYPE::kUpdate:
			RE::UIMessageQueue::GetSingleton()->AddMessage(RE::HUDMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kUpdate, nullptr);
			Update();
			break;

		case RE::UI_MESSAGE_TYPE::kHide:

			isSleeping = false;
			break;
		}

		return _ProcessMessage(this, a_message);
	}

	StatsMenuState StatsMenuEx::GetUpdatedState()
	{
		RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
		StatsMenuState state{};

		state.health = player->GetActorValue(RE::ActorValue::kHealth);
		state.maxHealth = state.health - player->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIER::kDamage, RE::ActorValue::kHealth);
		state.stamina = player->GetActorValue(RE::ActorValue::kStamina);
		state.maxStamina = state.stamina - player->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIER::kDamage, RE::ActorValue::kStamina);
		state.magicka = player->GetActorValue(RE::ActorValue::kMagicka);
		state.maxMagicka = state.magicka - player->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIER::kDamage, RE::ActorValue::kMagicka);

		state.playerData = *player->skills->data;  // Should be trivially copyable

		// Name and race should already be updated by the game and not really change anyways

		return state;
	}

	void StatsMenuEx::Update()
	{
		Settings* settings = Settings::GetSingleton();

		StatsMenuState newState = GetUpdatedState();

		if (settings->updateStatsMenuPlayerInfo)
		{
			bool updatePlayerInfoRequired =
				Util::IsActorValueMeterUpdateNeeded(lastState.health, lastState.maxHealth, newState.health, newState.maxHealth, settings->bottomBarMeterUpdateSteps) ||
				Util::IsActorValueMeterUpdateNeeded(lastState.stamina, lastState.maxStamina, newState.stamina, newState.maxStamina, settings->bottomBarMeterUpdateSteps) ||
				Util::IsActorValueMeterUpdateNeeded(lastState.magicka, lastState.maxMagicka, newState.magicka, newState.maxMagicka, settings->bottomBarMeterUpdateSteps) ||
				lastState.playerData.xp != newState.playerData.xp ||
				lastState.playerData.levelThreshold != newState.playerData.levelThreshold;

			if (updatePlayerInfoRequired)
			{
				using func_t = void (*)(RE::StatsMenu*);
				static REL::Relocation<func_t> SetPlayerInfo(Offsets::Menus::StatsMenu::SetPlayerInfo);
				SetPlayerInfo(this);
			}
		}

		if (settings->updateStatsMenuSkillList)
		{
			bool updateSkillListRequired = false;
			for (std::uint32_t i = 0; i < RE::PlayerCharacter::PlayerSkills::Data::Skill::kTotal; ++i)
			{
				const RE::PlayerCharacter::PlayerSkills::Data::SkillData& lastSkill = lastState.playerData.skills[i];
				const RE::PlayerCharacter::PlayerSkills::Data::SkillData& newSkill = newState.playerData.skills[i];

				if (lastSkill.level != newSkill.level || lastSkill.xp != newSkill.xp || lastSkill.levelThreshold != newSkill.levelThreshold || lastState.playerData.legendaryLevels[i] != newState.playerData.legendaryLevels[i])
				{
					updateSkillListRequired = true;
					break;
				}
			}

			if (updateSkillListRequired)
			{
				using func_t = void (*)(RE::StatsMenu*);
				static REL::Relocation<func_t> UpdateSkillList(Offsets::Menus::StatsMenu::UpdateSkillList);
				UpdateSkillList(this);
			}
		}

		lastState = newState;
	}

	RE::IMenu* StatsMenuEx::Creator()
	{
		StatsMenuEx* menu = static_cast<StatsMenuEx*>(CreateMenu(RE::StatsMenu::MENU_NAME));
		return menu;
	}

	void OpenStatsMenuAfterSleep_Hook(RE::UIMessageQueue* a_queue, const RE::BSFixedString& a_menuName, RE::UI_MESSAGE_TYPE a_type, RE::IUIMessageData* a_data)
	{
		RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
		if (player->playerFlags.sleeping)
		{
			StatsMenuEx::isSleeping = true;
		}

		return a_queue->AddMessage(a_menuName, a_type, a_data);
	}

	void StatsMenuEx::InstallHook()
	{
		REL::Relocation<std::uintptr_t> vTable(RE::VTABLE_StatsMenu[0]);
		_ProcessMessage = vTable.write_vfunc(0x4, &StatsMenuEx::ProcessMessage_Hook);

		SKSE::Trampoline& trampoline = SKSE::GetTrampoline();

		// Fix level up when sleeping using survival mode
		// We replace a tail-call jump with a normal call, so the rest of the function can execute as well (this will remove the sleeping flag).
		REL::safe_write(Offsets::Menus::StatsMenu::OpenStatsMenuAfterSleep_Hook.address() + 0x65, std::uint32_t(0x90909090));
		REL::safe_write(Offsets::Menus::StatsMenu::OpenStatsMenuAfterSleep_Hook.address() + 0x69, std::uint8_t(0x90));
		trampoline.write_call<5>(Offsets::Menus::StatsMenu::OpenStatsMenuAfterSleep_Hook.address() + 0x6A, OpenStatsMenuAfterSleep_Hook);

		// Make StatsMenu check our sleeping variable
		struct SleepCheck_Code : Xbyak::CodeGenerator
		{
			SleepCheck_Code(uintptr_t a_trueAddress, uintptr_t a_falseAddress)
			{
				Xbyak::Label trueAddress;
				Xbyak::Label falseAddress;
				Xbyak::Label falseLabel;

				mov(rcx, std::uintptr_t(&StatsMenuEx::isSleeping));
				mov(cl, byte[rcx]);
				cmp(cl, 1);
				jne(falseLabel);
				jmp(ptr[rip + trueAddress]);

				L(falseLabel);
				jmp(ptr[rip + falseAddress]);

				L(trueAddress);
				dq(a_trueAddress);

				L(falseAddress);
				dq(a_falseAddress);
			}
		};

		SleepCheck_Code code{ std::uintptr_t(Offsets::Menus::StatsMenu::ProcessMessage.address() + 0xFC9), std::uintptr_t(Offsets::Menus::StatsMenu::ProcessMessage.address() + 0x102D) };
		void* codeLoc = SKSE::GetTrampoline().allocate(code);
		trampoline.write_branch<5>(Offsets::Menus::StatsMenu::ProcessMessage.address() + 0xFC0, codeLoc);

		// Prevent setting kFreezeFrameBackground flag
		REL::safe_write(Offsets::Menus::StatsMenu::ProcessMessage.address() + 0xA10, std::uint32_t(0x90909090));

		// Fix for controls not working
		REL::safe_write(Offsets::Menus::StatsMenu::CanProcess.address() + 0x46, std::uint32_t(0x90909090));
		REL::safe_write(Offsets::Menus::StatsMenu::CanProcess.address() + 0x4A, std::uint16_t(0x9090));
	}
}
