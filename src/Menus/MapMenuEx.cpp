#include "Menus/MapMenuEx.h"

#include <xbyak/xbyak.h>

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

	RE::UI_MESSAGE_RESULTS MapMenuEx::ProcessMessage_Hook(RE::UIMessage& a_message)
	{
		switch (a_message.type.get())
		{
		case RE::UI_MESSAGE_TYPE::kShow:
			mapMenuCellLoadedEventHandler.Register();
			lastTimeDateString[0] = '\0';
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
				auto func = reinterpret_cast<void (*)()>(Offsets::Menus::MapMenu::EnableMapModeRenderingFunc.address());
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

		if (std::memcmp(lastTimeDateString, timeDateString, sizeof(timeDateString)) == 0)
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

	bool MapMenuEx::UpdatePlayerCamera_Hook()
	{
		bool* isInMenuMode_1 = reinterpret_cast<bool*>(Offsets::Papyrus::IsInMenuMode::Value1.address());  // Original check
		return *isInMenuMode_1 || RE::UI::GetSingleton()->IsMenuOpen(RE::MapMenu::MENU_NAME);
	}

	bool MapMenuEx::ApplyEOFImageSpace_Hook()
	{
		bool* enableEOFImageSpace = reinterpret_cast<bool*>(Offsets::Misc::EnableEOFImageSpaceValue.address());  // Original check
		return *enableEOFImageSpace && !RE::UI::GetSingleton()->IsMenuOpen(RE::MapMenu::MENU_NAME);
	}

	void MapMenuEx::InstallHook()
	{
		REL::Relocation<std::uintptr_t> vTable(RE::VTABLE_MapMenu[0]);
		_ProcessMessage = vTable.write_vfunc(0x4, &MapMenuEx::ProcessMessage_Hook);

		auto& trampoline = SKSE::GetTrampoline();

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

		// Fix for first person model reappearing overlaid on the screen when the map menu is open for an extended period of time
		{
			struct CameraUpdate_Code : Xbyak::CodeGenerator
			{
				CameraUpdate_Code(uintptr_t a_funcAddress, uintptr_t a_retAddress)
				{
					Xbyak::Label funcAddress;
					Xbyak::Label retAddress;

					push(rcx);
					call(ptr[rip + funcAddress]);
					pop(rcx);
					cmp(al, 0);
					jmp(ptr[rip + retAddress]);

					L(funcAddress);
					dq(a_funcAddress);

					L(retAddress);
					dq(a_retAddress);
				}
			};

			CameraUpdate_Code code{ std::uintptr_t(UpdatePlayerCamera_Hook), Offsets::Menus::MapMenu::UpdatePlayerCamera_Hook.address() + 0x71 };
			void* codeLoc = trampoline.allocate(code);

			trampoline.write_branch<6>(Offsets::Menus::MapMenu::UpdatePlayerCamera_Hook.address() + 0x6A, codeLoc);

			REL::safe_write(Offsets::Menus::MapMenu::UpdatePlayerCamera_Hook.address() + 0x70, std::uint8_t(0x90));
		}

		// Fix for flickering world map due to End-of-frame image spaces being rendered
		{
			struct ApplyEOFImageSpace_Code : Xbyak::CodeGenerator
			{
				ApplyEOFImageSpace_Code(uintptr_t a_funcAddress, uintptr_t a_retAddress)
				{
					Xbyak::Label funcAddress;
					Xbyak::Label retAddress;

					push(rcx);
					call(ptr[rip + funcAddress]);
					pop(rcx);
					cmp(al, 0);
					jmp(ptr[rip + retAddress]);

					L(funcAddress);
					dq(a_funcAddress);

					L(retAddress);
					dq(a_retAddress);
				}
			};

			ApplyEOFImageSpace_Code code{ std::uintptr_t(ApplyEOFImageSpace_Hook), Offsets::Main::Draw.address() + 0xA33 };
			void* codeLoc = trampoline.allocate(code);

			trampoline.write_branch<6>(Offsets::Main::Draw.address() + 0xA2C, codeLoc);

			REL::safe_write(Offsets::Main::Draw.address() + 0xA32, std::uint8_t(0x90));
		}
	}
}
