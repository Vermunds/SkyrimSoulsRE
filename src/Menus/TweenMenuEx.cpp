#include "Menus/TweenMenuEx.h"
#include "Util.h"

namespace SkyrimSoulsRE
{
	RE::UI_MESSAGE_RESULTS TweenMenuEx::ProcessMessage_Hook(RE::UIMessage& a_message)
	{
		if (a_message.type == RE::UI_MESSAGE_TYPE::kUpdate)
		{
			UpdateInfo();
		}

		return _ProcessMessage(this, a_message);
	}

	TweenMenuState TweenMenuEx::GetUpdatedState()
	{
		using CanLevelUp_t = bool (RE::PlayerCharacter::PlayerSkills::*)();
		REL::Relocation<CanLevelUp_t> CanLevelUp(Offsets::PlayerCharacter::PlayerSkills::CanLevelUp);

		using GetXPAndLevelUpThreshold_t = void (RE::PlayerCharacter::PlayerSkills::*)(float&, float&);
		REL::Relocation<GetXPAndLevelUpThreshold_t> GetXPAndLevelUpThreshold(Offsets::PlayerCharacter::PlayerSkills::GetXPAndLevelUpThreshold);

		RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
		const auto IsInSurvivalMode = []() {
			const auto dobj = RE::BGSDefaultObjectManager::GetSingleton();
			const auto survival = dobj ? dobj->GetObject<RE::TESGlobal>(RE::DEFAULT_OBJECT::kSurvivalModeEnabled) : nullptr;
			return survival ? survival->value == 1.0F : false;
		};

		TweenMenuState state;

		RE::Calendar::GetSingleton()->GetTimeDateString(state.timeDateString, sizeof(state.timeDateString), false);
		state.canLevelUp = CanLevelUp(player->skills) && !IsInSurvivalMode();
		GetXPAndLevelUpThreshold(player->skills, state.xp, state.levelUpThreshold);

		state.level = player->GetLevel();

		return state;
	}

	void TweenMenuEx::UpdateInfo()
	{
		TweenMenuState currentState = GetUpdatedState();

		// Update clock
		if (std::memcmp(lastState.timeDateString, currentState.timeDateString, sizeof(TweenMenuState::timeDateString)) != 0)
		{
			std::memcpy(lastState.timeDateString, currentState.timeDateString, sizeof(TweenMenuState::timeDateString));

			RE::GFxValue dateText;
			this->uiMovie->GetVariable(&dateText, "_root.TweenMenu_mc.BottomBarTweener_mc.BottomBar_mc.DateText");

			if (dateText.GetType() != RE::GFxValue::ValueType::kUndefined)
			{
				RE::GFxValue newDate(currentState.timeDateString);
				dateText.SetMember("htmlText", newDate);
			}
		}

		// Update "can level up" (this doesn't fully update everything related to this but it seems to work)
		if (currentState.canLevelUp != lastState.canLevelUp)
		{
			lastState.canLevelUp = currentState.canLevelUp;

			RE::GFxValue canLevelUpText;
			this->uiMovie->GetVariable(&canLevelUpText, "_root.TweenMenu_mc.Selections_mc.SkillsText_mc.textField");

			if (canLevelUpText.GetType() != RE::GFxValue::ValueType::kUndefined)
			{
				if (currentState.canLevelUp)
				{
					static std::wstring levelUpText = Util::TranslateUIString(this->uiMovie.get(), L"$LEVEL UP");
					canLevelUpText.SetMember("text", levelUpText.c_str());
				}
				else
				{
					static std::wstring levelUpText = Util::TranslateUIString(this->uiMovie.get(), L"$SKILLS");
					canLevelUpText.SetMember("text", levelUpText.c_str());
				}
			}
		}

		// Update level
		if (currentState.level != lastState.level)
		{
			lastState.level = currentState.level;

			RE::GFxValue levelText;
			this->uiMovie->GetVariable(&levelText, "_root.TweenMenu_mc.BottomBarTweener_mc.BottomBar_mc.LevelNumberLabel");

			if (levelText.GetType() != RE::GFxValue::ValueType::kUndefined)
			{
				levelText.SetMember("text", currentState.level);
			}
		}

		// Update xp
		if (currentState.xp != lastState.xp)
		{
			lastState.xp = currentState.xp;

			RE::GFxValue xpMeter;
			this->uiMovie->GetVariable(&xpMeter, "_root.TweenMenu_mc.LevelMeter");

			// need to invoke SetPercent
			if (xpMeter.GetType() != RE::GFxValue::ValueType::kUndefined)
			{
				std::array<RE::GFxValue, 1> args = { (currentState.xp / currentState.levelUpThreshold) * 100.0f };
				xpMeter.Invoke<1>("SetPercent", args);
			}
		}
	}

	RE::IMenu* TweenMenuEx::Creator()
	{
		RE::TweenMenu* menu = static_cast<RE::TweenMenu*>(CreateMenu(RE::TweenMenu::MENU_NAME));
		lastState = TweenMenuState();
		return menu;
	}

	void TweenMenuEx::InstallHook()
	{
		//Hook AdvanceMovie
		REL::Relocation<std::uintptr_t> vTable(RE::VTABLE_TweenMenu[0]);
		_ProcessMessage = vTable.write_vfunc(0x4, &TweenMenuEx::ProcessMessage_Hook);

		//Fix for camera movement
		std::uint8_t codes[] = { 0x90, 0x90, 0x90, 0x90, 0x90 };
		REL::safe_write(Offsets::Menus::TweenMenu::ProcessMessage.address() + 0x4F3, codes, sizeof(codes));
	}
}
