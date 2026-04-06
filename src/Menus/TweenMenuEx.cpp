#include "Menus/TweenMenuEx.h"
#include "Util.h"

namespace SkyrimSoulsRE
{
	RE::UI_MESSAGE_RESULTS TweenMenuEx::ProcessMessage_Hook(RE::UIMessage& a_message)
	{
		switch (a_message.type.get())
		{
		case RE::UI_MESSAGE_TYPE::kShow:
			{
				lastState = GetUpdatedState();

				RE::GFxValue skyuiVersion;
				isSkyUI6 = this->uiMovie->GetVariable(&skyuiVersion, "_global.TweenMenu.SKYUI_VERSION_MAJOR") && skyuiVersion.IsNumber() && static_cast<int32_t>(skyuiVersion.GetNumber()) >= 6;
			}
			break;

		case RE::UI_MESSAGE_TYPE::kUpdate:
			Update();
			break;
		}

		return _ProcessMessage(this, a_message);
	}

	TweenMenuState TweenMenuEx::GetUpdatedState()
	{
		RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
		TweenMenuState state{};

		RE::Calendar::GetSingleton()->GetTimeDateString(state.timeDateString, sizeof(state.timeDateString), true);
		state.canLevelUp = player->skills->CanLevelUp() && !IsInSurvivalMode();
		state.xp = player->skills->data->xp;
		state.levelUpThreshold = player->skills->data->levelThreshold;
		state.level = player->GetLevel();

		return state;
	}

	void TweenMenuEx::Update()
	{
		Settings* settings = Settings::GetSingleton();
		if (!settings->updateTweenMenuBottomBar)
		{
			return;
		}

		TweenMenuState newState = GetUpdatedState();

		bool timeDateChanged = std::memcmp(lastState.timeDateString, newState.timeDateString, sizeof(TweenMenuState::timeDateString)) != 0;
		bool xpChanged = newState.canLevelUp != lastState.canLevelUp || newState.level != lastState.level || newState.xp != lastState.xp || newState.levelUpThreshold != lastState.levelUpThreshold;

		if (isSkyUI6)
		{
			if (timeDateChanged)
			{
				RE::FxResponseArgs<1> args;
				args.Add(newState.timeDateString);
				this->fxDelegate->Invoke(this->uiMovie.get(), "SetDateString", args);
			}

			if (xpChanged)
			{
				RE::FxResponseArgs<3> args;
				args.Add(newState.canLevelUp);
				args.Add(newState.level);
				args.Add((newState.xp / newState.levelUpThreshold) * 100.0f);
				this->fxDelegate->Invoke(this->uiMovie.get(), "SetPlayerInfo", args);
			}
		}
		else
		{
			if (timeDateChanged)
			{
				RE::GFxValue dateText;
				if (this->uiMovie->GetVariable(&dateText, "_root.TweenMenu_mc.BottomBarTweener_mc.BottomBar_mc.DateText"))
				{
					RE::GFxValue newDate(newState.timeDateString);
					dateText.SetMember("htmlText", newDate);
				}
			}

			if (newState.canLevelUp != lastState.canLevelUp)
			{
				RE::GFxValue canLevelUpText;
				if (this->uiMovie->GetVariable(&canLevelUpText, "_root.TweenMenu_mc.Selections_mc.SkillsText_mc.textField") &&
					canLevelUpText.GetType() != RE::GFxValue::ValueType::kUndefined)
				{
					if (newState.canLevelUp)
					{
						static std::wstring levelUpText = Util::TranslateUIString(this->uiMovie.get(), L"$LEVEL UP");
						canLevelUpText.SetMember("text", levelUpText.c_str());
					}
					else
					{
						static std::wstring skillsText = Util::TranslateUIString(this->uiMovie.get(), L"$SKILLS");
						canLevelUpText.SetMember("text", skillsText.c_str());
					}
				}
			}

			if (newState.level != lastState.level)
			{
				RE::GFxValue levelText;
				if (this->uiMovie->GetVariable(&levelText, "_root.TweenMenu_mc.BottomBarTweener_mc.BottomBar_mc.LevelNumberLabel") &&
					levelText.GetType() != RE::GFxValue::ValueType::kUndefined)
				{
					levelText.SetMember("text", newState.level);
				}
			}

			if (newState.xp != lastState.xp || newState.levelUpThreshold != lastState.levelUpThreshold)
			{
				RE::GFxValue xpMeter;
				if (this->uiMovie->GetVariable(&xpMeter, "_root.TweenMenu_mc.LevelMeter") &&
					xpMeter.GetType() != RE::GFxValue::ValueType::kUndefined)
				{
					std::array<RE::GFxValue, 1> xpArgs = { (newState.xp / newState.levelUpThreshold) * 100.0f };
					xpMeter.Invoke<1>("SetPercent", xpArgs);
				}
			}
		}

		lastState = newState;
	}

	RE::IMenu* TweenMenuEx::Creator()
	{
		RE::TweenMenu* menu = static_cast<RE::TweenMenu*>(CreateMenu(RE::TweenMenu::MENU_NAME));
		return menu;
	}

	void TweenMenuEx::InstallHook()
	{
		REL::Relocation<std::uintptr_t> vTable(RE::VTABLE_TweenMenu[0]);
		_ProcessMessage = vTable.write_vfunc(0x4, &TweenMenuEx::ProcessMessage_Hook);

		//Fix for camera movement
		std::uint8_t codes[] = { 0x90, 0x90, 0x90, 0x90, 0x90 };
		REL::safe_write(Offsets::Menus::TweenMenu::ProcessMessage.address() + 0x4F3, codes, sizeof(codes));
	}
}
