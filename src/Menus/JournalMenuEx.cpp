#include "Menus/JournalMenuEx.h"

namespace SkyrimSoulsRE
{
	RE::UI_MESSAGE_RESULTS JournalMenuEx::ProcessMessage_Hook(RE::UIMessage& a_message)
	{
		RE::UI_MESSAGE_RESULTS result = _ProcessMessage(this, a_message);

		switch (a_message.type.get())
		{
		case RE::UI_MESSAGE_TYPE::kShow:
			lastState = GetUpdatedState();
			break;

		case RE::UI_MESSAGE_TYPE::kUpdate:
			RE::UIMessageQueue::GetSingleton()->AddMessage(RE::HUDMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kUpdate, nullptr);
			Update();
			break;
		}

		return result;
	}

	JournalMenuState JournalMenuEx::GetUpdatedState()
	{
		JournalMenuState state{};
		RE::Calendar::GetSingleton()->GetTimeDateString(state.lastTimeDateString, sizeof(state.lastTimeDateString), true);

		RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
		state.level = static_cast<float>(player->GetLevel());
		state.xp = player->skills->data->xp;
		state.levelUpThreshold = player->skills->data->levelThreshold;

		return state;
	}

	void JournalMenuEx::Update()
	{
		Settings* settings = Settings::GetSingleton();
		if (!settings->updateJournalMenuBottomBar)
		{
			return;
		}

		JournalMenuState newState = GetUpdatedState();

		bool timeDateChanged = std::memcmp(lastState.lastTimeDateString, newState.lastTimeDateString, sizeof(JournalMenuState::lastTimeDateString)) != 0;
		bool xpChanged = newState.level != lastState.level || newState.xp != lastState.xp || newState.levelUpThreshold != lastState.levelUpThreshold;

		if (timeDateChanged || xpChanged)
		{
			float xpPercent = (newState.xp / newState.levelUpThreshold) * 100.0f;

			RE::GFxValue args[3];
			args[0].SetString(newState.lastTimeDateString);
			args[1].SetNumber(newState.level);
			args[2].SetNumber(xpPercent);
			this->uiMovie->Invoke("_root.QuestJournalFader.Menu_mc.BottomBar_mc.SetPlayerInfo", nullptr, args, 3);
		}

		lastState = newState;
	}

	RE::IMenu* JournalMenuEx::Creator()
	{
		RE::JournalMenu* menu = static_cast<RE::JournalMenu*>(CreateMenu(RE::JournalMenu::MENU_NAME.data()));
		return menu;
	}

	void JournalMenuEx::InstallHook()
	{
		REL::Relocation<std::uintptr_t> vTable(RE::VTABLE_JournalMenu[0]);
		_ProcessMessage = vTable.write_vfunc(0x4, &JournalMenuEx::ProcessMessage_Hook);

		// Fix save screenshots
		REL::safe_write(Offsets::Main::Render.address() + 0x5CA, std::uint16_t(0x9090));
	}
};
