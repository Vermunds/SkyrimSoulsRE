#include "ItemMenuUpdater.h"

namespace SkyrimSoulsRE::ItemMenuUpdater
{
	RE::TESObjectREFR* GetTargetReference()
	{
		RE::TESObjectREFR* targetRef = nullptr;

		RE::UI* ui = RE::UI::GetSingleton();
		if (ui->IsMenuOpen(RE::ContainerMenu::MENU_NAME))
		{
			RE::ContainerMenu* menu = static_cast<RE::ContainerMenu*>(ui->GetMenu(RE::ContainerMenu::MENU_NAME).get());

			RE::RefHandle handle = menu->GetTargetRefHandle();
			RE::TESObjectREFRPtr refptr = nullptr;
			RE::TESObjectREFR* ref = nullptr;
			if (RE::TESObjectREFR::LookupByHandle(handle, refptr)) {
				targetRef = refptr.get();
			}
		}
		else if (ui->IsMenuOpen(RE::BarterMenu::MENU_NAME))
		{
			RE::BarterMenu* menu = static_cast<RE::BarterMenu*>(ui->GetMenu(RE::BarterMenu::MENU_NAME).get());

			RE::RefHandle handle = menu->GetTargetRefHandle();
			RE::TESObjectREFRPtr refptr = nullptr;
			RE::TESObjectREFR* ref = nullptr;
			if (RE::TESObjectREFR::LookupByHandle(handle, refptr)) {
				targetRef = refptr.get();
				//if (targetRef->formType == RE::FormType::ActorCharacter)
				//{
				//	RE::Actor* vendor = targetRef->As<RE::Actor>();
				//	RE::TESFaction* vendorFaction = vendor->vendorFaction;

				//	if (vendorFaction && vendorFaction->vendorData.merchantContainer)
				//	{
				//		targetRef = vendorFaction->vendorData.merchantContainer;
				//	}
				//}
			}
		}
		else if (ui->IsMenuOpen(RE::GiftMenu::MENU_NAME))
		{
			RE::GiftMenu* menu = static_cast<RE::GiftMenu*>(ui->GetMenu(RE::GiftMenu::MENU_NAME).get());

			RE::RefHandle handle = menu->GetTargetRefHandle();
			RE::TESObjectREFRPtr refptr = nullptr;
			RE::TESObjectREFR* ref = nullptr;
			if (RE::TESObjectREFR::LookupByHandle(handle, refptr)) {
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

	// Update after RemoveItem for TESObjectREFRs
	RE::ObjectRefHandle& RemoveItem_TESObjectREFR(RE::TESObjectREFR* a_this, RE::ObjectRefHandle& a_handle, RE::TESBoundObject* a_item, std::int32_t a_count, RE::ITEM_REMOVE_REASON a_reason, RE::ExtraDataList* a_extraList, RE::TESObjectREFR* a_moveToRef, const RE::NiPoint3* a_dropLoc = 0, const RE::NiPoint3* a_rotate = 0)
	{
		using func_t = decltype(&RemoveItem_TESObjectREFR);
		REL::Relocation<func_t> func(Offsets::ItemMenuUpdater::RemoveItem_TESObjectREFR);
		return func(a_this, a_handle, a_item, a_count, a_reason, a_extraList, a_moveToRef, a_dropLoc, a_rotate);
	}

	// Update after RemoveItem for Actors
	RE::ObjectRefHandle& RemoveItem_Actor(RE::Actor* a_this, RE::ObjectRefHandle& a_handle, RE::TESBoundObject* a_item, std::int32_t a_count, RE::ITEM_REMOVE_REASON a_reason, RE::ExtraDataList* a_extraList, RE::TESObjectREFR* a_moveToRef, const RE::NiPoint3* a_dropLoc = 0, const RE::NiPoint3* a_rotate = 0)
	{
		using func_t = decltype(&RemoveItem_Actor);
		REL::Relocation<func_t> func(Offsets::ItemMenuUpdater::RemoveItem_Actor);
		return func(a_this, a_handle, a_item, a_count, a_reason, a_extraList, a_moveToRef, a_dropLoc, a_rotate);
	}

	RE::ObjectRefHandle& RemoveItem_Hook(RE::TESObjectREFR* a_this, RE::ObjectRefHandle& a_handle, RE::TESBoundObject* a_item, std::int32_t a_count, RE::ITEM_REMOVE_REASON a_reason, RE::ExtraDataList* a_extraList, RE::TESObjectREFR* a_moveToRef, const RE::NiPoint3* a_dropLoc = 0, const RE::NiPoint3* a_rotate = 0)
	{
		RE::ObjectRefHandle result;
		if (a_this->formType == RE::FormType::ActorCharacter)
		{
			RemoveItem_Actor(static_cast<RE::Actor*>(a_this), a_handle, a_item, a_count, a_reason, a_extraList, a_moveToRef, a_dropLoc, a_rotate);
		}
		else
		{
			RemoveItem_TESObjectREFR(a_this, a_handle, a_item, a_count, a_reason, a_extraList, a_moveToRef, a_dropLoc, a_rotate);
		}

		RE::TESObjectREFR* targetRef = GetTargetReference();

		if (a_this == RE::PlayerCharacter::GetSingleton() || a_this == targetRef)
		{
			RequestItemListUpdate(a_this, nullptr);
		}

		return a_handle;
	}

	// Update after RemoveAllItems
	void RemoveAllItems_Hook(void* a_unk1, RE::TESObjectREFR* a_containerRef, void* a_unk3, std::uint64_t a_unk4, std::uint32_t a_unk5, void* a_unk6, void* a_unk7)
	{
		using func_t = decltype(&RemoveAllItems_Hook);
		REL::Relocation<func_t> func(Offsets::ItemMenuUpdater::RemoveAllItems);
		func(a_unk1, a_containerRef, a_unk3, a_unk4, a_unk5, a_unk6, a_unk7);

		RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
		RE::UI* ui = RE::UI::GetSingleton();

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
		SKSE::GetTrampoline().write_call<5>(Offsets::ItemMenuUpdater::RemoveAllItems_Hook1.address() + 0x16, (std::uintptr_t)RemoveAllItems_Hook);
		SKSE::GetTrampoline().write_call<5>(Offsets::ItemMenuUpdater::RemoveAllItems_Hook2.address() + 0x36, (std::uintptr_t)RemoveAllItems_Hook);
		SKSE::GetTrampoline().write_call<5>(Offsets::ItemMenuUpdater::RemoveAllItems_Hook3.address() + 0xBA, (std::uintptr_t)RemoveAllItems_Hook);
		SKSE::GetTrampoline().write_call<5>(Offsets::ItemMenuUpdater::RemoveAllItems_Hook4.address() + 0x230, (std::uintptr_t)RemoveAllItems_Hook);
		SKSE::GetTrampoline().write_call<5>(Offsets::ItemMenuUpdater::RemoveAllItems_Hook5.address() + 0x46, (std::uintptr_t)RemoveAllItems_Hook);

		SKSE::GetTrampoline().write_call<6>(Offsets::ItemMenuUpdater::RemoveItem_Hook1.address() + 0x9A, (std::uintptr_t)RemoveItem_Hook);
		SKSE::GetTrampoline().write_call<6>(Offsets::ItemMenuUpdater::RemoveItem_Hook2.address() + 0xD9, (std::uintptr_t)RemoveItem_Hook);
		SKSE::GetTrampoline().write_call<6>(Offsets::ItemMenuUpdater::RemoveItem_Hook3.address() + 0x461, (std::uintptr_t)RemoveItem_Hook);
		SKSE::GetTrampoline().write_call<6>(Offsets::ItemMenuUpdater::RemoveItem_Hook4.address() + 0x268, (std::uintptr_t)RemoveItem_Hook);
	}
}
