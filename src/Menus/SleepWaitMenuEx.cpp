#include "Menus/SleepWaitMenuEx.h"
#include "SlowMotionHandler.h"

namespace SkyrimSoulsRE
{
	RE::UI_MESSAGE_RESULTS SleepWaitMenuEx::ProcessMessage_Hook(RE::UIMessage& a_message)
	{
		if (a_message.type == RE::UI_MESSAGE_TYPE::kScaleformEvent)
		{
			return this->RE::IMenu::ProcessMessage(a_message);
		}
		return _ProcessMessage(this, a_message);
	}

	void SleepWaitMenuEx::AdvanceMovie_Hook(float a_interval, std::uint32_t a_currentTime)
	{
		HUDMenuEx* hudMenu = static_cast<HUDMenuEx*>(RE::UI::GetSingleton()->GetMenu(RE::HUDMenu::MENU_NAME).get());
		if (hudMenu)
		{
			hudMenu->UpdateHUD();
		}
		if (!this->PausesGame())
		{
			this->UpdateClock();
		}

		return _AdvanceMovie(this, a_interval, a_currentTime);
	}

	void SleepWaitMenuEx::UpdateClock()
	{
		char timeDateString[200];
		RE::Calendar::GetSingleton()->GetTimeDateString(timeDateString, 200, false);

		RE::GFxValue args[1];
		args[0].SetString(timeDateString);
		this->uiMovie->Invoke("_root.SleepWaitMenu_mc.SetCurrentTime", nullptr, args, 1);
	}

	void SleepWaitMenuEx::StartSleepWait_Hook(const RE::FxDelegateArgs& a_args)
	{
		RE::UI* ui = RE::UI::GetSingleton();
		RE::BSSpinLockGuard lk(ui->processMessagesLock);

		RE::SleepWaitMenu* menu = static_cast<RE::SleepWaitMenu*>(a_args.GetHandler());
		if (!menu->PausesGame())
		{
			menu->menuFlags.set(RE::IMenu::Flag::kPausesGame);
			ui->numPausesGame++;
		}

		double sleepWaitTime = a_args[0].GetNumber();

		auto task = [sleepWaitTime]() {
			RE::UI* ui = RE::UI::GetSingleton();

			if (ui->IsMenuOpen(RE::SleepWaitMenu::MENU_NAME))
			{
				RE::SleepWaitMenu* menu = static_cast<RE::SleepWaitMenu*>(ui->GetMenu(RE::SleepWaitMenu::MENU_NAME).get());
				RE::GFxValue time = sleepWaitTime;
				RE::FxDelegateArgs args(0, menu, menu->uiMovie.get(), &time, 1);
				_StartSleepWait(args);
			}
		};

		UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
		queue->AddTask(task);
	}

	RE::IMenu* SleepWaitMenuEx::Creator()
	{
		RE::SleepWaitMenu* menu = static_cast<RE::SleepWaitMenu*>(CreateMenu(RE::SleepWaitMenu::MENU_NAME));

		RE::FxDelegate* dlg = menu->fxDelegate.get();
		_StartSleepWait = dlg->callbacks.GetAlt("OK")->callback;
		dlg->callbacks.GetAlt("OK")->callback = StartSleepWait_Hook;

		return menu;
	}
	void SleepWaitMenuEx::InstallHook()
	{
		//Hook AdvanceMovie
		REL::Relocation<std::uintptr_t> vTable(RE::VTABLE_SleepWaitMenu[0]);
		_AdvanceMovie = vTable.write_vfunc(0x5, &SleepWaitMenuEx::AdvanceMovie_Hook);

		//Hook ProcessMessage
		_ProcessMessage = vTable.write_vfunc(0x4, &SleepWaitMenuEx::ProcessMessage_Hook);
	}
}
