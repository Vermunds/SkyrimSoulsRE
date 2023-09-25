#include "Menus/MapMenuEx.h"

namespace SkyrimSoulsRE
{
	RE::UI_MESSAGE_RESULTS MapMenuEx::ProcessMessage_Hook(RE::UIMessage& a_message)
	{
		return _ProcessMessage(this, a_message);
	}

	void MapMenuEx::AdvanceMovie_Hook(float a_interval, std::uint32_t a_currentTime)
	{
		auto task = [a_interval, a_currentTime]() {
			RE::UI* ui = RE::UI::GetSingleton();

			if (ui->IsMenuOpen(RE::MapMenu::MENU_NAME))
			{
				MapMenuEx* menu = static_cast<MapMenuEx*>(ui->GetMenu(RE::MapMenu::MENU_NAME).get());

				menu->UpdateClock();
				menu->_AdvanceMovie(menu, a_interval, a_currentTime);
			}
		};

		UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
		queue->AddTask(task);
	}

	void MapMenuEx::UpdateClock()
	{
		char timeDateString[200];
		RE::Calendar::GetSingleton()->GetTimeDateString(timeDateString, 200, true);

		RE::GFxValue dateText;
		if (this->uiMovie->GetVariable(&dateText, "_root.bottomBar.DateText"))  //SkyUI
		{
			RE::GFxValue newDate(timeDateString);
			dateText.SetMember("htmlText", newDate);
		}
		else if (this->uiMovie->GetVariable(&dateText, "_root.Bottom.DateText"))  // non-SkyUI
		{
			RE::GFxValue newDate(timeDateString);
			dateText.SetMember("htmlText", newDate);
		}
	}

	RE::IMenu* MapMenuEx::Creator()
	{
		RE::MapMenu* menu = static_cast<RE::MapMenu*>(CreateMenu(RE::MapMenu::MENU_NAME));
		return menu;
	}

	void MapMenuEx::BGSTerrainManager_Update_Hook(RE::BGSTerrainManager* a_this, std::uint64_t a_unk1, std::uint64_t a_unk2)
	{
		RE::UI* ui = RE::UI::GetSingleton();

		if (!ui->IsMenuOpen(RE::MapMenu::MENU_NAME))
		{
			_TerrainManagerUpdate(a_this, a_unk1, a_unk2);
		}
	}

	bool MapMenuEx::UpdateClouds_Hook(RE::NiAVObject* a_obj, RE::NiUpdateData* a_data)
	{
		_UpdateClouds(a_obj, a_data);
		std::uint32_t* updateValue = reinterpret_cast<std::uint32_t*>(REL::ID{ 391006 }.address());
		*updateValue ^= 1;

		return false; // do not update again
	}

	void MapMenuEx::InstallHook()
	{
		REL::Relocation<std::uintptr_t> vTable(Offsets::Menus::MapMenu::Vtbl);
		_ProcessMessage = vTable.write_vfunc(0x4, &MapMenuEx::ProcessMessage_Hook);
		_AdvanceMovie = vTable.write_vfunc(0x5, &MapMenuEx::AdvanceMovie_Hook);

		// Prevent setting kFreezeFrameBackground flag when opening local map
		REL::safe_write(Offsets::Menus::MapMenu::LocalMapUpdaterFunc.address() + 0x53, std::uint32_t(0x90909090));
		REL::safe_write(Offsets::Menus::MapMenu::LocalMapUpdaterFunc.address() + 0x9D, std::uint16_t(0x9090));
		REL::safe_write(Offsets::Menus::MapMenu::LocalMapUpdaterFunc.address() + 0x9F, std::uint8_t(0x90));

		// Disable map menu background sound
		REL::safe_write(Offsets::Menus::MapMenu::Ctor.address() + 0x52D, std::uint8_t(0xEB));

		// Fix controls while journal is open
		MapInputHandlerEx<RE::MapMoveHandler>::InstallHook(Offsets::Menus::MapMenu::MapMoveHandler::Vtbl);
		MapInputHandlerEx<RE::MapZoomHandler>::InstallHook(Offsets::Menus::MapMenu::MapZoomHandler::Vtbl);
		MapInputHandlerEx<RE::MapLookHandler>::InstallHook(Offsets::Menus::MapMenu::MapLookHandler::Vtbl);

		// Prevent TerrainManager from updating while the menu is open.
		// This prevents child worldspaces from rendering on top of their parents. Possibly avoids other issues as well.
		_TerrainManagerUpdate = *reinterpret_cast<TerrainManagerUpdate_t*> (SKSE::GetTrampoline().write_call<5>(Offsets::BGSTerrainManager::TerrainManager_UpdateFunc.address() + 0x5D, (std::uintptr_t)BGSTerrainManager_Update_Hook));

		// Fix for flickering/non-moving clouds
		_UpdateClouds = *reinterpret_cast<UpdateClouds_t*>(SKSE::GetTrampoline().write_call<5>(Offsets::Menus::MapMenu::UpdateClouds_Hook.address() + 0x10E, (std::uintptr_t)UpdateClouds_Hook));
	}
}
