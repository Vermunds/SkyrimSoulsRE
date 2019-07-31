#include "skse64/PluginAPI.h" //SKSETaskInterface
#include "skse64/gamethreads.h" //TaskDelegate

#include "RE/Skyrim.h"

#include "Tasks.h"
#include "Offsets.h"
#include "Hooks.h"

#include <thread> //std::this_thread::sleep_for
#include <chrono> //std::chrono::seconds

namespace Tasks
{
	SKSETaskInterface * g_task = nullptr;

	static bool updateInventoryInProgress = false;

	//SleepWaitMenu
	SleepWaitDelegate::SleepWaitDelegate()
	{
	}

	void SleepWaitDelegate::Run()
	{
		RE::MenuManager * mm = RE::MenuManager::GetSingleton();
		RE::UIStringHolder * strHolder = RE::UIStringHolder::GetSingleton();

		const RE::FxDelegateArgs * args;

		RE::IMenu * sleepWaitMenu = mm->GetMenu(strHolder->sleepWaitMenu).get();
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
		g_task->AddTask(task);
	}

	SaveGameDelegate::SaveGameDelegate()
	{
	}

	void SaveGameDelegate::Run()
	{
		bool(*SaveGame_Original)(RE::BGSSaveLoadManager*, Hooks::BGSSaveLoadManagerEx::SaveMode, Hooks::BGSSaveLoadManagerEx::DumpFlag, const char*, UInt32);
		SaveGame_Original = reinterpret_cast<bool(*)(RE::BGSSaveLoadManager*, Hooks::BGSSaveLoadManagerEx::SaveMode, Hooks::BGSSaveLoadManagerEx::DumpFlag, const char*, UInt32)>(Offsets::SaveGame_Original.GetUIntPtr());
		SaveGame_Original(RE::BGSSaveLoadManager::GetSingleton(), Hooks::BGSSaveLoadManagerEx::kSave, this->dumpFlag, this->saveName, 0);
	}

	void SaveGameDelegate::Dispose()
	{
		delete this;
	}

	void SaveGameDelegate::RegisterTask(Hooks::BGSSaveLoadManagerEx::DumpFlag a_dumpFlag, const char* a_name)
	{
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

		task->dumpFlag = a_dumpFlag;
		g_task->AddTask(task);
	}


	//Serve Time (MessageBoxMenu)
	ServeTimeDelegate::ServeTimeDelegate()
	{
	}

	void ServeTimeDelegate::Run()
	{
		RE::Actor* player = reinterpret_cast<RE::Actor*>(RE::TESForm::LookupByID(0x00000014));
		player->ServeJailTime();
		RE::MenuManager* mm = RE::MenuManager::GetSingleton();
		mm->numPauseGame--;
	}

	void ServeTimeDelegate::Dispose()
	{
		delete this;
	}

	void ServeTimeDelegate::RegisterAsyncTask()
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		ServeTimeDelegate* task = new ServeTimeDelegate();
		g_task->AddTask(task);
	}

	void ServeTimeDelegate::RegisterTask()
	{
		RE::MenuManager* mm = RE::MenuManager::GetSingleton();
		mm->numPauseGame++;
		std::thread t(RegisterAsyncTask);
		t.detach();
	}

	UpdateInventoryDelegate::UpdateInventoryDelegate()
	{

	}

	void UpdateInventoryDelegate::Run()
	{
		RE::MenuManager * mm = RE::MenuManager::GetSingleton();
		RE::UIStringHolder * strHolder = RE::UIStringHolder::GetSingleton();
		RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();

		player->processManager->UpdateEquipment(player);
		(this->containerOwner)->processManager->UpdateEquipment(this->containerOwner);

		UpdateInventory_Original = reinterpret_cast<void(*)(void*, RE::PlayerCharacter*)>(RE::Offset::InventoryMenu::InventoryData::Update);
		UpdateInventory_Original(this->unk, player);
		updateInventoryInProgress = false;
	}

	void UpdateInventoryDelegate::Dispose()
	{
		delete this;
	}

	bool UpdateInventoryDelegate::RegisterTask(void* a_unk, RE::Actor* a_containerOwner)
	{
		if (!updateInventoryInProgress)
		{
			updateInventoryInProgress = true;
			UpdateInventoryDelegate * task = new UpdateInventoryDelegate();
			task->unk = a_unk;
			task->containerOwner = a_containerOwner;
			g_task->AddTask(task);
			return true;
		}
		return false;
	}
}
