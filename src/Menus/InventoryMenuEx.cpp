#include "Menus/InventoryMenuEx.h"

namespace SkyrimSoulsRE
{
	std::string GetMessageTypeName(RE::UI_MESSAGE_TYPE a_type)
	{
		switch (a_type)
		{
		case RE::UI_MESSAGE_TYPE::kUpdate:
			return "kUpdate";
		case RE::UI_MESSAGE_TYPE::kShow:
			return "kShow";
		case RE::UI_MESSAGE_TYPE::kReshow:
			return "kReshow";
		case RE::UI_MESSAGE_TYPE::kHide:
			return "kHide";
		case RE::UI_MESSAGE_TYPE::kForceHide:
			return "kForceHide";
		case RE::UI_MESSAGE_TYPE::kScaleformEvent:
			return "kScaleformEvent";
		case RE::UI_MESSAGE_TYPE::kUserEvent:
			return "kUserEvent";
		case RE::UI_MESSAGE_TYPE::kInventoryUpdate:
			return "kInventoryUpdate";
		case RE::UI_MESSAGE_TYPE::kUserProfileChange:
			return "kUserProfileChange";
		case RE::UI_MESSAGE_TYPE::kMUStatusChange:
			return "kMUStatusChange";
		case RE::UI_MESSAGE_TYPE::kResumeCaching:
			return "kResumeCaching";
		case RE::UI_MESSAGE_TYPE::kUpdateController:
			return "kUpdateController";
		case RE::UI_MESSAGE_TYPE::kChatterEvent:
			return "kChatterEvent";
		}

		return "unknown";
	}

	RE::UI_MESSAGE_RESULTS InventoryMenuEx::ProcessMessage_Hook(RE::UIMessage& a_message)
	{
		RE::UI_MESSAGE_RESULTS result = _ProcessMessage(this, a_message);
		//SKSE::log::info("Received inventory message type: {}", GetMessageTypeName(a_message.type.get()));
		if (a_message.type == RE::UI_MESSAGE_TYPE::kHide)
		{
			HUDMenuEx* hudMenu = static_cast<HUDMenuEx*>(RE::UI::GetSingleton()->GetMenu(RE::InterfaceStrings::GetSingleton()->hudMenu).get());
			if (hudMenu)
			{
				hudMenu->SetSkyrimSoulsMode(false);
			}
		}
		else if (a_message.type == RE::UI_MESSAGE_TYPE::kUpdate)
		{
			this->UpdateBottomBar();
		}
		return result;
	}

	void InventoryMenuEx::UpdateBottomBar()
	{
		using func_t = decltype(&InventoryMenuEx::UpdateBottomBar);
		REL::Relocation<func_t> func(Offsets::Menus::InventoryMenu::UpdateBottomBar);
		return func(this);
	}

	RE::IMenu* InventoryMenuEx::Creator()
	{
		RE::InventoryMenu* menu = static_cast<RE::InventoryMenu*>(CreateMenu(RE::InventoryMenu::MENU_NAME));

		HUDMenuEx* hudMenu = static_cast<HUDMenuEx*>(RE::UI::GetSingleton()->GetMenu(RE::InterfaceStrings::GetSingleton()->hudMenu).get());
		if (hudMenu)
		{
			hudMenu->SetSkyrimSoulsMode(true);
		}
		return menu;
	}

	void InventoryMenuEx::InstallHook()
	{
		REL::Relocation<std::uintptr_t> vTable(RE::VTABLE_InventoryMenu[0]);
		_ProcessMessage = vTable.write_vfunc(0x4, &InventoryMenuEx::ProcessMessage_Hook);
	}
};
