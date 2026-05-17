#include "Menus/MapMenuEx.h"
#include "Controls/MapInputHandlerEx.h"

#include <xbyak/xbyak.h>

#undef GetObject

namespace SkyrimSoulsRE
{
	namespace MapMenuAudioHooks
	{
		void QuaternionToMatrix(RE::NiPoint3& a_v1, RE::NiPoint3& a_v2, RE::NiQuaternion& a_quaternion)
		{
			// https://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToMatrix/index.htm
			// ( - | v1.x | v2.x )
			// ( - | v1.y | v2.y )
			// ( - | v1.z | v2.z )

			a_v1.x = 2 * a_quaternion.x * a_quaternion.y - 2 * a_quaternion.z * a_quaternion.w;
			a_v1.y = 1 - 2 * a_quaternion.x * a_quaternion.x - 2 * a_quaternion.z * a_quaternion.z;
			a_v1.z = 2 * a_quaternion.y * a_quaternion.z + 2 * a_quaternion.x * a_quaternion.w;

			a_v2.x = 2 * a_quaternion.x * a_quaternion.z + 2 * a_quaternion.y * a_quaternion.w;
			a_v2.y = 2 * a_quaternion.y * a_quaternion.z - 2 * a_quaternion.x * a_quaternion.w;
			a_v2.z = 1 - 2 * a_quaternion.x * a_quaternion.x - 2 * a_quaternion.y * a_quaternion.y;
		}

		void SetListenerPosition_Hook(RE::BSAudioManager* a_audioManager, RE::NiPoint3* a_pos)
		{
			RE::UI* ui = RE::UI::GetSingleton();
			if (ui->IsMenuOpen(RE::MapMenu::MENU_NAME))
			{
				RE::PlayerCamera* camera = RE::PlayerCamera::GetSingleton();
				RE::TESCameraState* currentState = camera->currentState.get();
				currentState->GetTranslation(*a_pos);
			}

			using func_t = decltype(&SetListenerPosition_Hook);
			REL::Relocation<func_t> func{ Offsets::BSAudioManager::SetListenerPosition };
			return func(a_audioManager, a_pos);
		}

		void SetListenerRotation_Hook(RE::BSAudioManager* a_audioManager, RE::NiPoint3* a_unk1, RE::NiPoint3* a_unk2)
		{
			RE::UI* ui = RE::UI::GetSingleton();
			if (ui->IsMenuOpen(RE::MapMenu::MENU_NAME))
			{
				RE::PlayerCamera* camera = RE::PlayerCamera::GetSingleton();
				RE::TESCameraState* currentState = camera->currentState.get();

				RE::NiQuaternion rot;
				currentState->GetRotation(rot);
				QuaternionToMatrix(*a_unk1, *a_unk2, rot);
			}

			using func_t = decltype(&SetListenerRotation_Hook);
			REL::Relocation<func_t> func{ Offsets::BSAudioManager::SetListenerRotation };
			return func(a_audioManager, a_unk1, a_unk2);
		}
	}

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

	void MapMenuEx::MapSky::SkyState::SaveState(RE::Sky* a_sky)
	{
		currentClimate = a_sky->currentClimate;
		currentWeather = a_sky->currentWeather;
		lastWeather = a_sky->lastWeather;
		defaultWeather = a_sky->defaultWeather;
		overrideWeather = a_sky->overrideWeather;
		region = a_sky->region;
		currentGameHour = a_sky->currentGameHour;
		currentWeatherPct = a_sky->currentWeatherPct;
		flags = a_sky->flags;
		lastMoonPhaseUpdate = a_sky->lastMoonPhaseUpdate;
		unk174 = a_sky->unk174;
		unk178 = a_sky->unk178;
		unk17C = a_sky->unk17C;
		fogPower = a_sky->fogPower;
		mode = a_sky->mode;
		flash = a_sky->flash;
		flashTime = a_sky->flashTime;
		currentRoom = a_sky->currentRoom;
		previousRoom = a_sky->previousRoom;
		extLightingOverride = a_sky->extLightingOverride;
	}

	void MapMenuEx::MapSky::SkyState::RestoreState(RE::Sky* a_sky) const
	{
		a_sky->currentClimate = currentClimate;
		a_sky->currentWeather = currentWeather;
		a_sky->lastWeather = lastWeather;
		a_sky->defaultWeather = defaultWeather;
		a_sky->overrideWeather = overrideWeather;
		a_sky->region = region;
		a_sky->currentGameHour = currentGameHour;
		a_sky->currentWeatherPct = currentWeatherPct;
		a_sky->flags = flags;
		a_sky->lastMoonPhaseUpdate = lastMoonPhaseUpdate;
		a_sky->unk174 = unk174;
		a_sky->unk178 = unk178;
		a_sky->unk17C = unk17C;
		a_sky->fogPower = fogPower;
		a_sky->mode = mode;
		a_sky->flash = flash;
		a_sky->flashTime = flashTime;
		a_sky->currentRoom = currentRoom;
		a_sky->previousRoom = previousRoom;
		a_sky->extLightingOverride = extLightingOverride;
	}

