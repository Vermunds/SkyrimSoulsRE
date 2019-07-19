#pragma once

#include "skse64/PluginAPI.h" //SKSETaskInterface
#include "skse64/gamethreads.h" //TaskDelegate

#include "RE/FxDelegateArgs.h" //FxDelegateArgs
#include "RE/Actor.h" //Actor
#include "RE/PlayerCharacter.h" //PlayerCharacter

namespace Tasks
{
	extern SKSETaskInterface * g_task;

	class SleepWaitDelegate : TaskDelegate
	{
	private:
		void(*RequestSleepWait_Original)(RE::FxDelegateArgs*);
		double sleepWaitTime;
		SleepWaitDelegate();
	public:
		void Run() override;
		void Dispose() override;
		static bool RegisterTask(RE::FxDelegateArgs * p_arg);
	};

	class SaveGameDelegate : TaskDelegate
	{
	private:
		const char * saveName = nullptr;
		SaveGameDelegate();
	public:
		void Run() override;
		void Dispose() override;
		static bool RegisterTask(const char * a_name);
	};

	class ServeTimeDelegate : TaskDelegate
	{
	private:
		ServeTimeDelegate();
		static void RegisterAsyncTask();
	public:
		void Run() override;
		void Dispose() override;
		static void RegisterTask();
	};

	class UpdateInventoryDelegate : TaskDelegate
	{
	private:
		uintptr_t unkArg; //this ptr
		RE::Actor* containerOwner;
		void(*UpdateInventory_Original)(uintptr_t, RE::PlayerCharacter*);
		UpdateInventoryDelegate();
	public:
		void Run() override;
		void Dispose() override;
		static bool RegisterTask(uintptr_t a_unkArg, RE::Actor* a_containerOwner);
	};
}
