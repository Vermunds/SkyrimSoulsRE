#include "skse64/PluginAPI.h" //SKSETaskInterface
#include "skse64/GameData.h" //BGSSaveLoadManager
#include "skse64/gamethreads.h" //TaskDelegate

#include "RE/FxDelegateArgs.h" //FxDelegateArgs
#include "RE/TESForm.h" //TESForm::LookupByID
#include "RE/Actor.h" //Actor

#include "Tasks.h"
#include "Offsets.h"

namespace Tasks
{
	SKSETaskInterface * g_task = nullptr;

	static bool sleepWaitInProgress = false;
	static bool saveInProgress = false;

	//SleepWaitMenu
	SleepWaitDelegate::SleepWaitDelegate()
	{
	}

	void SleepWaitDelegate::Run()
	{
		RequestSleepWait_Original = reinterpret_cast<void(*)(RE::FxDelegateArgs*)>(Offsets::StartSleepWait_Original.GetUIntPtr());
		RequestSleepWait_Original(this->args);
		sleepWaitInProgress = false;
		delete(this->args);
	}

	void SleepWaitDelegate::Dispose()
	{
		delete this;
	}

	bool SleepWaitDelegate::RegisterTask(RE::FxDelegateArgs * p_args)
	{
		if (!sleepWaitInProgress) {
			sleepWaitInProgress = true;
			SleepWaitDelegate * task = new SleepWaitDelegate();

			double sleepWaitTime = static_cast<double>((p_args->operator[](0)).GetNumber());

			RE::GFxValue responseID, time;
			responseID.SetNumber(0);
			time.SetNumber(sleepWaitTime);

			task->args = new RE::FxDelegateArgs(responseID, p_args->GetHandler(), p_args->GetMovie(), &time, 1);
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

	bool SaveGameDelegate::RegisterTask(const char * name)
	{
		if (!saveInProgress) {
			saveInProgress = true;
			SaveGameDelegate * task = new SaveGameDelegate();

			if (name) {
				const size_t len = strlen(name);
				char * tmp = new char[len + 1];
				strncpy_s(tmp, len + 1, name, len);
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
	}

	void ServeTimeDelegate::Dispose()
	{
		delete this;
	}

	void ServeTimeDelegate::RegisterTask()
	{
		ServeTimeDelegate * task = new ServeTimeDelegate();
		g_task->AddTask(task);
	}
}