	MapMenuEx::MapSky::MapSky()
	{
		RE::BGSDefaultObjectManager* defaultObjectmanager = RE::BGSDefaultObjectManager::GetSingleton();
		m_mapWeather = defaultObjectmanager->GetObject<RE::TESWeather>(RE::DEFAULT_OBJECTS::kWorldMapWeather);
	}

	void MapMenuEx::MapSky::Apply(RE::Sky* a_sky)
	{
		static RE::ImageSpaceManager* imageSpaceManager = RE::ImageSpaceManager::GetSingleton();
		static RE::ImageSpaceBaseData* weatherUpdatebaseData = reinterpret_cast<RE::ImageSpaceBaseData*>(Offsets::ImageSpaceManager::WeatherUpdateBaseData.address());

		RE::PlayerRegionState* playerRegionState = RE::PlayerRegionState::GetSingleton();
		RE::TESRegion* prevRegion = playerRegionState->unk48;
		playerRegionState->unk48 = nullptr;

		SkyState state;
		state.SaveState(a_sky);

		a_sky->mode = RE::Sky::Mode::kFull;
		a_sky->extLightingOverride = nullptr;

		a_sky->currentWeather = m_mapWeather;
		a_sky->overrideWeather = m_mapWeather;
		a_sky->defaultWeather = nullptr;
		a_sky->lastWeather = nullptr;
		a_sky->region = nullptr;
		a_sky->currentWeatherPct = 1.0f;
		a_sky->flash = 0.0f;
		a_sky->flashTime = 0;

		a_sky->currentRoom.reset();
		a_sky->previousRoom.reset();

		if (a_sky->precip)
		{
			if (a_sky->precip->currentPrecip)
			{
				a_sky->precip->currentPrecip->flags.set(RE::NiAVObject::Flag::kHidden);
			}
			if (a_sky->precip->lastPrecip)
			{
				a_sky->precip->lastPrecip->flags.set(RE::NiAVObject::Flag::kHidden);
			}
		}

		if (a_sky->sun)
		{
			a_sky->sun->sunBaseNode->flags.set(RE::NiAVObject::Flag::kHidden);
			a_sky->sun->sunGlareNode->flags.set(RE::NiAVObject::Flag::kHidden);
		}

		if (a_sky->atmosphere)
		{
			a_sky->atmosphere->updateFogDistance = true;
		}

		Sky_UpdateSunGlareLensFlare(a_sky);
		Sky_UpdatePartial(a_sky, 0.0f);
		if (a_sky->clouds)
		{
			a_sky->clouds->forceUpdate = true;
			a_sky->clouds->Update(a_sky, 0.0f);
		}

		ImageSpaceManager_Func1(imageSpaceManager, weatherUpdatebaseData);
		ImageSpaceManager_Func2(imageSpaceManager);
		ImageSpaceManager_Func3(imageSpaceManager);

		state.RestoreState(a_sky);

		playerRegionState->unk48 = prevRegion;
	}

	void MapMenuEx::MapSky::Finish(RE::Sky* a_sky)
	{
		static RE::ImageSpaceManager* imageSpaceManager = RE::ImageSpaceManager::GetSingleton();

		ImageSpaceManager_Func1(imageSpaceManager, 0);
		ImageSpaceManager_Func2(imageSpaceManager);

		if (a_sky->precip)
		{
			if (a_sky->precip->currentPrecip)
			{
				a_sky->precip->currentPrecip->flags.reset(RE::NiAVObject::Flag::kHidden);
			}
			if (a_sky->precip->lastPrecip)
			{
				a_sky->precip->lastPrecip->flags.reset(RE::NiAVObject::Flag::kHidden);
			}
		}

		if (a_sky->sun)
		{
			a_sky->sun->sunBaseNode->flags.reset(RE::NiAVObject::Flag::kHidden);
			a_sky->sun->sunGlareNode->flags.reset(RE::NiAVObject::Flag::kHidden);
		}

		if (a_sky->clouds)
		{
			// Fix for wrong clouds after weather change while the menu is open
			RE::TESWeather* lastWeather = a_sky->lastWeather;
			a_sky->lastWeather = nullptr;
			a_sky->clouds->forceUpdate = true;
			a_sky->clouds->Update(a_sky, 0.0f);
			a_sky->lastWeather = lastWeather;
		}

		if (a_sky->atmosphere)
		{
			a_sky->atmosphere->updateFogDistance = true;
		}
	}

