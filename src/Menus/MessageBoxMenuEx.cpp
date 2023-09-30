#include "Menus/MessageBoxMenuEx.h"

namespace SkyrimSoulsRE
{
	void MessageBoxMenuEx::ButtonPress_Hook(const RE::FxDelegateArgs& a_args)
	{
		if (a_args.GetArgCount() == 1 && a_args[0].IsNumber())
		{
			double selectedIndex = a_args[0].GetNumber();

			auto task = [selectedIndex]() {
				RE::UI* ui = RE::UI::GetSingleton();

				if (ui->IsMenuOpen(RE::MessageBoxMenu::MENU_NAME))
				{
					RE::MessageBoxMenu* menu = static_cast<RE::MessageBoxMenu*>(ui->GetMenu(RE::MessageBoxMenu::MENU_NAME).get());
					RE::GFxValue index = selectedIndex;
					RE::FxDelegateArgs args(0, menu, menu->uiMovie.get(), &index, 1);
					_ButtonPress(args);
				}
			};

			UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
			queue->AddTask(task);
		}
	}

	void MessageBoxMenuEx::AdvanceMovie_Hook(float a_interval, std::uint32_t a_currentTime)
	{
		HUDMenuEx* hudMenu = static_cast<HUDMenuEx*>(RE::UI::GetSingleton()->GetMenu(RE::HUDMenu::MENU_NAME).get());
		if (hudMenu)
		{
			hudMenu->UpdateHUD();
		}

		return _AdvanceMovie(this, a_interval, a_currentTime);
	}

	RE::IMenu* MessageBoxMenuEx::Creator()
	{
		RE::MessageBoxMenu* menu = static_cast<RE::MessageBoxMenu*>(CreateMenu(RE::MessageBoxMenu::MENU_NAME));

		RE::FxDelegate* dlg = menu->fxDelegate.get();
		_ButtonPress = dlg->callbacks.GetAlt("buttonPress")->callback;
		dlg->callbacks.GetAlt("buttonPress")->callback = ButtonPress_Hook;

		return menu;
	}

	void MessageBoxMenuEx::InstallHook()
	{
		//Hook AdvanceMovie
		REL::Relocation<std::uintptr_t> vTable(RE::VTABLE_MessageBoxMenu[0]);
		_AdvanceMovie = vTable.write_vfunc(0x5, &MessageBoxMenuEx::AdvanceMovie_Hook);
	}
}
