#include "Menus/TweenMenuEx.h"

namespace SkyrimSoulsRE
{
	void TweenMenuEx::AdvanceMovie_Hook(float a_interval, uint32_t a_currentTime)
	{
		UpdateClock();
		return _AdvanceMovie(this, a_interval, a_currentTime);
	}

	//Tween menu clock
	void TweenMenuEx::UpdateClock()
	{
		char timeDateString[200];
		RE::Calendar::GetSingleton()->GetTimeDateString(timeDateString, 200, true);

		RE::GFxValue dateText;
		this->uiMovie->GetVariable(&dateText, "_root.TweenMenu_mc.BottomBarTweener_mc.BottomBar_mc.DateText");

		if (dateText.GetType() != RE::GFxValue::ValueType::kUndefined)
		{
			RE::GFxValue newDate(timeDateString);
			dateText.SetMember("htmlText", newDate);
		}
	}

	RE::IMenu* TweenMenuEx::Creator()
	{
		return CreateMenu(RE::TweenMenu::MENU_NAME);
	}

	void TweenMenuEx::InstallHook()
	{
		//Hook AdvanceMovie
		REL::Relocation<std::uintptr_t> vTable(Offsets::Menus::TweenMenu::Vtbl);
		_AdvanceMovie = vTable.write_vfunc(0x5, &TweenMenuEx::AdvanceMovie_Hook);

		//Fix for camera movement
		std::uint8_t codes[] = { 0x90, 0x90, 0x90, 0x90, 0x90 };
		REL::safe_write(Offsets::Menus::TweenMenu::Camera_Hook.address() + 0x4F3, codes, sizeof(codes));
	}
}
