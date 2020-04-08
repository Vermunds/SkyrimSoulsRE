#include "skse64/gamethreads.h" //TaskDelegate

#include "RE/Skyrim.h"

#include "Tasks.h"
#include "Offsets.h"
#include "Hooks.h"
#include "BGSSaveLoadManagerEx.h"

#include <thread> //std::this_thread::sleep_for
#include <chrono> //std::chrono::seconds

#include <SKSE\API.h>

namespace SkyrimSoulsRE::Tasks
{

	//SleepWaitMenu
	SleepWaitDelegate::SleepWaitDelegate()
	{
	}

	void SleepWaitDelegate::Run()
	{
		RE::UI* ui = RE::UI::GetSingleton();
		RE::InterfaceStrings* interfaceStrings = RE::InterfaceStrings::GetSingleton();

		const RE::FxDelegateArgs * args;

		RE::IMenu * sleepWaitMenu = ui->GetMenu(interfaceStrings->sleepWaitMenu).get();
		if (sleepWaitMenu) {

			RE::GFxValue time = sleepWaitTime;
			UInt32 numArgs = 1;

			args = new RE::FxDelegateArgs(RE::GFxValue(), sleepWaitMenu, sleepWaitMenu->view.get(), &time, numArgs);
		} else {
			return;
		}

		RequestSleepWait_Original = reinterpret_cast<void(*)(const RE::FxDelegateArgs&)>(Offsets::StartSleepWait_Original.GetUIntPtr());
		RequestSleepWait_Original(*args);
		delete(args);
	}

	void SleepWaitDelegate::Dispose()
	{
		delete this;
	}

	void SleepWaitDelegate::RegisterTask(const RE::FxDelegateArgs& a_args)
	{
		SleepWaitDelegate * task = new SleepWaitDelegate();
		task->sleepWaitTime = a_args[0].GetNumber();

		auto taskInterface = SKSE::GetTaskInterface();
		taskInterface->AddTask(task);
	}

	SaveGameDelegate::SaveGameDelegate()
	{
	}

	void SaveGameDelegate::Run()
	{
		using SaveType = BGSSaveLoadManagerEx::SaveType;
		using OutputFlag = BGSSaveLoadManagerEx::OutputFlag;

		//Save
		BGSSaveLoadManagerEx* slm = static_cast<BGSSaveLoadManagerEx*>(RE::BGSSaveLoadManager::GetSingleton());
		slm->SaveGame(SaveType::kSave, this->outputFlag, this->saveName);
	}

	void SaveGameDelegate::Dispose()
	{
		delete this;
	}

	void SaveGameDelegate::RegisterTask(BGSSaveLoadManagerEx::OutputFlag a_outputFlag, const char* a_name)
	{
		//Create save screenshot
		void(*RequestScreenshot_Original)();
		RequestScreenshot_Original = reinterpret_cast<void(*)()>(Offsets::RequestScreenshot_Original.GetUIntPtr());
		RequestScreenshot_Original();

		void(*CreateScreenshot_Original)();
		CreateScreenshot_Original = reinterpret_cast<void(*)()>(Offsets::CreateScreenshot_Original.GetUIntPtr());
		CreateScreenshot_Original();

		SaveGameDelegate* task = new SaveGameDelegate();

		if (a_name) {
			const size_t len = strlen(a_name);
			char* tmp = new char[len + 1];
			strncpy_s(tmp, len + 1, a_name, len);
			task->saveName = tmp;
		}
		else {
			task->saveName = nullptr;
		}

		task->outputFlag = a_outputFlag;
		auto taskInterface = SKSE::GetTaskInterface();
		taskInterface->AddTask(task);
	}

	ServeTimeDelegate::ServeTimeDelegate()
	{
	}

	void ServeTimeDelegate::Run()
	{
		RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
		player->ServePrisonTime();
	}

	void ServeTimeDelegate::Dispose()
	{
		delete this;
	}

	void ServeTimeDelegate::RegisterTask()
	{
		ServeTimeDelegate* task = new ServeTimeDelegate();
		auto taskInterface = SKSE::GetTaskInterface();
		taskInterface->AddTask(task);
	}


	UpdateInventoryDelegate::UpdateInventoryDelegate()
	{

	}

	void UpdateInventoryDelegate::Run()
	{
		RE::UI* ui = RE::UI::GetSingleton();
		RE::InterfaceStrings * interfaceStrings = RE::InterfaceStrings::GetSingleton();
		RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();

		player->currentProcess->Update3DModel(player);
		(this->containerOwner)->currentProcess->Update3DModel(this->containerOwner);
		(this->list)->Update(player);
	}

	void UpdateInventoryDelegate::Dispose()
	{
		delete this;
	}

	void UpdateInventoryDelegate::RegisterTask(RE::ItemList* a_list, RE::Actor* a_containerOwner)
	{
		UpdateInventoryDelegate * task = new UpdateInventoryDelegate();
		task->list = a_list;
		task->containerOwner = a_containerOwner;
		auto taskInterface = SKSE::GetTaskInterface();
		taskInterface->AddTask(task);;
	}

	MessageBoxButtonPressDelegate::MessageBoxButtonPressDelegate()
	{

	}

	void MessageBoxButtonPressDelegate::Run()
	{
		RE::UI* ui = RE::UI::GetSingleton();
		RE::InterfaceStrings* interfaceStrings = RE::InterfaceStrings::GetSingleton();

		const RE::FxDelegateArgs* args;

		RE::IMenu* messageBoxMenu = ui->GetMenu(interfaceStrings->messageBoxMenu).get();
		if (messageBoxMenu) {

			RE::GFxValue index = this->selectedIndex;
			UInt32 numArgs = 1;

			args = new RE::FxDelegateArgs(RE::GFxValue(), messageBoxMenu, messageBoxMenu->view.get(), &index, numArgs);
		}
		else {
			return;
		}

		void(*MessageBoxButtonPress_Original)(const RE::FxDelegateArgs&);
		MessageBoxButtonPress_Original = reinterpret_cast<void(*)(const RE::FxDelegateArgs&)>(Offsets::MessageBoxButtonPress_Original.GetUIntPtr());
		MessageBoxButtonPress_Original(*args);
		delete(args);
	}
	void MessageBoxButtonPressDelegate::Dispose()
	{
		delete this;
	}
	void MessageBoxButtonPressDelegate::RegisterTask(const RE::FxDelegateArgs& a_args)
	{
		if (a_args.GetArgCount() == 1 && a_args[0].IsNumber())
		{
			MessageBoxButtonPressDelegate* task = new MessageBoxButtonPressDelegate();
			task->selectedIndex = a_args[0].GetNumber();
			auto taskInterface = SKSE::GetTaskInterface();
			taskInterface->AddTask(task);
		}
	}
}
