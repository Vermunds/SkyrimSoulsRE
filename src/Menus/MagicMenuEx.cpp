#include "Menus/MagicMenuEx.h"

namespace SkyrimSoulsRE
{
	bool MagicMenuEx::IsViewingActiveEffects()
	{
		RE::GFxValue selectedIndex, categoryList, inventoryLists;

		bool success = this->root.GetMember("inventoryLists", &inventoryLists);
		if (!success)
			return false;

		success = inventoryLists.GetMember("categoryList", &categoryList);
		if (!success)
			return false;

		success = categoryList.GetMember("_selectedIndex", &selectedIndex);
		if (!success)
			return false;

		if (selectedIndex.GetNumber() == 9.0f)
		{
			return true;
		}
		return false;
	}

	RE::UI_MESSAGE_RESULTS MagicMenuEx::ProcessMessage_Hook(RE::UIMessage& a_message)
	{
		switch (a_message.type.get())
		{
		case RE::UI_MESSAGE_TYPE::kHide:
			{
				HUDMenuEx* hudMenu = static_cast<HUDMenuEx*>(RE::UI::GetSingleton()->GetMenu(RE::InterfaceStrings::GetSingleton()->hudMenu).get());
				if (hudMenu)
				{
					hudMenu->SetSkyrimSoulsMode(false);
				}
				break;
			}
		case RE::UI_MESSAGE_TYPE::kInventoryUpdate:
			{
				// for SkyUI only
				if (IsViewingActiveEffects())
				{
					RE::GFxValue listEnumeration, clipPool, entryClipManager, categoryList, inventoryLists;

					bool success = this->root.GetMember("inventoryLists", &inventoryLists);
					if (!success)
						break;

					success = inventoryLists.GetMember("categoryList", &categoryList);
					if (!success)
						break;

					success = categoryList.GetMember("_entryClipManager", &entryClipManager);
					if (!success)
						break;

					success = entryClipManager.GetMember("_clipPool", &clipPool);
					if (!success)
						break;

					success = categoryList.GetMember("listEnumeration", &listEnumeration);
					if (!success)
						break;

					assert(clipPool.GetType() == RE::GFxValue::ValueType::kArray);

					std::int32_t count = clipPool.GetArraySize();

					std::vector<bool> listEnableState(count);
					std::vector<double> listAlphaState(count);

					for (std::int32_t i = 0; i < count; ++i)
					{
						RE::GFxValue entry, value;
						if (clipPool.GetElement(i, &entry))
						{
							entry.GetMember("enabled", &value);
							listEnableState[i] = value.GetBool();

							entry.GetMember("_alpha", &value);
							listAlphaState[i] = value.GetNumber();
						}
					}

					blockSpells = true;
					RE::UI_MESSAGE_RESULTS result = _ProcessMessage(this, a_message);
					blockSpells = false;

					for (std::int32_t i = 0; i < count; ++i)
					{
						RE::GFxValue entry, value;

						if (clipPool.GetElement(i, &entry))
						{
							value = RE::GFxValue{ listEnableState[i] };
							entry.SetMember("enabled", value);

							value = RE::GFxValue{ listAlphaState[i] };
							entry.SetMember("_alpha", value);
						}

						RE::GFxValue index{ i };
						if (listEnumeration.Invoke("at", &entry, &index, 1) && listEnableState[i])
						{
							entry.SetMember("filterFlag", 1);
						}
					}

					return result;
				}
			}
		}

		return _ProcessMessage(this, a_message);
	}

	// Called during ProcessMessage
	// Prevent spells from being added to the itemlist, while viewing active effects for performance reasons
	void MagicMenuEx::AddSpells_Hook(RE::PlayerCharacter* a_player, void* a_unk1)
	{
		if (blockSpells)
		{
			return;
		}

		using func_t = decltype(&AddSpells_Hook);
		REL::Relocation<func_t> func(Offsets::Menus::MagicMenu::AddSpells);
		return func(a_player, a_unk1);
	}

	// Called during ProcessMessage
	// Prevent shouts from being added to the itemlist, while viewing active effects for performance reasons
	void MagicMenuEx::AddShouts_Hook(RE::ItemList* a_list, void* a_unk1, RE::RefHandle a_playerRefHandle)
	{
		if (blockSpells)
		{
			return;
		}

		using func_t = decltype(&AddShouts_Hook);
		REL::Relocation<func_t> func(Offsets::Menus::MagicMenu::AddShout);
		return func(a_list, a_unk1, a_playerRefHandle);
	}

	void MagicMenuEx::AdvanceMovie_Hook(float a_interval, std::uint32_t a_currentTime)
	{
		if (IsViewingActiveEffects())
		{
			// Update the list without the spells included
			wasViewingActiveEffects = true;
			reinterpret_cast<RE::ItemList*>(this->unk30)->Update();
		}
		else if (wasViewingActiveEffects)
		{
			// Update the list with the spells included
			wasViewingActiveEffects = false;
			reinterpret_cast<RE::ItemList*>(this->unk30)->Update();
		}
		else
		{
			this->UpdateBottomBar();
		}

		return _AdvanceMovie(this, a_interval, a_currentTime);
	}

	void MagicMenuEx::UpdateBottomBar()
	{
		using func_t = decltype(&MagicMenuEx::UpdateBottomBar);
		REL::Relocation<func_t> func(Offsets::Menus::MagicMenu::UpdateBottomBar);
		return func(this);
	}

	RE::IMenu* MagicMenuEx::Creator()
	{
		HUDMenuEx* hudMenu = static_cast<HUDMenuEx*>(RE::UI::GetSingleton()->GetMenu(RE::InterfaceStrings::GetSingleton()->hudMenu).get());
		if (hudMenu)
		{
			hudMenu->SetSkyrimSoulsMode(true);
		}

		return CreateMenu(RE::MagicMenu::MENU_NAME);
	}

	void MagicMenuEx::InstallHook()
	{
		//Hook AdvanceMovie
		REL::Relocation<std::uintptr_t> vTable(RE::VTABLE_MagicMenu[0]);
		_ProcessMessage = vTable.write_vfunc(0x4, &MagicMenuEx::ProcessMessage_Hook);
		_AdvanceMovie = vTable.write_vfunc(0x5, &MagicMenuEx::AdvanceMovie_Hook);

		SKSE::GetTrampoline().write_call<5>(Offsets::Menus::MagicMenu::UpdateItemList.address() + 0x53, AddSpells_Hook);
		SKSE::GetTrampoline().write_call<5>(Offsets::Menus::MagicMenu::UpdateItemList.address() + 0x9A, AddShouts_Hook);
	}
}
