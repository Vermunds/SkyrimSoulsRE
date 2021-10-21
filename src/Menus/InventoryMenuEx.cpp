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
		class ItemDropTask : public UnpausedTask
		{
		public:
			double count;

			void Run() override
			{
				RE::UI* ui = RE::UI::GetSingleton();

				if (ui->IsMenuOpen(RE::InventoryMenu::MENU_NAME))
				{
					RE::IMenu* menu = ui->GetMenu(RE::InventoryMenu::MENU_NAME).get();

					RE::GFxValue arg = this->count;
					const RE::FxDelegateArgs args(0, menu, menu->uiMovie.get(), &arg, 1);
					_ItemDrop(args);
				}
			}
		};

		std::shared_ptr<ItemDropTask> task = std::make_shared<ItemDropTask>();
		task->count = a_args[0].GetNumber();

		UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
		queue->AddTask(task);
	}

	void InventoryMenuEx::ItemSelect_Hook(const RE::FxDelegateArgs& a_args)
	{
		class ItemSelectTask : public UnpausedTask
		{
		public:
			bool	hasSlot;
			double	slot = 0.0;

			void Run() override
			{
				RE::UI* ui = RE::UI::GetSingleton();

				if (ui->IsMenuOpen(RE::InventoryMenu::MENU_NAME))
				{
					RE::IMenu* menu = ui->GetMenu(RE::InventoryMenu::MENU_NAME).get();

					if (this->hasSlot)
					{
						RE::GFxValue arg = this->slot;
						const RE::FxDelegateArgs args(0, menu, menu->uiMovie.get(), &arg, 1);
						_ItemSelect(args);
					}
					else
					{
						const RE::FxDelegateArgs args(0, menu, menu->uiMovie.get(), nullptr, 0);
						_ItemSelect(args);
					}
				}
			}
		};

		std::shared_ptr<ItemSelectTask> task = std::make_shared<ItemSelectTask>();

		if (a_args.GetArgCount() == 0)
		{
			task->hasSlot = false;
		}
		else
		{
			task->hasSlot = true;
			task->slot = a_args[0].GetNumber();
		}

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
		REL::Relocation<std::uintptr_t> vTable(Offsets::Menus::InventoryMenu::Vtbl);
		_ProcessMessage = vTable.write_vfunc(0x4, &InventoryMenuEx::ProcessMessage_Hook);
		_AdvanceMovie = vTable.write_vfunc(0x5, &InventoryMenuEx::AdvanceMovie_Hook);
	}
};
