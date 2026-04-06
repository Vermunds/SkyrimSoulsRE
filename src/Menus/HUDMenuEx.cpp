#include "Menus/HUDMenuEx.h"

namespace SkyrimSoulsRE
{
	RE::UI_MESSAGE_RESULTS HUDMenuEx::ProcessMessage_Hook(RE::UIMessage& a_message)
	{
		switch (a_message.type.get())
		{
		case RE::UI_MESSAGE_TYPE::kShow:
			{
				RE::GFxValue val = true;

				this->uiMovie->SetVariable("_root.HUDMovieBaseInstance.StealthMeterInstance.InventoryMode", &val);
				this->uiMovie->SetVariable("_root.HUDMovieBaseInstance.StealthMeterInstance.SneakTextHolder.SneakTextClip.InventoryMode", &val);
				this->uiMovie->SetVariable("_root.HUDMovieBaseInstance.StealthMeterInstance.SneakTextHolder.SneakTextClip.SneakTextInstance.InventoryMode", &val);
			}
			break;
		}

		return _ProcessMessage(this, a_message);
	}

	bool HUDMenuEx::SetHudMode_Hook(RE::GFxValue::ObjectInterface* a_this, void* a_data, RE::GFxValue* a_result, const char* a_name, const RE::GFxValue* a_args, RE::UPInt a_numArgs, bool a_isDObj)
	{
		bool result = a_this->Invoke(a_data, a_result, a_name, a_args, a_numArgs, a_isDObj);

		RE::UI* ui = RE::UI::GetSingleton();
		RE::GPtr<RE::HUDMenu> hudMenu = ui->GetMenu<RE::HUDMenu>(RE::HUDMenu::MENU_NAME);
		if (!hudMenu)
		{
			SKSE::log::error("Failed to intercept SetMode in HUDMenu. Instance not found.");
			return result;
		}

		Settings* settings = Settings::GetSingleton();

		std::string mode = a_args[0].GetString();
		bool show = a_args[1].GetBool();

		if (!settings->disableHUDModifications && mode == "InventoryMode")
		{
			RE::GFxValue StealthMeterInstance;
			RE::GFxValue::DisplayInfo displayInfo;
			if (!hudMenu->uiMovie->GetVariable(&StealthMeterInstance, "_root.HUDMovieBaseInstance.StealthMeterInstance"))
			{
				settings->disableHUDModifications = true;
				SKSE::log::error("Incompatible HUD Menu! Disabling HUD modifications for current session.");
				return result;
			}

			StealthMeterInstance.GetDisplayInfo(&displayInfo);

			if (show)
			{
				HUDMenuEx::stealthMeterPosX = displayInfo.GetX();
				HUDMenuEx::stealthMeterPosY = displayInfo.GetY();

				displayInfo.SetX(settings->sneakMeterPosX);  // 24
				displayInfo.SetY(settings->sneakMeterPosY);  // 120
			}
			else
			{
				displayInfo.SetX(HUDMenuEx::stealthMeterPosX);
				displayInfo.SetY(HUDMenuEx::stealthMeterPosY);
			}

			StealthMeterInstance.SetDisplayInfo(displayInfo);
			hudMenu->uiMovie->SetVariable("_root.HUDMovieBaseInstance.StealthMeterInstance", &StealthMeterInstance);
		}

		return result;
	}

	RE::IMenu* HUDMenuEx::Creator()
	{
		RE::HUDMenu* menu = static_cast<RE::HUDMenu*>(CreateMenu(RE::HUDMenu::MENU_NAME));
		return menu;
	}

	void HUDMenuEx::InstallHook()
	{
		REL::Relocation<std::uintptr_t> vTable(RE::VTABLE_HUDMenu[0]);
		_ProcessMessage = vTable.write_vfunc(0x4, &HUDMenuEx::ProcessMessage_Hook);

		SKSE::Trampoline& trampoline = SKSE::GetTrampoline();
		trampoline.write_call<5>(Offsets::Menus::HUDMenu::ProcessMessage.address() + 0x990, (uintptr_t)SetHudMode_Hook);
	}
}
