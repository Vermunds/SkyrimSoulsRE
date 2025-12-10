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

				this->uiMovie->SetVariable("_root.HUDMovieBaseInstance.StealthMeterInstance.SkyrimSoulsMode", &val);
				this->uiMovie->SetVariable("_root.HUDMovieBaseInstance.StealthMeterInstance.SneakTextHolder.SneakTextClip.SkyrimSoulsMode", &val);
				this->uiMovie->SetVariable("_root.HUDMovieBaseInstance.StealthMeterInstance.SneakTextHolder.SneakTextClip.SneakTextInstance.SkyrimSoulsMode", &val);
				this->uiMovie->SetVariable("_root.HUDMovieBaseInstance.QuestUpdateBaseInstance.SkyrimSoulsMode", &val);
				this->uiMovie->SetVariable("_root.HUDMovieBaseInstance.MessagesBlock.SkyrimSoulsMode", &val);
			}
			break;

		case SET_SKYRIMSOULS_HUD_MODE_MESSAGE_TYPE:
			SetSkyrimSoulsMode(static_cast<RE::BSUIMessageData*>(a_message.data)->data.b);
		}

		return _ProcessMessage(this, a_message);
	}

	void HUDMenuEx::SetSkyrimSoulsMode(bool a_isEnabled)
	{
		RE::InterfaceStrings* interfaceStrings = RE::InterfaceStrings::GetSingleton();
		RE::UIMessageQueue* msgQueue = RE::UIMessageQueue::GetSingleton();
		RE::MessageDataFactoryManager* msgFactory = RE::MessageDataFactoryManager::GetSingleton();
		Settings* settings = Settings::GetSingleton();

		auto creator = msgFactory->GetCreator(interfaceStrings->hudData);
		RE::HUDData* messageData = static_cast<RE::HUDData*>(creator->Create());
		messageData->type = RE::HUD_MESSAGE_TYPE::kSetMode;
		messageData->text = "SkyrimSoulsMode";
		messageData->show = a_isEnabled;

		msgQueue->AddMessage(interfaceStrings->hudMenu, RE::UI_MESSAGE_TYPE::kUpdate, messageData);

		if (!settings->disableHUDModifications)
		{
			RE::GFxValue StealthMeterInstance;
			RE::GFxValue::DisplayInfo displayInfo;
			if (!this->uiMovie->GetVariable(&StealthMeterInstance, "_root.HUDMovieBaseInstance.StealthMeterInstance"))
			{
				settings->disableHUDModifications = true;
				SKSE::log::error("Incompatible HUD Menu! Disabling HUD modifications for current session. Please report this so I can add support for the mods you're using.");
				return;
			}

			StealthMeterInstance.GetDisplayInfo(&displayInfo);

			if (a_isEnabled)
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
			this->uiMovie->SetVariable("_root.HUDMovieBaseInstance.StealthMeterInstance", &StealthMeterInstance);
		}
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
	}
}
