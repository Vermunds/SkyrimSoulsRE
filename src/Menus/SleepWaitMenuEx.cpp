#include "Menus/SleepWaitMenuEx.h"
#include "AutoCloseManager.h"
#include "SlowMotionHandler.h"
#include "Util.h"

namespace SkyrimSoulsRE
{
	RE::UI_MESSAGE_RESULTS SleepWaitMenuEx::ProcessMessage_Hook(RE::UIMessage& a_message)
	{
		switch (a_message.type.get())
		{
		case RE::UI_MESSAGE_TYPE::kScaleformEvent:
			return this->RE::IMenu::ProcessMessage(a_message);

		case RE::UI_MESSAGE_TYPE::kUpdate:
			{
				auto hudMenu = static_cast<HUDMenuEx*>(RE::UI::GetSingleton()->GetMenu(RE::HUDMenu::MENU_NAME).get());
				if (hudMenu)
				{
					hudMenu->UpdateHUD();
				}

				this->UpdateClock();

				if (bedReferenceMessageReceived)
				{
					AutoCloseManager::GetSingleton()->CheckAutoClose(RE::SleepWaitMenu::MENU_NAME);
				}
				break;
			}

		case SET_BED_REFERENCE_MESSAGE_TYPE:
			{
				auto messageData = static_cast<RE::BSUIMessageData*>(a_message.data);
				RE::RefHandle bedRefHandle = static_cast<RE::RefHandle>(messageData->data.u);

				auto* autoCloseManager = AutoCloseManager::GetSingleton();
				autoCloseManager->InitAutoClose(RE::SleepWaitMenu::MENU_NAME, bedRefHandle, true);
				bedReferenceMessageReceived = true;
				break;
			}

		default:
			break;
		}

		return _ProcessMessage(this, a_message);
	}

	void SleepWaitMenuEx::UpdateClock()
	{
		char timeDateString[200];
		RE::Calendar::GetSingleton()->GetTimeDateString(timeDateString, sizeof(timeDateString), false);

		if (std::memcmp(lastTimeDateString, timeDateString, sizeof(timeDateString)) == 0)
		{
			return;
		}

		std::memcpy(lastTimeDateString, timeDateString, sizeof(timeDateString));

		RE::GFxValue args[1];
		args[0].SetString(timeDateString);
		this->uiMovie->Invoke("_root.SleepWaitMenu_mc.SetCurrentTime", nullptr, args, 1);
	}

	bool SleepWaitMenuEx::CanSleep_Hook(RE::PlayerCharacter* a_player, RE::TESObjectREFR* a_bedRef)
	{
		// This function is called in TESFurniture::Activate just before the menu is opened.
		// We can grab the reference here for auto-close.
		// This will not work if an other DLL mod opens the menu from elsewhere, like Go to Bed, but it should cover Vanilla logic (and it's not really an issue with that mod anyway).

		if (!_CanSleep(a_player, a_bedRef))
		{
			return false;
		}

		RE::UI* ui = RE::UI::GetSingleton();
		RE::UIMessageQueue* messageQueue = RE::UIMessageQueue::GetSingleton();

		if (ui->IsMenuOpen(RE::SleepWaitMenu::MENU_NAME))
		{
			messageQueue->AddMessage(RE::SleepWaitMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kHide, nullptr);
		}
		else
		{
			RE::BSUIMessageData* messageIsSleeping = RE::UIMessageDataFactory::Create<RE::BSUIMessageData>();
			messageIsSleeping->data.b = true;  // isSleeping
			messageQueue->AddMessage(RE::SleepWaitMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kShow, messageIsSleeping);

			RE::RefHandle bedRefHandle;
			RE::CreateRefHandle(bedRefHandle, a_bedRef);
			RE::BSUIMessageData* messageRef = RE::UIMessageDataFactory::Create<RE::BSUIMessageData>();
			messageRef->data.u = static_cast<std::uint32_t>(bedRefHandle);
			messageQueue->AddMessage(RE::SleepWaitMenu::MENU_NAME, SET_BED_REFERENCE_MESSAGE_TYPE, messageRef);
		}

		return false;  // skip the original menu open function
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
		bedReferenceMessageReceived = false;

		RE::FxDelegate* dlg = menu->fxDelegate.get();
		_StartSleepWait = dlg->callbacks.GetAlt("OK")->callback;
		dlg->callbacks.GetAlt("OK")->callback = StartSleepWait_Hook;

		return menu;
	}

	void SleepWaitMenuEx::InstallHook()
	{
		// Hook ProcessMessage
		REL::Relocation<std::uintptr_t> vTable(RE::VTABLE_SleepWaitMenu[0]);
		_ProcessMessage = vTable.write_vfunc(0x4, &SleepWaitMenuEx::ProcessMessage_Hook);

		// Hook CanSleep
		_CanSleep = SKSE::GetTrampoline().write_call<5>(Offsets::TESFurniture::Activate.address() + 0x16A, &SleepWaitMenuEx::CanSleep_Hook);
	}
}
