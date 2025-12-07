#include "Menus/ContainerMenuEx.h"

namespace SkyrimSoulsRE
{
	RE::UI_MESSAGE_RESULTS ContainerMenuEx::ProcessMessage_Hook(RE::UIMessage& a_message)
	{
		if (a_message.type == RE::UI_MESSAGE_TYPE::kHide)
		{
			HUDMenuEx* hudMenu = static_cast<HUDMenuEx*>(RE::UI::GetSingleton()->GetMenu(RE::HUDMenu::MENU_NAME).get());
			if (hudMenu)
			{
				hudMenu->SetSkyrimSoulsMode(false);
			}
		}
		else if (a_message.type == RE::UI_MESSAGE_TYPE::kUpdate)
		{
			this->UpdateBottomBar();
			if (this->GetContainerMode() == RE::ContainerMenu::ContainerMode::kPickpocket)
			{
				this->UpdatePickpocketChance();
			}

			AutoCloseManager::GetSingleton()->CheckAutoClose(RE::ContainerMenu::MENU_NAME);
		}
		return _ProcessMessage(this, a_message);
	}

	std::int32_t ContainerMenuEx::CalcPickPocketChance(RE::StandardItemData* a_itemData)
	{
		RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
		RE::TESObjectREFRPtr containerRef = RE::TESObjectREFR::LookupByHandle(GetTargetRefHandle());

		if (!containerRef)
		{
			return -1;
		}

		RE::Actor* targetActor = static_cast<RE::Actor*>(containerRef.get());

		float itemWeight = a_itemData->objDesc->GetWeight();
		std::uint32_t count = 1;

		if (a_itemData->objDesc->object->IsAmmo() || a_itemData->objDesc->object->IsLockpick() || a_itemData->objDesc->object->IsGold())
		{
			count = a_itemData->GetCount();
			itemWeight = -1.0f;
		}

		std::uint32_t stealValue = targetActor->GetStealValue(a_itemData->objDesc, count, true);

		bool isDetected = targetActor->RequestDetectionLevel(player, RE::DETECTION_PRIORITY::kNormal) > 0;
		float playerSkill = player->GetActorValue(RE::ActorValue::kPickpocket);
		float targetSkill = targetActor->GetActorValue(RE::ActorValue::kPickpocket);

		auto chance = RE::AIFormulas::ComputePickpocketSuccess(playerSkill, targetSkill, stealValue, itemWeight, player, targetActor, isDetected, a_itemData->objDesc->object);

		if (chance > 100)
		{
			chance = 100;
		}
		else if (chance < 0)
		{
			chance = 0;
		}

		return chance;
	}

	void ContainerMenuEx::UpdatePickpocketChance()
	{
		RE::TESObjectREFRPtr containerRef = RE::TESObjectREFR::LookupByHandle(GetTargetRefHandle());

		if (containerRef && containerRef->formType == RE::FormType::ActorCharacter)
		{
			RE::ItemList::Item* item = this->itemList->GetSelectedItem();
			if (item)
			{
				std::int32_t chance = CalcPickPocketChance(&item->data);

				RE::GFxValue isViewingContainer;

				if (!this->uiMovie->Invoke("_root.Menu_mc.isViewingContainer", &isViewingContainer, nullptr, 0))
					return;

				std::wstring desc;
				desc = isViewingContainer.GetBool() ? toSteal : toPlace;

				std::wstring stealText(L"<font face=\'$EverywhereBoldFont\' size=\'24\' color=\'#FFFFFF\'>" + std::to_wstring(chance) + L"%</font>" + desc);

				RE::GFxValue stealTextObj;
				RE::GFxValue newText(stealText.c_str());
				if (this->uiMovie->GetVariable(&stealTextObj, "_root.Menu_mc.itemCardFadeHolder.StealTextInstance.PercentTextInstance"))
				{
					//SkyUI
					stealTextObj.SetMember("htmlText", newText);
				}
				else if (this->uiMovie->GetVariable(&stealTextObj, "_root.Menu_mc.ItemCardFadeHolder_mc.StealTextInstance.PercentTextInstance"))
				{
					//vanilla
					stealTextObj.SetMember("htmlText", newText);
				}
			}
		}
	}

