#include "Menus/InventoryMenuEx.h"

namespace SkyrimSoulsRE
{
	RE::UI_MESSAGE_RESULTS InventoryMenuEx::ProcessMessage_Hook(RE::UIMessage& a_message)
	{
		if (a_message.type == RE::UI_MESSAGE_TYPE::kHide)
		{
			HUDMenuEx* hudMenu = static_cast<HUDMenuEx*>(RE::UI::GetSingleton()->GetMenu(RE::InterfaceStrings::GetSingleton()->hudMenu).get());
			if (hudMenu)
			{
				hudMenu->SetSkyrimSoulsMode(false);
			}
		}
		return _ProcessMessage(this, a_message);
	}
	void InventoryMenuEx::AdvanceMovie_Hook(float a_interval, std::uint32_t a_currentTime)
	{
		this->UpdateBottomBar();
		return _AdvanceMovie(this, a_interval, a_currentTime);
	}

	void InventoryMenuEx::UpdateBottomBar()
	{
		using func_t = decltype(&InventoryMenuEx::UpdateBottomBar);
		REL::Relocation<func_t> func(Offsets::Menus::InventoryMenu::UpdateBottomBar);
		return func(this);
	}

	void InventoryMenuEx::ItemDrop_Hook(const RE::FxDelegateArgs& a_args)
	{
		double count = a_args[0].GetNumber();

		auto task = [count]() {
			RE::UI* ui = RE::UI::GetSingleton();

			if (ui->IsMenuOpen(RE::InventoryMenu::MENU_NAME))
			{
				RE::IMenu* menu = ui->GetMenu(RE::InventoryMenu::MENU_NAME).get();

				RE::GFxValue arg = count;
				const RE::FxDelegateArgs args(0, menu, menu->uiMovie.get(), &arg, 1);
				InventoryMenuEx::_ItemDrop(args);
			}
		};

		UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
		queue->AddTask(task);
	}

	void InventoryMenuEx::ItemSelect_Hook(const RE::FxDelegateArgs& a_args)
	{
		bool hasSlot = (a_args.GetArgCount() > 0);
		double slot = hasSlot ? a_args[0].GetNumber() : 0.0;

		auto task = [hasSlot, slot]() {
			RE::UI* ui = RE::UI::GetSingleton();

			if (ui->IsMenuOpen(RE::InventoryMenu::MENU_NAME))
			{
				RE::IMenu* menu = ui->GetMenu(RE::InventoryMenu::MENU_NAME).get();

				if (hasSlot)
				{
					RE::GFxValue arg = slot;
					const RE::FxDelegateArgs args(0, menu, menu->uiMovie.get(), &arg, 1);
					_ItemSelect(args);
				}
				else
				{
					const RE::FxDelegateArgs args(0, menu, menu->uiMovie.get(), nullptr, 0);
					_ItemSelect(args);
				}
			}
		};

		UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
		queue->AddTask(task);
	}

	RE::IMenu* InventoryMenuEx::Creator()
	{
		RE::InventoryMenu* menu = static_cast<RE::InventoryMenu*>(CreateMenu(RE::InventoryMenu::MENU_NAME));

		RE::FxDelegate* dlg = menu->fxDelegate.get();

		_ItemSelect = dlg->callbacks.GetAlt("ItemSelect")->callback;
		dlg->callbacks.GetAlt("ItemSelect")->callback = ItemSelect_Hook;
		_ItemDrop = dlg->callbacks.GetAlt("ItemDrop")->callback;
		dlg->callbacks.GetAlt("ItemDrop")->callback = ItemDrop_Hook;

		HUDMenuEx* hudMenu = static_cast<HUDMenuEx*>(RE::UI::GetSingleton()->GetMenu(RE::InterfaceStrings::GetSingleton()->hudMenu).get());
		if (hudMenu)
		{
			hudMenu->SetSkyrimSoulsMode(true);
		}

		return menu;
	}

	void InventoryMenuEx::InstallHook()
	{
		//Hook AdvanceMovie
		REL::Relocation<std::uintptr_t> vTable(RE::VTABLE_InventoryMenu[0]);
		_ProcessMessage = vTable.write_vfunc(0x4, &InventoryMenuEx::ProcessMessage_Hook);
		_AdvanceMovie = vTable.write_vfunc(0x5, &InventoryMenuEx::AdvanceMovie_Hook);
	}
};
