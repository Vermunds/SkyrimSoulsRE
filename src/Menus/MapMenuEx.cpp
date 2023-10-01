#include "Menus/MapMenuEx.h"

namespace SkyrimSoulsRE
{
	void MapMenuEx::MapMenuCellLoadedEventHandler::Unregister()
	{
		RE::ScriptEventSourceHolder* holder = RE::ScriptEventSourceHolder::GetSingleton();
		holder->RemoveEventSink<RE::TESCellFullyLoadedEvent>(this);
	}

	RE::BSEventNotifyControl MapMenuEx::MapMenuCellLoadedEventHandler::ProcessEvent(const RE::TESCellFullyLoadedEvent* a_event, RE::BSTEventSource<RE::TESCellFullyLoadedEvent>* a_eventSource)
	{
		MapMenuEx::cellRenderingUpdateNeeded = true;
		return RE::BSEventNotifyControl::kContinue;
	}

	void MapMenuEx::MapMenuCellLoadedEventHandler::Register()
	{
		RE::ScriptEventSourceHolder* holder = RE::ScriptEventSourceHolder::GetSingleton();
		holder->AddEventSink<RE::TESCellFullyLoadedEvent>(this);
	}

	RE::UI_MESSAGE_RESULTS MapMenuEx::ProcessMessage_Hook(RE::UIMessage& a_message)
	{
		if (a_message.type == RE::UI_MESSAGE_TYPE::kShow)
		{
			mapMenuCellLoadedEventHandler.Register();
		}
		else if (a_message.type == RE::UI_MESSAGE_TYPE::kHide)
		{
			// Fix disappearing first person model after closing the menu
			auto task = []() {
				RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
				RE::PlayerCamera* camera = RE::PlayerCamera::GetSingleton();
				if (!player || !camera)
				{
					return;
				}

				RE::NiAVObject* player3d = player->Get3D1(true);
				if (player3d && camera->currentState == camera->cameraStates[RE::CameraStates::kFirstPerson])
				{
					if (player3d->flags.all(RE::NiAVObject::Flag::kHidden))
					{
						player3d->flags.reset(RE::NiAVObject::Flag::kHidden);
					}
					camera->Update();
				}
			};

			UnpausedTaskQueue::GetSingleton()->AddTask(task);
			mapMenuCellLoadedEventHandler.Unregister();
		}

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
				menu->UpdatePlayerMarkerPosition();

				if (cellRenderingUpdateNeeded)
				{
					// Force map mode rendering for newly loaded cells as well
					auto func = reinterpret_cast<void (*)()>(Offsets::Menus::MapMenu::EnableMapModeRenderingFunc.address());
					func();

					// This has the side-effect that LOD trees will disappear, so re-enable them again (they still won't appear in Map Menu)
					if (menu->worldSpace && menu->worldSpace->terrainManager)
					{
						menu->worldSpace->terrainManager->lodTreesHidden = false;
					}

					cellRenderingUpdateNeeded = false;
				}

				menu->_AdvanceMovie(menu, a_interval, a_currentTime);
			}
		};

		UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
		queue->AddTask(task);
	}

	void MapMenuEx::UpdatePlayerMarkerPosition()
	{
		// This function should handle any edge-cases (interiors, child worldspaces, etc.)
		auto func = reinterpret_cast<bool (*)(RE::MapCamera*, RE::NiPoint3&, RE::NiAVObject*)>(Offsets::Menus::MapMenu::UpdatePlayerMarkerPosFunc.address());
		func(&this->camera, this->playerMarkerPosition, nullptr);

		// Now we need to update the marker position
		RE::RefHandle playerMarkerHandle = *reinterpret_cast<RE::RefHandle*>(Offsets::Menus::MapMenu::PlayerMarkerRefHandle.address());
		RE::TESObjectREFRPtr playerMarkerRefPtr = nullptr;

		if (RE::TESObjectREFR::LookupByHandle(playerMarkerHandle, playerMarkerRefPtr))
		{
			// Do not use SetPosition as there is no objectReference, but setting the position directly is enough
			// Rotation is already updating so no special handling is necessary
			playerMarkerRefPtr->data.location = this->playerMarkerPosition;
		}
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
		std::uint32_t* updateValue = reinterpret_cast<std::uint32_t*>(Offsets::Menus::MapMenu::UpdateClouds_UpdateValue.address());
		*updateValue ^= 1;

		return false;  // do not update again
	}

	void MapMenuEx::InstallHook()
	{
		REL::Relocation<std::uintptr_t> vTable(RE::VTABLE_MapMenu[0]);
		_ProcessMessage = vTable.write_vfunc(0x4, &MapMenuEx::ProcessMessage_Hook);
		_AdvanceMovie = vTable.write_vfunc(0x5, &MapMenuEx::AdvanceMovie_Hook);

		// Prevent setting kFreezeFrameBackground flag when opening local map
		REL::safe_write(Offsets::Menus::MapMenu::LocalMapUpdaterFunc.address() + 0x53, std::uint32_t(0x90909090));
		REL::safe_write(Offsets::Menus::MapMenu::LocalMapUpdaterFunc.address() + 0x9D, std::uint16_t(0x9090));
		REL::safe_write(Offsets::Menus::MapMenu::LocalMapUpdaterFunc.address() + 0x9F, std::uint8_t(0x90));

		// Disable map menu background sound
		REL::safe_write(Offsets::Menus::MapMenu::Ctor.address() + 0x52D, std::uint8_t(0xEB));

		// Fix controls while journal is open
		MapInputHandlerEx<RE::MapMoveHandler>::InstallHook(RE::VTABLE_MapMoveHandler[0]);
		MapInputHandlerEx<RE::MapZoomHandler>::InstallHook(RE::VTABLE_MapZoomHandler[0]);
		MapInputHandlerEx<RE::MapLookHandler>::InstallHook(RE::VTABLE_MapLookHandler[0]);

		auto& trampoline = SKSE::GetTrampoline();

		// Prevent TerrainManager from updating while the menu is open.
		// This prevents child worldspaces from rendering on top of their parents. Possibly avoids other issues as well.
		_TerrainManagerUpdate = *reinterpret_cast<TerrainManagerUpdate_t*>(trampoline.write_call<5>(Offsets::BGSTerrainManager::TerrainManager_UpdateFunc.address() + 0x5D, (std::uintptr_t)BGSTerrainManager_Update_Hook));

		// Fix for flickering/non-moving clouds
		_UpdateClouds = *reinterpret_cast<UpdateClouds_t*>(trampoline.write_call<5>(Offsets::Menus::MapMenu::UpdateClouds_Hook.address() + 0x10E, (std::uintptr_t)UpdateClouds_Hook));
	}
}
