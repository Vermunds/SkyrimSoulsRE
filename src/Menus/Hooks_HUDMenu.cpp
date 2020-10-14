#include "Menus/Hooks_HUDMenu.h"

namespace SkyrimSoulsRE
{
	void HUDMenuEx::SetStealPickpocketHUDMode(bool a_isEnabled)
	{
		RE::InterfaceStrings* interfaceStrings = RE::InterfaceStrings::GetSingleton();
		RE::UIMessageQueue* msgQueue = RE::UIMessageQueue::GetSingleton();
		RE::MessageDataFactoryManager* msgFactory = RE::MessageDataFactoryManager::GetSingleton();
		Settings* settings = Settings::GetSingleton();

		auto creator = msgFactory->GetCreator(interfaceStrings->hudData);
		RE::HUDData* messageData = static_cast<RE::HUDData*>(creator->Create());
		messageData->type = static_cast<RE::HUDData::Type>(23);
		messageData->text = "StealPickpocketMode";
		messageData->unk40 = a_isEnabled;

		msgQueue->AddMessage(interfaceStrings->hudMenu, RE::UI_MESSAGE_TYPE::kUpdate, messageData);

		RE::GFxValue StealthMeterInstance;
		RE::GFxValue::DisplayInfo displayInfo;
		this->uiMovie->GetVariable(&StealthMeterInstance, "_root.HUDMovieBaseInstance.StealthMeterInstance");
		StealthMeterInstance.GetDisplayInfo(&displayInfo);
		
		if (a_isEnabled)
		{
			HUDMenuEx::stealthMeterPosX = displayInfo.GetX();
			HUDMenuEx::stealthMeterPosY = displayInfo.GetY();

			displayInfo.SetX(settings->sneakMeterPosX); // 24
			displayInfo.SetY(settings->sneakMeterPosY); // 120
		}
		else
		{
			displayInfo.SetX(HUDMenuEx::stealthMeterPosX);
			displayInfo.SetY(HUDMenuEx::stealthMeterPosY);
		}

		StealthMeterInstance.SetDisplayInfo(displayInfo);
		this->uiMovie->SetVariable("_root.HUDMovieBaseInstance.StealthMeterInstance", &StealthMeterInstance);
	}

	RE::IMenu* HUDMenuEx::Creator()
	{
		RE::HUDMenu* menu = static_cast<RE::HUDMenu*>(CreateMenu(RE::HUDMenu::MENU_NAME));

		//MessagesBlock.InventoryMode = true;
		//QuestUpdateBaseInstance.InventoryMode = true;

		RE::GFxValue val = true;
		menu->uiMovie->SetVariable("_root.HUDMovieBaseInstance.StealthMeterInstance.StealPickpocketMode", &val);
		menu->uiMovie->SetVariable("_root.HUDMovieBaseInstance.StealthMeterInstance.SneakTextHolder.SneakTextClip.StealPickpocketMode", &val);
		menu->uiMovie->SetVariable("_root.HUDMovieBaseInstance.StealthMeterInstance.SneakTextHolder.SneakTextClip.SneakTextInstance.StealPickpocketMode", &val);
		menu->uiMovie->SetVariable("_root.HUDMovieBaseInstance.QuestUpdateBaseInstance.StealPickpocketMode", &val);
		menu->uiMovie->SetVariable("_root.HUDMovieBaseInstance.MessagesBlock.StealPickpocketMode", &val);

		return menu;
	}

	void HUDMenuEx::InstallHook()
	{
	}
}
