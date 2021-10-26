#include "Menus/GiftMenuEx.h"

namespace SkyrimSoulsRE
{
	RE::UI_MESSAGE_RESULTS GiftMenuEx::ProcessMessage_Hook(RE::UIMessage& a_message)
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
	void GiftMenuEx::AdvanceMovie_Hook(float a_interval, std::uint32_t a_currentTime)
	{
		this->UpdateBottomBar();

		AutoCloseManager::GetSingleton()->CheckAutoClose(RE::GiftMenu::MENU_NAME);

		return _AdvanceMovie(this, a_interval, a_currentTime);
	}

	void GiftMenuEx::ItemSelect_Hook(const RE::FxDelegateArgs& a_args)
	{
		class ItemSelectTask : public UnpausedTask
		{
		public:
			double amount;

			void Run() override
			{
				RE::UI* ui = RE::UI::GetSingleton();

				if (ui->IsMenuOpen(RE::GiftMenu::MENU_NAME))
				{
					RE::IMenu* menu = ui->GetMenu(RE::GiftMenu::MENU_NAME).get();

					RE::GFxValue arg = this->amount;
					const RE::FxDelegateArgs args(0, menu, menu->uiMovie.get(), &arg, 1);
					_ItemSelect(args);
				}
			}
		};

		std::shared_ptr<ItemSelectTask> task = std::make_shared<ItemSelectTask>();
		task->amount = a_args[0].GetNumber();

		UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
		queue->AddTask(task);
	}

	void GiftMenuEx::UpdateBottomBar()
	{
		using func_t = decltype(&GiftMenuEx::UpdateBottomBar);
		REL::Relocation<func_t> func(Offsets::Menus::GiftMenu::UpdateBottomBar);
		return func(this);
	}

	RE::IMenu* GiftMenuEx::Creator()
	{
		RE::GiftMenu* menu = static_cast<RE::GiftMenu*>(CreateMenu(RE::GiftMenu::MENU_NAME));

		RE::FxDelegate* dlg = menu->fxDelegate.get();
		_ItemSelect = dlg->callbacks.GetAlt("ItemSelect")->callback;
		dlg->callbacks.GetAlt("ItemSelect")->callback = ItemSelect_Hook;

		RE::RefHandle handle = menu->GetTargetRefHandle();
		RE::TESObjectREFRPtr refptr = nullptr;
		RE::TESObjectREFR* ref = nullptr;
		if (RE::TESObjectREFR::LookupByHandle(handle, refptr))
		{
			ref = refptr.get();
		}

		HUDMenuEx* hudMenu = static_cast<HUDMenuEx*>(RE::UI::GetSingleton()->GetMenu(RE::InterfaceStrings::GetSingleton()->hudMenu).get());
		if (hudMenu)
		{
			hudMenu->SetSkyrimSoulsMode(true);
		}

		AutoCloseManager* autoCloseManager = AutoCloseManager::GetSingleton();
		autoCloseManager->InitAutoClose(RE::ContainerMenu::MENU_NAME, ref, true);

		return menu;
	}

	void GiftMenuEx::InstallHook()
	{
		REL::Relocation<std::uintptr_t> vTable(Offsets::Menus::GiftMenu::Vtbl);
		_ProcessMessage = vTable.write_vfunc(0x4, &GiftMenuEx::ProcessMessage_Hook);
		_AdvanceMovie = vTable.write_vfunc(0x5, &GiftMenuEx::AdvanceMovie_Hook);
	}
}
