#pragma once

#include "skse64/gamethreads.h" //TaskDelegate

#include "Hooks.h"

#include "RE/FxDelegateArgs.h"
#include "RE/Actor.h"
#include "RE/PlayerCharacter.h"
#include "RE/ItemList.h"

namespace Tasks
{

	class SleepWaitDelegate : TaskDelegate
	{
	private:
		void(*RequestSleepWait_Original)(const RE::FxDelegateArgs&);
		double sleepWaitTime;
		SleepWaitDelegate();
	public:
		void Run() override;
		void Dispose() override;
		static void RegisterTask(const RE::FxDelegateArgs&);
	};

	class SaveGameDelegate : TaskDelegate
	{
	private:
		const char* saveName = nullptr;
		Hooks::BGSSaveLoadManagerEx::DumpFlag dumpFlag;
		SaveGameDelegate();
	public:
		void Run() override;
		void Dispose() override;
		static void RegisterTask(Hooks::BGSSaveLoadManagerEx::DumpFlag a_dumpFlag, const char* a_name);
	};

	//Used by console command 'ServeTime'
	class ServeTimeDelegate : TaskDelegate
	{
	private:
		ServeTimeDelegate();
	public:
		void Run() override;
		void Dispose() override;
		static void RegisterTask();
	};

	class UpdateInventoryDelegate : TaskDelegate
	{
	private:
		RE::ItemList* list;
		RE::Actor* containerOwner;
		UpdateInventoryDelegate();
	public:
		void Run() override;
		void Dispose() override;
		static void RegisterTask(RE::ItemList* a_list, RE::Actor* a_containerOwner);
	};

	class MessageBoxButtonPressDelegate : TaskDelegate
	{
	private:
		double selectedIndex;
		MessageBoxButtonPressDelegate();
	public:
		void Run() override;
		void Dispose() override;
		static void RegisterTask(const RE::FxDelegateArgs&);
	};
}