	void ContainerMenuEx::UpdateBottomBar()
	{
		using func_t = decltype(&ContainerMenuEx::UpdateBottomBar);
		REL::Relocation<func_t> func(Offsets::Menus::ContainerMenu::UpdateBottomBar);
		return func(this);
	}

	RE::IMenu* ContainerMenuEx::Creator()
	{
		RE::ContainerMenu* menu = static_cast<RE::ContainerMenu*>(CreateMenu(RE::ContainerMenu::MENU_NAME));

		HUDMenuEx* hudMenu = static_cast<HUDMenuEx*>(RE::UI::GetSingleton()->GetMenu(RE::HUDMenu::MENU_NAME).get());
		if (hudMenu)
		{
			hudMenu->SetSkyrimSoulsMode(true);
		}

		AutoCloseManager* autoCloseManager = AutoCloseManager::GetSingleton();
		bool checkForDeath = menu->GetContainerMode() == RE::ContainerMenu::ContainerMode::kPickpocket;
		autoCloseManager->InitAutoClose(RE::ContainerMenu::MENU_NAME, menu->GetTargetRefHandle(), checkForDeath);

		return menu;
	}

	void ContainerMenuEx::InstallHook()
	{
		REL::Relocation<std::uintptr_t> vTable(RE::VTABLE_ContainerMenu[0]);
		_ProcessMessage = vTable.write_vfunc(0x4, &ContainerMenuEx::ProcessMessage_Hook);
	}

	void ContainerMenuEx::ParseTranslations()
	{
		bool foundToSteal = false;
		bool foundToPlace = false;

		RE::Setting* language = RE::INISettingCollection::GetSingleton()->GetSetting("sLanguage:General");
		std::string path = "Interface\\";

		// Construct translation filename
		path += "Translate_";
		path += (language && language->GetType() == RE::Setting::Type::kString) ? language->data.s : "ENGLISH";
		path += ".txt";

		SKSE::log::info("Reading translations from {}", path);

		RE::BSResourceNiBinaryStream fileStream(path.c_str());
		if (!fileStream.good())
		{
			SKSE::log::error("Failed to read file {}. Aborting.", path);
			return;
		}

		wchar_t bom = 0;
		bool ret = fileStream.read(&bom, 1);
		if (!ret)
		{
			SKSE::log::error("Empty translation file. Aborting.");
			return;
		}

		if (bom != L'\xFEFF')
		{
			SKSE::log::error("BOM Error, file must be encoded in UCS-2 LE. Aborting.");
			return;
		}

		while (!(foundToSteal && foundToPlace))
		{
			std::wstring str;

			bool notEOF = std::getline(fileStream, str);
			if (!notEOF)  // End of file
			{
				SKSE::log::error("Unexpected end of file.");
				break;
			}

			std::size_t len = str.length();

			wchar_t last = str.at(len - 1);
			if (last == '\r')
				len--;

			std::size_t delimIdx = 0;
			for (std::size_t i = 0; i < len; ++i)
			{
				if (str.at(i) == L'\t')
				{
					delimIdx = i;
					break;
				}
			}

			if (delimIdx == 0)
				continue;

			std::wstring key = std::wstring{ str.substr(0, delimIdx) };
			std::wstring translation = std::wstring{ str.substr(delimIdx + 1, len - delimIdx - 1) };

			if (key == L"$ TO PLACE")
			{
				foundToPlace = true;
				toPlace = translation;
			}
			else if (key == L"$ TO STEAL")
			{
				foundToSteal = true;
				toSteal = translation;
			}
		}

		if (!foundToPlace)
		{
			SKSE::log::error("Failed to find translation for \"$ TO PLACE\".");
		}

		if (!foundToSteal)
		{
			SKSE::log::error("Failed to find translation for \"$ TO STEAL\".");
		}

		SKSE::log::info("Reading translations finished.");
	}
}
