#include "RE/Skyrim.h"
#include "Offsets.h"
#include "SKSE/API.h"

namespace SkyrimSoulsRE::Hooks
{
	namespace InventoryMenu
	{
		void DropItem_Hook(RE::PlayerCharacter* a_player, RE::ObjectRefHandle& a_handle, const RE::TESBoundObject* a_object, RE::ExtraDataList* a_extraList, SInt32 a_count, const RE::NiPoint3* a_dropLoc, const RE::NiPoint3* a_rotate)
		{
			//Looks like some other thread moves it before the calculation is complete, resulting in the item being moved to the coc marker
			//This is an ugly workaround, but it should work good enought

			RE::UI* ui = RE::UI::GetSingleton();
			ui->numPausesGame++;
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			a_handle = a_player->DropObject(a_object, a_extraList, a_count, a_dropLoc, a_rotate);

			ui->numPausesGame--;
		}

		void InstallHook()
		{
			SKSE::GetTrampoline()->Write6Call(Offsets::InventoryMenu_DropItem_Hook.GetUIntPtr(), uintptr_t(DropItem_Hook));
		}
	}
};