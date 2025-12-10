#include "Menus/JournalMenuEx.h"

namespace SkyrimSoulsRE
{
	RE::UI_MESSAGE_RESULTS JournalMenuEx::ProcessMessage_Hook(RE::UIMessage& a_message)
	{
		switch (a_message.type.get())
		{
		case RE::UI_MESSAGE_TYPE::kUpdate:
			UpdateClock();
		}

		return _ProcessMessage(this, a_message);
	}

	void JournalMenuEx::UpdateClock()
	{
		char timeDateString[200];
		RE::Calendar::GetSingleton()->GetTimeDateString(timeDateString, 200, true);

		RE::GFxValue dateText;
		this->uiMovie->GetVariable(&dateText, "_root.QuestJournalFader.Menu_mc.BottomBar_mc.DateText");
		if (dateText.GetType() != RE::GFxValue::ValueType::kUndefined)
		{
			RE::GFxValue newDate(timeDateString);
			dateText.SetMember("htmlText", newDate);
		}
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
