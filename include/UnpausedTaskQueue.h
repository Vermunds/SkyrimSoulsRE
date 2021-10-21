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
		};

		void Lock();
		void Unlock();

		void AddTask(std::shared_ptr<UnpausedTask> a_task);
		void AddDelayedTask(std::shared_ptr<UnpausedTask> a_task, std::chrono::milliseconds a_delayTimeMS);

		void ProcessTasks();

		static UnpausedTaskQueue* GetSingleton();

		static void UnpausedTaskQueue_Hook(void* a_unk);
		static void InstallHook();

	private:
		UnpausedTaskQueue(){};
		~UnpausedTaskQueue(){};
		UnpausedTaskQueue(const UnpausedTaskQueue&) = delete;
  		UnpausedTaskQueue& operator=(const UnpausedTaskQueue&) = delete;

		std::queue<std::shared_ptr<UnpausedTask>> _taskQueue;
		std::list<std::pair<std::shared_ptr<UnpausedTask>, std::chrono::steady_clock::time_point>> _delayedTaskQueue;
		std::mutex _mutex;
	};

	using UnpausedTask = UnpausedTaskQueue::UnpausedTask;
}
