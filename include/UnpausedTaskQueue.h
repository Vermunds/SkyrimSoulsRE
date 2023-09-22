#pragma once
#include <chrono>
#include <mutex>
#include <queue>

namespace SkyrimSoulsRE
{
	class UnpausedTaskQueue
	{
	public:
		void AddTask(std::function<void()> a_task);
		void AddDelayedTask(std::function<void()> a_task, std::chrono::milliseconds a_delayTimeMS);

		void ProcessTasks();

		static UnpausedTaskQueue* GetSingleton();

		static void UnpausedTaskQueue_Hook(void* a_unk);
		static void InstallHook();

	private:
		UnpausedTaskQueue(){};
		~UnpausedTaskQueue(){};
		UnpausedTaskQueue(const UnpausedTaskQueue&) = delete;
		UnpausedTaskQueue& operator=(const UnpausedTaskQueue&) = delete;

		std::queue<std::function<void()>> _taskQueue;
		std::list<std::pair<std::function<void()>, std::chrono::steady_clock::time_point>> _delayedTaskQueue;
		std::mutex _mutex;

		static inline uintptr_t _originalFunc;
	};
}
