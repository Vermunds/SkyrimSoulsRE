#include "UnpausedTaskQueue.h"
#include "Offsets.h"

namespace SkyrimSoulsRE
{
	UnpausedTaskQueue* UnpausedTaskQueue::_singleton = nullptr;

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

	void UnpausedTaskQueue::AddTask(UnpausedTask* task)
	{
		this->Lock();
		this->_taskQueue.push(task);
		this->Unlock();
	}

	void UnpausedTaskQueue::ProcessTasks()
	{
		std::list<UnpausedTask*> tasks;

		this->Lock();
		while (!_taskQueue.empty())
		{
			
			UnpausedTask* task = _taskQueue.front();

			if (task->usesDelay)
			{
				auto currTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch());
				auto startTime = std::chrono::duration_cast<std::chrono::milliseconds>(task->beginTime.time_since_epoch());

				if (currTime - startTime > task->delayTimeMS)
				{
					tasks.push_back(task);
				}
				else
				{
					_nextFrameTaskQueue.push(task);
				}
			}
			else
			{
				tasks.push_back(task);
			}

			_taskQueue.pop();
		}
		_taskQueue = _nextFrameTaskQueue;
		std::queue<UnpausedTask*>().swap(_nextFrameTaskQueue);
		this->Unlock();

		for (UnpausedTask * task : tasks)
		{
			task->Run();
			task->Dispose();
			
		}		
	}

	void UnpausedTaskQueue::UnpausedTaskQueue_Hook(void* a_unk)
	{
		reinterpret_cast<void(*)(void*)>(Offsets::UnpausedTaskQueue::OriginalFunc.address())(a_unk);

		UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
		queue->ProcessTasks();
	}

	void UnpausedTaskQueue::InstallHook()
	{
		SKSE::GetTrampoline().write_call<5>(Offsets::UnpausedTaskQueue::Hook.address() + 0x11F, (uintptr_t)UnpausedTaskQueue::UnpausedTaskQueue_Hook);
	}

	UnpausedTaskQueue* UnpausedTaskQueue::GetSingleton()
	{
		if (_singleton)
		{
			return _singleton;
		}
		_singleton = new UnpausedTaskQueue();
		return _singleton;
	}
}