	RE::UI_MESSAGE_RESULTS MapMenuEx::ProcessMessage_Hook(RE::UIMessage& a_message)
	{
		switch (a_message.type.get())
		{
		case RE::UI_MESSAGE_TYPE::kShow:
			mapMenuCellLoadedEventHandler.Register();
			lastTimeDateString[0] = '\0';
			if (!mapSky && Settings::GetSingleton()->mapMenuCustomSky)
			{
				mapSky = std::make_unique<MapSky>();
			}
			break;

		case RE::UI_MESSAGE_TYPE::kHide:
			mapMenuCellLoadedEventHandler.Unregister();
			break;

		case RE::UI_MESSAGE_TYPE::kUpdate:
			Update();
			RE::UIMessageQueue::GetSingleton()->AddMessage(RE::HUDMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kUpdate, nullptr);

			if (cellRenderingUpdateNeeded)
			{
				// Force map mode rendering for newly loaded cells as well
				auto func = reinterpret_cast<void (*)()>(Offsets::Menus::MapMenu::EnableMapModeTerrainRendering.address());
				func();

				// This has the side-effect that LOD trees will disappear, so re-enable them again (they still won't appear in Map Menu)
				if (this->worldSpace && this->worldSpace->terrainManager)
				{
					this->worldSpace->terrainManager->lodTreesHidden = false;
				}

				cellRenderingUpdateNeeded = false;
			}

			// Prevent the original function to receive the message or else it will attempt to endlessly update the map markers
			if (a_message.menu != MENU_NAME)
			{
				return RE::UI_MESSAGE_RESULTS::kHandled;
			}
		}

		return _ProcessMessage(this, a_message);
	}

	void MapMenuEx::UpdatePlayerMarkerPosition()
	{
		// This function should handle any edge-cases (interiors, child worldspaces, etc.)
		auto func = reinterpret_cast<bool (*)(RE::MapCamera*, RE::NiPoint3&, RE::NiAVObject*)>(Offsets::Menus::MapMenu::SetMarkerPosition.address());
		func(&this->camera, this->playerMarkerPosition, nullptr);

		// Now we need to update the marker position
		RE::RefHandle playerMarkerHandle = *reinterpret_cast<RE::RefHandle*>(Offsets::Menus::MapMenu::PlayerMarkerRefHandle.address());
		RE::TESObjectREFRPtr playerMarkerRefPtr = nullptr;

		if (RE::TESObjectREFR::LookupByHandle(playerMarkerHandle, playerMarkerRefPtr))
		{
			// Do not use SetPosition as there is no objectReference, but setting the position directly is enough
			// Rotation is already updating so no special handling is necessary, unless its a child worldspace - todo: check why
			playerMarkerRefPtr->data.location = this->playerMarkerPosition;
		}
	}

