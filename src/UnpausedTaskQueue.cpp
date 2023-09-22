#include "UnpausedTaskQueue.h"
#include "Offsets.h"

namespace SkyrimSoulsRE
{
	void UnpausedTaskQueue::AddTask(std::function<void()> a_task)
	{
		std::lock_guard lk(_mutex);
		this->_taskQueue.push(std::move(a_task));
	}

	void UnpausedTaskQueue::AddDelayedTask(std::function<void()> a_task, std::chrono::milliseconds a_delayTimeMS)
	{
		std::lock_guard lk(_mutex);
		auto beginTime = std::chrono::high_resolution_clock::now() + a_delayTimeMS;
		this->_delayedTaskQueue.emplace_back(std::make_pair(std::move(a_task), beginTime));
	}

	void UnpausedTaskQueue::ProcessTasks()
	{
		std::queue<std::function<void()>> tasks;

		{
			std::lock_guard lk(_mutex);
			tasks.swap(_taskQueue);

			auto now = std::chrono::high_resolution_clock::now();

			for (auto it = _delayedTaskQueue.begin(); it != _delayedTaskQueue.end();)
			{
				const auto& delayedTask = *it;
				if (delayedTask.second < now)
				{
					tasks.push(delayedTask.first);
					it = _delayedTaskQueue.erase(it);
				}
				else
				{
					++it;
				}
			}
		}

		while (!tasks.empty())
		{
			tasks.front()();
			tasks.pop();
		}
	}

	void UnpausedTaskQueue::UnpausedTaskQueue_Hook(void* a_unk)
	{
		reinterpret_cast<void (*)(void*)>(_originalFunc)(a_unk);

		UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
		queue->ProcessTasks();
	}

	void UnpausedTaskQueue::InstallHook()
	{
		_originalFunc = SKSE::GetTrampoline().write_call<5>(Offsets::UnpausedTaskQueue::Hook.address() + 0x160, (uintptr_t)UnpausedTaskQueue::UnpausedTaskQueue_Hook);
	}

	UnpausedTaskQueue* UnpausedTaskQueue::GetSingleton()
	{
		static UnpausedTaskQueue singleton;
		return &singleton;
	}
}
