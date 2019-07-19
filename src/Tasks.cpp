#include "skse64/PluginAPI.h" //SKSETaskInterface
#include "skse64/GameData.h" //BGSSaveLoadManager
#include "skse64/gamethreads.h" //TaskDelegate

#include "RE/FxDelegateArgs.h" //FxDelegateArgs
#include "RE/TESForm.h" //TESForm::LookupByID
#include "RE/Actor.h" //Actor
#include "RE/MenuManager.h" //MenuManager
#include "RE/UIStringHolder.h" //UIStringHolder
#include "RE/IMenu.h" //IMenu
#include "RE/PlayerCharacter.h" //PlayerCharacter

#include "Tasks.h"
#include "Offsets.h"

#include <thread> //std::this_thread::sleep_for
#include <chrono> //std::chrono::seconds

namespace Tasks
{
	SKSETaskInterface * g_task = nullptr;

	static bool sleepWaitInProgress = false;
	static bool saveInProgress = false;
	static bool updateInventoryInProgress = false;

	//SleepWaitMenu
	SleepWaitDelegate::SleepWaitDelegate()
	{
	}

	void SleepWaitDelegate::Run()
	{
		static RE::MenuManager * mm = RE::MenuManager::GetSingleton();
		static RE::UIStringHolder * strHolder = RE::UIStringHolder::GetSingleton();

		RE::FxDelegateArgs * args;

		RE::IMenu * sleepWaitMenu = mm->GetMenu(strHolder->sleepWaitMenu);
		if (sleepWaitMenu) {
			SleepWaitDelegate * task = new SleepWaitDelegate();

			RE::GFxValue responseID, time;
			responseID.SetNumber(0);
			time.SetNumber(sleepWaitTime);

			args = new RE::FxDelegateArgs(responseID, sleepWaitMenu, sleepWaitMenu->view, &time, 1);
		} else {
			return;
		}

		RequestSleepWait_Original = reinterpret_cast<void(*)(RE::FxDelegateArgs*)>(Offsets::StartSleepWait_Original.GetUIntPtr());
		RequestSleepWait_Original(args);
		sleepWaitInProgress = false;
		delete(args);
	}

	void SleepWaitDelegate::Dispose()
	{
		delete this;
	}

	bool SleepWaitDelegate::RegisterTask(RE::FxDelegateArgs * a_args)
	{
		if (!sleepWaitInProgress) {
			sleepWaitInProgress = true;

			SleepWaitDelegate * task = new SleepWaitDelegate();
			task->sleepWaitTime = a_args->operator[](0).GetNumber();
			g_task->AddTask(task);

			return true;
		}
		return false;
	}

	// Save Game (JournalMenu)
	SaveGameDelegate::SaveGameDelegate()
	{
	}

	void SaveGameDelegate::Run()
	{
		BGSSaveLoadManager * slm = BGSSaveLoadManager::GetSingleton();
		slm->Save(this->saveName);
		saveInProgress = false;
	}

	void SaveGameDelegate::Dispose()
	{
		delete this;
	}

	bool SaveGameDelegate::RegisterTask(const char * a_name)
	{
		if (!saveInProgress) {
			saveInProgress = true;
			SaveGameDelegate * task = new SaveGameDelegate();

			if (a_name) {
				const size_t len = strlen(a_name);
				char * tmp = new char[len + 1];
				strncpy_s(tmp, len + 1, a_name, len);
				task->saveName = tmp;
			}
			else {
				task->saveName = nullptr;
			}

			g_task->AddTask(task);
			return true;
		}
		return false;
	}

	//Serve Time (MessageBoxMenu)
	ServeTimeDelegate::ServeTimeDelegate()
	{
	}

	void ServeTimeDelegate::Run()
	{
		RE::Actor * player = reinterpret_cast<RE::Actor*>(RE::TESForm::LookupByID(0x00000014));
		player->ServeJailTime();
		RE::MenuManager * mm = RE::MenuManager::GetSingleton();
		mm->numPauseGame--;
	}

	void ServeTimeDelegate::Dispose()
	{
		delete this;
	}

	void ServeTimeDelegate::RegisterAsyncTask()
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		ServeTimeDelegate * task = new ServeTimeDelegate();
		g_task->AddTask(task);
	}

	void ServeTimeDelegate::RegisterTask()
	{
		static RE::MenuManager * mm = RE::MenuManager::GetSingleton();
		mm->numPauseGame++;
		std::thread t(RegisterAsyncTask);
		t.detach();
	}

	UpdateInventoryDelegate::UpdateInventoryDelegate()
	{

	}

	void UpdateInventoryDelegate::Run()
	{
		static RE::MenuManager * mm = RE::MenuManager::GetSingleton();
		static RE::UIStringHolder * strHolder = RE::UIStringHolder::GetSingleton();
		static RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();

		player->processManager->UpdateEquipment(player);
		(this->containerOwner)->processManager->UpdateEquipment(this->containerOwner);

		UpdateInventory_Original = reinterpret_cast<void(*)(uintptr_t, RE::PlayerCharacter*)>(Offsets::UpdateInventory_Original.GetUIntPtr());
		UpdateInventory_Original(this->unkArg, player);
		updateInventoryInProgress = false;
	}

	void UpdateInventoryDelegate::Dispose()
	{
		delete this;
	}

	bool UpdateInventoryDelegate::RegisterTask(uintptr_t a_unkArg, RE::Actor* a_containerOwner)
	{
		if (!updateInventoryInProgress)
		{
			updateInventoryInProgress = true;
			UpdateInventoryDelegate * task = new UpdateInventoryDelegate();
			task->unkArg = a_unkArg;
			task->containerOwner = a_containerOwner;
			g_task->AddTask(task);
			return true;
		}
		return false;
	}
}
