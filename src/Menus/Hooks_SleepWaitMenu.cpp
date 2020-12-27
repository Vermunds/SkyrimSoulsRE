#include "Menus/Hooks_SleepWaitMenu.h"
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

		RE::GFxValue dateText;
		this->uiMovie->GetVariable(&dateText, "_root.SleepWaitMenu_mc.CurrentTime");
		RE::GFxValue newDate(timeDateString);
		dateText.SetMember("htmlText", newDate);
	}

	void SleepWaitMenuEx::StartSleepWait_Hook(const RE::FxDelegateArgs& a_args)
	{
		class StartSleepWaitTask : public UnpausedTask
		{
			double sleepWaitTime;

		public:
			StartSleepWaitTask(double a_time)
			{
				this->sleepWaitTime = a_time;
				this->beginTime = std::chrono::high_resolution_clock::now();			
			}

			void Run() override
			{
				RE::UI* ui = RE::UI::GetSingleton();

				if (ui->IsMenuOpen(RE::SleepWaitMenu::MENU_NAME))
				{
					RE::SleepWaitMenu* menu = static_cast<RE::SleepWaitMenu*>(ui->GetMenu(RE::SleepWaitMenu::MENU_NAME).get());
					RE::GFxValue time = this->sleepWaitTime;
					RE::FxDelegateArgs args(0, menu, menu->uiMovie.get(), &time, 1);
					_StartSleepWait(args);
				}
			}
		};

		RE::UI* ui = RE::UI::GetSingleton();
		Settings* settings = Settings::GetSingleton();

		RE::SleepWaitMenu* menu = static_cast<RE::SleepWaitMenu*>(a_args.GetHandler());
		if (!menu->PausesGame())
		{
			menu->menuFlags |= RE::IMenu::Flag::kPausesGame;
			ui->numPausesGame++;
		}

		StartSleepWaitTask* task = new StartSleepWaitTask(a_args[0].GetNumber());
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
		REL::Relocation<std::uintptr_t> vTable(Offsets::Menus::SleepWaitMenu::Vtbl);
		_AdvanceMovie = vTable.write_vfunc(0x5, &SleepWaitMenuEx::AdvanceMovie_Hook);

		//Hook ProcessMessage
		_ProcessMessage = vTable.write_vfunc(0x4, &SleepWaitMenuEx::ProcessMessage_Hook);

		//Fix for slow menu in slow motion
		std::uint32_t ptr = static_cast<std::uint32_t>(Offsets::Misc::SecondsSinceLastFrame_RealTime.address() - (Offsets::Menus::SleepWaitMenu::ProcessMessage.address() + 0x1C0 + 0x8));
		REL::safe_write(Offsets::Menus::SleepWaitMenu::ProcessMessage.address() + 0x1C0, ptr + 0x4);
	}
}
