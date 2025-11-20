#include "ItemMenuUpdater.h"

namespace SkyrimSoulsRE::ItemMenuUpdater
{
	// These hooks are used to send update events to the item menus when something occurs.
	// By default the game sends no updates for these events, causing the lists to retain old data (and possibly crash because of unloaded 3D models).
	// This is especially problematic when unpaused as external events can add or remove objects at any time.
	// Note: this does not necessarily cover all cases.

	RE::TESObjectREFR* GetTargetReference()
	{
		RE::TESObjectREFR* targetRef = nullptr;

		RE::UI* ui = RE::UI::GetSingleton();
		if (ui->IsMenuOpen(RE::ContainerMenu::MENU_NAME))
		{
			RE::ContainerMenu* menu = static_cast<RE::ContainerMenu*>(ui->GetMenu(RE::ContainerMenu::MENU_NAME).get());

			RE::RefHandle handle = menu->GetTargetRefHandle();
			RE::TESObjectREFRPtr refptr = nullptr;
			if (RE::TESObjectREFR::LookupByHandle(handle, refptr))
			{
				targetRef = refptr.get();
			}
		}
		else if (ui->IsMenuOpen(RE::BarterMenu::MENU_NAME))
		{
			RE::BarterMenu* menu = static_cast<RE::BarterMenu*>(ui->GetMenu(RE::BarterMenu::MENU_NAME).get());

			RE::RefHandle handle = menu->GetTargetRefHandle();
			RE::TESObjectREFRPtr refptr = nullptr;
			if (RE::TESObjectREFR::LookupByHandle(handle, refptr))
			{
				targetRef = refptr.get();
			}
		}
		else if (ui->IsMenuOpen(RE::GiftMenu::MENU_NAME))
		{
			RE::GiftMenu* menu = static_cast<RE::GiftMenu*>(ui->GetMenu(RE::GiftMenu::MENU_NAME).get());

			RE::RefHandle handle = menu->IsPlayerGifting() ? menu->GetReceiverRefHandle() : menu->GetGifterRefHandle();
			RE::TESObjectREFRPtr refptr = nullptr;
			if (RE::TESObjectREFR::LookupByHandle(handle, refptr))
			{
				targetRef = refptr.get();
			}
		}

		return targetRef;
	}

	void RequestItemListUpdate(RE::TESObjectREFR* a_ref, RE::TESForm* a_unk)
	{
		using func_t = decltype(&RequestItemListUpdate);
		REL::Relocation<func_t> func(Offsets::ItemMenuUpdater::RequestItemListUpdate);
		return func(a_ref, a_unk);
	}

	// Update after RemoveAllItems
	void RemoveAllItems_Hook(RE::BSExtraData* a_unk1, std::uint32_t a_unk2, void* a_unk3, RE::TESObjectREFR* a_containerRef, std::uint64_t a_unk5, std::uint32_t a_unk6, void* a_unk7, void* a_unk8)
	{
		using func_t = decltype(&RemoveAllItems_Hook);
		REL::Relocation<func_t> func(Offsets::ItemMenuUpdater::RemoveAllItems);
		func(a_unk1, a_unk2, a_unk3, a_containerRef, a_unk5, a_unk6, a_unk7, a_unk8);

		RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();

		RE::TESObjectREFR* targetRef = GetTargetReference();

		if (a_containerRef == player || a_containerRef == targetRef)
		{
			// Some items might still remain in the list. Updating twice in a row seems to fix it for some reason.
			RequestItemListUpdate(a_containerRef, nullptr);
			RequestItemListUpdate(a_containerRef, nullptr);
		}
	}

	void InstallHook()
	{
		SKSE::GetTrampoline().write_call<5>(Offsets::ItemMenuUpdater::RemoveAllItems_Hook1.address() + 0x3A, (std::uintptr_t)RemoveAllItems_Hook);
		SKSE::GetTrampoline().write_call<5>(Offsets::ItemMenuUpdater::RemoveAllItems_Hook2.address() + 0x55, (std::uintptr_t)RemoveAllItems_Hook);
	}
}
