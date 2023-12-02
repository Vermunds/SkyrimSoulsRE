#include "Menus/BarterMenuEx.h"

namespace SkyrimSoulsRE
{
	RE::UI_MESSAGE_RESULTS BarterMenuEx::ProcessMessage_Hook(RE::UIMessage& a_message)
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
	void BarterMenuEx::AdvanceMovie_Hook(float a_interval, std::uint32_t a_currentTime)
	{
		//this->UpdateBottomBar();
		AutoCloseManager::GetSingleton()->CheckAutoClose(RE::BarterMenu::MENU_NAME);
		return _AdvanceMovie(this, a_interval, a_currentTime);
	}

	void BarterMenuEx::ItemSelect_Hook(const RE::FxDelegateArgs& a_args)
	{
		double count = a_args[0].GetNumber();
		double value = a_args[1].GetNumber();
		bool isViewingVendorItems = a_args[2].GetBool();

		auto task = [count, value, isViewingVendorItems]() {
			RE::UI* ui = RE::UI::GetSingleton();
			RE::BSSpinLockGuard lk(ui->processMessagesLock);

			if (ui->IsMenuOpen(RE::BarterMenu::MENU_NAME))
			{
				BarterMenuEx* menu = static_cast<BarterMenuEx*>(ui->GetMenu(RE::BarterMenu::MENU_NAME).get());

				RE::GFxValue arg[3];
				arg[0] = count;
				arg[1] = value;
				arg[2] = isViewingVendorItems;
				const RE::FxDelegateArgs args(0, menu, menu->uiMovie.get(), arg, 3);
				BarterMenuEx::_ItemSelect(args);
			}
		};

		UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
		queue->AddTask(task);
	}

	void BarterMenuEx::UpdateBottomBar()
	{
		using func_t = decltype(&BarterMenuEx::UpdateBottomBar);
		REL::Relocation<func_t> func(Offsets::Menus::BarterMenu::UpdateBottomBar);
		return func(this);
	}

	RE::IMenu* BarterMenuEx::Creator()
	{
		RE::BarterMenu* menu = static_cast<RE::BarterMenu*>(CreateMenu(RE::BarterMenu::MENU_NAME));

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
		autoCloseManager->InitAutoClose(RE::BarterMenu::MENU_NAME, ref, true);

		return menu;
	}

	void BarterMenuEx::InstallHook()
	{
		//Hook AdvanceMovie + ProcessMessage
		REL::Relocation<std::uintptr_t> vTable(RE::VTABLE_BarterMenu[0]);
		_ProcessMessage = vTable.write_vfunc(0x4, &BarterMenuEx::ProcessMessage_Hook);
		_AdvanceMovie = vTable.write_vfunc(0x5, &BarterMenuEx::AdvanceMovie_Hook);
	}
}
