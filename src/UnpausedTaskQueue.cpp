#include "UnpausedTaskQueue.h"
#include "Offsets.h"

namespace SkyrimSoulsRE
{
	void UnpausedTaskQueue::Lock()
	{
		bool locked = false;
		while (!locked)
		{
			locked = _mutex.try_lock();
		}
	}

	void UnpausedTaskQueue::Unlock()
	{
		return _mutex.unlock();
	}

	void UnpausedTaskQueue::AddTask(std::shared_ptr<UnpausedTask> a_task)
	{
		this->Lock();
		this->_taskQueue.push(a_task);
		this->Unlock();
	}

	void UnpausedTaskQueue::AddDelayedTask(std::shared_ptr<UnpausedTask> a_task, std::chrono::milliseconds a_delayTimeMS)
	{
		auto beginTime = std::chrono::high_resolution_clock::now();

		this->Lock();
		this->_delayedTaskQueue.push_back(std::pair<std::shared_ptr<UnpausedTask>, std::chrono::steady_clock::time_point>{ a_task, beginTime + a_delayTimeMS });
		this->Unlock();
	}

	void UnpausedTaskQueue::ProcessTasks()
	{
		std::queue<std::shared_ptr<UnpausedTask>> tasks = std::queue<std::shared_ptr<UnpausedTask>>();

		this->Lock();

		tasks.swap(_taskQueue);

		auto now = std::chrono::high_resolution_clock::now();

		auto it = _delayedTaskQueue.begin();
		while (it != _delayedTaskQueue.end())
		{
			auto delayedTask = *it;
			if (delayedTask.second < now)
			{
				tasks.push(delayedTask.first);
				it = _delayedTaskQueue.erase(it);
				continue;
			}
			++it;
		}

		this->Unlock();

		while (!tasks.empty())
		{
			tasks.front()->Run();
			tasks.pop();
		}
	}

	void UnpausedTaskQueue::UnpausedTaskQueue_Hook(void* a_unk)
	{
		reinterpret_cast<void (*)(void*)>(Offsets::UnpausedTaskQueue::OriginalFunc.address())(a_unk);

		UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
		queue->ProcessTasks();
	}

	void UnpausedTaskQueue::InstallHook()
	{
		SKSE::GetTrampoline().write_call<5>(Offsets::UnpausedTaskQueue::Hook.address() + 0x160, (uintptr_t)UnpausedTaskQueue::UnpausedTaskQueue_Hook);
	}

	UnpausedTaskQueue* UnpausedTaskQueue::GetSingleton()
	{
		static UnpausedTaskQueue singleton;
		return &singleton;
	}
}
