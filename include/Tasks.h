#pragma once

#include "skse64/PluginAPI.h" //SKSETaskInterface
#include "skse64/gamethreads.h" //TaskDelegate

#include "RE/FxDelegateArgs.h" //FxDelegateArgs

namespace Tasks
{
	extern SKSETaskInterface * g_task;

	class SleepWaitDelegate : TaskDelegate
	{
	public:
		void(*RequestSleepWait_Original)(RE::FxDelegateArgs*);
		RE::FxDelegateArgs * args;

		SleepWaitDelegate();
		void Run() override;
		void Dispose() override;
		static bool RegisterTask(RE::FxDelegateArgs * p_arg);
	};

	class SaveGameDelegate : TaskDelegate
	{
	public:
		const char * saveName = nullptr;

		SaveGameDelegate();
		void Run() override;
		void Dispose() override;
		static bool RegisterTask(const char * name);
	};

	class ServeTimeDelegate : TaskDelegate
	{
	public:
		ServeTimeDelegate();
		void Run() override;
		void Dispose() override;
		static void RegisterTask();
	};
}
