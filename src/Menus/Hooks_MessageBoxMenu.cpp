#include "Menus/Hooks_MessageBoxMenu.h"

namespace SkyrimSoulsRE
{
	void MessageBoxMenuEx::ButtonPress_Hook(const RE::FxDelegateArgs& a_args)
	{
		class ButtonPressTask : public UnpausedTask
		{
			double selectedIndex;
		public:
			ButtonPressTask(double a_selectedIndex)
			{
				this->selectedIndex = a_selectedIndex;
				this->beginTime = std::chrono::high_resolution_clock::now();
				this->delayTimeMS = std::chrono::milliseconds(0);
			}

			void Run() override
			{
				RE::UI* ui = RE::UI::GetSingleton();

				if (ui->IsMenuOpen(RE::MessageBoxMenu::MENU_NAME))
				{
					RE::MessageBoxMenu* menu = static_cast<RE::MessageBoxMenu*>(ui->GetMenu(RE::MessageBoxMenu::MENU_NAME).get());
					RE::GFxValue index = this->selectedIndex;
					RE::FxDelegateArgs args(0, menu, menu->uiMovie.get(), &index, 1);
					_ButtonPress(args);
				}
			}
		};

		if (a_args.GetArgCount() == 1 && a_args[0].IsNumber())
		{
			UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
			ButtonPressTask* task = new ButtonPressTask(a_args[0].GetNumber());
			queue->AddTask(task);
		}
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

	}
}
