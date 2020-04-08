#include "Hooks_ContainerMenu.h"
#include "RE/Skyrim.h"
#include "Offsets.h"
#include "SKSE/API.h"

#include "Tasks.h"

namespace SkyrimSoulsRE::Hooks
{
	namespace ContainerMenu
	{
		ContainerMode GetContainerMode()
		{
			UInt8* mode = reinterpret_cast<UInt8*>(Offsets::ContainerMenu_Mode.GetUIntPtr());
			return static_cast<ContainerMode>(*mode);
		}

		RE::TESObjectREFR* GetContainerRef()
		{

			UInt32* handle = reinterpret_cast<UInt32*>(Offsets::ContainerMenu_Target.GetUIntPtr());
			RE::TESObjectREFRPtr refptr = nullptr;
			if (RE::TESObjectREFR::LookupByHandle(*handle, refptr))
			{
				RE::TESObjectREFR* ref = refptr.get();
				return ref;
			};
			return nullptr;
		}

		void UpdateEquipment_Hook(RE::ItemList* a_this, RE::PlayerCharacter* a_player)
		{
			RE::UI* ui = RE::UI::GetSingleton();
			RE::InterfaceStrings* interfaceStrings = RE::InterfaceStrings::GetSingleton();
			if (ui->IsMenuOpen(interfaceStrings->containerMenu))
			{
				RE::TESObjectREFR* target = GetContainerRef();

				if (target)
				{
					ContainerMode mode = GetContainerMode();
					if (target->Is(RE::FormType::ActorCharacter) && mode == kMode_FollowerTrade)
					{
						RE::Actor* containerOwner = reinterpret_cast<RE::Actor*>(target);
						Tasks::UpdateInventoryDelegate::RegisterTask(a_this, containerOwner);
						return;
					}
				}
			}

			return a_this->Update(a_player);
		}

		void InstallHook()
		{
			//Fix for trading with followers
			SKSE::GetTrampoline()->Write5Branch(Offsets::ContainerMenu_TransferItem_Original.GetUIntPtr() + 0x105, (uintptr_t)UpdateEquipment_Hook);

			//Fix for equipping directly from follower inventory
			SKSE::GetTrampoline()->Write5Call(Offsets::ContainerMenu_TransferItemEquip_Original.GetUIntPtr() + 0x14A, (uintptr_t)UpdateEquipment_Hook);

			//Another call here
			SKSE::GetTrampoline()->Write5Call(Offsets::Unk_UpdateInventory_Call.GetUIntPtr(), (uintptr_t)UpdateEquipment_Hook);
		}
	}
}