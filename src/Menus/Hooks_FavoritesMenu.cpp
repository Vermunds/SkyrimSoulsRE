#include "Menus/Hooks_FavoritesMenu.h"

namespace SkyrimSoulsRE
{
	void FavoritesMenuEx::ItemSelect_Hook(RE::FavoritesMenu* a_this, RE::BGSEquipSlot* a_slot)
	{
		class ItemSelectTask : public UnpausedTask
		{
			RE::BGSEquipSlot* slot;
		public:
			ItemSelectTask(RE::BGSEquipSlot* a_slot)
			{
				this->slot = a_slot;
			}

			void Run() override
			{
				RE::UI* ui = RE::UI::GetSingleton();
				RE::InterfaceStrings* interfaceStrings = RE::InterfaceStrings::GetSingleton();

				if (ui->IsMenuOpen(interfaceStrings->favoritesMenu))
				{
					RE::IMenu* menu = ui->GetMenu(interfaceStrings->favoritesMenu).get();

					using func_t = decltype(&FavoritesMenuEx::ItemSelect_Hook);
					REL::Relocation<func_t> func(Offsets::Menus::FavoritesMenu::ItemSelect);
					func(static_cast<RE::FavoritesMenu*>(menu), this->slot);
				}
			}
		};

		ItemSelectTask* task = new ItemSelectTask(a_slot);
		UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
		queue->AddTask(task);
	}

	RE::IMenu* FavoritesMenuEx::Creator()
	{
		return CreateMenu(RE::FavoritesMenu::MENU_NAME);
	}

	void FavoritesMenuEx::InstallHook()
	{
		//Fix for hotkeys not working
		REL::safe_write(Offsets::Menus::FavoritesMenu::CanProcess.address() + 0x15, std::uint16_t(0x9090));

		SKSE::GetTrampoline().write_call<5>(Offsets::Menus::FavoritesMenu::SelectItem_HookFunc_1.address() + 0x122, (uintptr_t)ItemSelect_Hook);
		SKSE::GetTrampoline().write_call<5>(Offsets::Menus::FavoritesMenu::SelectItem_HookFunc_1.address() + 0x141, (uintptr_t)ItemSelect_Hook);
		SKSE::GetTrampoline().write_call<5>(Offsets::Menus::FavoritesMenu::SelectItem_HookFunc_1.address() + 0x159, (uintptr_t)ItemSelect_Hook);

		SKSE::GetTrampoline().write_branch<5>(Offsets::Menus::FavoritesMenu::SelectItem_HookFunc_2.address() + 0x45, (uintptr_t)ItemSelect_Hook);
		SKSE::GetTrampoline().write_branch<5>(Offsets::Menus::FavoritesMenu::SelectItem_HookFunc_2.address() + 0x73, (uintptr_t)ItemSelect_Hook);
	}
}
