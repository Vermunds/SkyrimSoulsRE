#pragma once
#include <chrono>
#include <mutex>
#include <queue>

namespace SkyrimSoulsRE
{
	class UnpausedTaskQueue
	{
	public:
		class UnpausedTask
		{
		public:
			virtual void Run() = 0;
			virtual void Dispose() { delete this; };

			bool usesDelay = false;
			std::chrono::steady_clock::time_point beginTime;
			std::chrono::milliseconds delayTimeMS;
		};

		void Lock();
		void Unlock();
		void AddTask(UnpausedTask* task);
		void ProcessTasks();

		static void UnpausedTaskQueue_Hook(void* a_unk);
		static UnpausedTaskQueue* GetSingleton();

		static void InstallHook();

	private:
		UnpausedTaskQueue(){};

		static UnpausedTaskQueue* _singleton;

		std::queue<UnpausedTask*> _taskQueue;
		std::queue<UnpausedTask*> _nextFrameTaskQueue;
		std::mutex _mutex;
	};

	using UnpausedTask = UnpausedTaskQueue::UnpausedTask;
}