	void MapMenuEx::Update()
	{
		UpdatePlayerMarkerPosition();

		Settings* settings = Settings::GetSingleton();
		if (!settings->updateMapMenuBottomBar)
		{
			return;
		}

		char timeDateString[200];
		RE::Calendar::GetSingleton()->GetTimeDateString(timeDateString, sizeof(timeDateString), false);

		if (std::strncmp(lastTimeDateString, timeDateString, sizeof(timeDateString)) == 0)
		{
			return;
		}

		RE::FxResponseArgs<1> args;
		args.Add(timeDateString);
		this->fxDelegate->Invoke(this->uiMovie.get(), "SetDateString", args);

		std::memcpy(lastTimeDateString, timeDateString, sizeof(timeDateString));
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

	bool MapMenuEx::UpdatePlayer_Hook(RE::PlayerCharacter* a_this)
	{
		return a_this->Get3D2() || RE::UI::GetSingleton()->IsMenuOpen(RE::MapMenu::MENU_NAME);
	}

	void MapMenuEx::Sky_Update_Hook(RE::Sky* a_this, float a_timeDelta)
	{
		static bool isActive = false;

		_SkyUpdate(a_this, a_timeDelta);

		if (RE::UI::GetSingleton()->IsMenuOpen(RE::MapMenu::MENU_NAME) && mapSky)
		{
			isActive = true;
			mapSky->Apply(a_this);
		}
		else if (isActive)
		{
			isActive = false;
			mapSky->Finish(a_this);
		}
	}

	void MapMenuEx::InstallHook()
	{
		Settings* settings = Settings::GetSingleton();

		REL::Relocation<std::uintptr_t> vTable(RE::VTABLE_MapMenu[0]);
		_ProcessMessage = vTable.write_vfunc(0x4, &MapMenuEx::ProcessMessage_Hook);

		auto& trampoline = SKSE::GetTrampoline();

		// Prevent setting kFreezeFrameBackground flag when opening local map
		REL::safe_write(Offsets::Menus::MapMenu::LocalMapUpdaterFunc.address() + 0x53, std::uint32_t(0x90909090));
		REL::safe_write(Offsets::Menus::MapMenu::LocalMapUpdaterFunc.address() + 0x9D, std::uint16_t(0x9090));
		REL::safe_write(Offsets::Menus::MapMenu::LocalMapUpdaterFunc.address() + 0x9F, std::uint8_t(0x90));

		// Disable map menu background sound
		if (settings->mapMenuAmbientSoundLoop)
		{
			REL::safe_write(Offsets::Menus::MapMenu::Ctor.address() + 0x52D, std::uint8_t(0xEB));
			REL::safe_fill(Offsets::Menus::MapMenu::Dtor.address() + 0x1BB, std::uint8_t(0x90), 5);
		}

		// Re enable certain sounds - the map mutes some effects
		REL::safe_write(Offsets::Menus::MapMenu::Ctor.address() + 0x4F9, std::uint8_t(0xEB));
		REL::safe_write(Offsets::Menus::MapMenu::Dtor.address() + 0x180, std::uint8_t(0xEB));

		// Fix controls while journal is open
		MapInputHandlerEx<RE::MapMoveHandler>::InstallHook(RE::VTABLE_MapMoveHandler[0]);
		MapInputHandlerEx<RE::MapZoomHandler>::InstallHook(RE::VTABLE_MapZoomHandler[0]);
		MapInputHandlerEx<RE::MapLookHandler>::InstallHook(RE::VTABLE_MapLookHandler[0]);

		// Prevent TerrainManager from updating while the menu is open.
		// This prevents child worldspaces from rendering on top of their parents. Possibly avoids other issues as well.
		_TerrainManagerUpdate = *reinterpret_cast<TerrainManagerUpdate_t*>(trampoline.write_call<5>(Offsets::BGSTerrainManager::TerrainManager_UpdateFunc.address() + 0x5D, (std::uintptr_t)BGSTerrainManager_Update_Hook));

		// Fix for flickering/non-moving clouds
		_UpdateClouds = *reinterpret_cast<UpdateClouds_t*>(trampoline.write_call<5>(Offsets::Menus::MapMenu::UpdateClouds_Hook.address() + 0x10E, (std::uintptr_t)UpdateClouds_Hook));

		// By default if the menu is unpaused and the player opens the map, audio will stop working.
		// This is because the listener position is linked to the camera, which is now far up in the sky.
		// These functions set position and rotation back to its expected values manually.
		trampoline.write_call<5>(Offsets::BSAudioManager::Hook.address() + 0xC6, (std::uintptr_t)MapMenuAudioHooks::SetListenerPosition_Hook);
		trampoline.write_call<5>(Offsets::BSAudioManager::Hook.address() + 0x12E, (std::uintptr_t)MapMenuAudioHooks::SetListenerRotation_Hook);

		// Fix player not updating while the menu is open, causing various issues
		trampoline.write_call<6>(Offsets::Main::UpdatePlayer.address() + 0x7A, (std::uintptr_t)UpdatePlayer_Hook);

		// Hook Sky Job - decouples the map weather from real world weather so the map can't affect gameplay.
		// Can be disabled for compatibility.
		if (settings->mapMenuCustomSky)
		{
			_SkyUpdate = *reinterpret_cast<Sky_Update_t*>(trampoline.write_branch<5>(Offsets::Job::Sky.address() + 0x33, (std::uintptr_t)Sky_Update_Hook));

			// Disable sky related stuff when Map Menu opens/closes - we handle it ourselves
			REL::safe_write<std::uint16_t>(Offsets::Menus::MapMenu::EnableMapMode.address() + 0x96, std::uint16_t(0x02E9));  // jmp + nop
			REL::safe_write<std::uint32_t>(Offsets::Menus::MapMenu::EnableMapMode.address() + 0x98, std::uint32_t(0x90000001));

			REL::safe_write<std::uint16_t>(Offsets::Menus::MapMenu::DisableMapMode.address() + 0x7C, std::uint16_t(0x85E9));  // jmp + nop
			REL::safe_write<std::uint32_t>(Offsets::Menus::MapMenu::DisableMapMode.address() + 0x7E, std::uint32_t(0x90000000));
		}
	}
}
