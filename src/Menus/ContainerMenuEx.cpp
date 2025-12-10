#include "Menus/ContainerMenuEx.h"
#include "Util.h"

namespace SkyrimSoulsRE
{
	RE::UI_MESSAGE_RESULTS ContainerMenuEx::ProcessMessage_Hook(RE::UIMessage& a_message)
	{
		AutoCloseManager* autoCloseManager = AutoCloseManager::GetSingleton();

		switch (a_message.type.get())
		{
		case RE::UI_MESSAGE_TYPE::kShow:
			{
				SendSetSkyrimSoulsHUDModeMessage(true);

				bool checkForDeath = GetContainerMode() == RE::ContainerMenu::ContainerMode::kPickpocket;
				autoCloseManager->InitAutoClose(RE::ContainerMenu::MENU_NAME, GetTargetRefHandle(), checkForDeath);
			}
			break;

		case RE::UI_MESSAGE_TYPE::kUpdate:
			{
				UpdateBottomBar();
				if (GetContainerMode() == RE::ContainerMenu::ContainerMode::kPickpocket)
				{
					UpdatePickpocketChance();
				}

				autoCloseManager->CheckAutoClose(RE::ContainerMenu::MENU_NAME);
			}
			break;

		case RE::UI_MESSAGE_TYPE::kHide:
			SendSetSkyrimSoulsHUDModeMessage(false);
			break;
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

				static std::wstring toSteal = Util::TranslateUIString(this->uiMovie.get(), L"$ TO STEAL");
				static std::wstring toPlace = Util::TranslateUIString(this->uiMovie.get(), L"$ TO PLACE");

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
		return menu;
	}

	void ContainerMenuEx::InstallHook()
	{
		REL::Relocation<std::uintptr_t> vTable(RE::VTABLE_ContainerMenu[0]);
		_ProcessMessage = vTable.write_vfunc(0x4, &ContainerMenuEx::ProcessMessage_Hook);
	}
}
