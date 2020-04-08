#include "RE/FxDelegateArgs.h"
#include "RE/MessageBoxMenu.h"
#include "Tasks.h"

namespace SkyrimSoulsRE::Hooks
{
	namespace MessageBoxMenu
	{
		void ButtonPress_Hook(const RE::FxDelegateArgs& a_args)
		{
			Tasks::MessageBoxButtonPressDelegate::RegisterTask(a_args);
		}

		void Register_Func(RE::MessageBoxMenu* a_MessageBoxMenu)
		{
			RE::FxDelegate* dlg = a_MessageBoxMenu->fxDelegate.get();
			dlg->callbacks.GetAlt("buttonPress")->callback = ButtonPress_Hook; //method name lowercase
		}
	}
}