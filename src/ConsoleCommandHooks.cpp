#include "ConsoleCommandHooks.h"

namespace SkyrimSoulsRE::ConsoleCommandHooks
{
	class BGSSaveLoadManagerEx : public RE::BGSSaveLoadManager
	{
	public:
		bool Save_Impl(std::int32_t a_deviceID, std::uint32_t a_outputStats, const char* a_fileName)
		{
			return this->RE::BGSSaveLoadManager::Save_Impl(a_deviceID, a_outputStats, a_fileName);
		}
	};

	bool Save_Hook(RE::BGSSaveLoadManager* a_this, std::int32_t a_deviceID, std::uint32_t a_outputStats, const char* a_fileName)
	{
		// Create save screenshot
		reinterpret_cast<void (*)()>(Offsets::Misc::CreateSaveScreenshot.address())();
		RE::UI::GetSingleton()->numPausesGame++;

		std::string fileName(a_fileName);

		auto task = [a_deviceID, a_outputStats, fileName]() {
			BGSSaveLoadManagerEx* saveLoadManager = static_cast<BGSSaveLoadManagerEx*>(RE::BGSSaveLoadManager::GetSingleton());
			if (saveLoadManager->Save_Impl(a_deviceID, a_outputStats, fileName.c_str()))
			{
				RE::ConsoleLog::GetSingleton()->Print("Saved.");
			}
			else
			{
				RE::ConsoleLog::GetSingleton()->Print("Save failed.");
			}

			RE::UI::GetSingleton()->numPausesGame--;
		};

		UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
		queue->AddDelayedTask(task, std::chrono::milliseconds(Settings::GetSingleton()->saveDelayMS));

		return true;
	}

	bool ServeTime_Hook(RE::PlayerCharacter* a_player)
	{
		auto task = []() {
			RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
			return player->ServePrisonTime();
		};

		UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
		queue->AddTask(task);

		return true;
	}

	bool CenterOnCell_Hook(RE::PlayerCharacter* a_player, const char* a_cellName, RE::TESObjectCELL* a_cell)
	{
		std::string cellName;
		if (a_cellName)
		{
			cellName = a_cellName;
		}

		auto task = [cellName, a_cell]() {
			RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();

			if (cellName == "")
			{
				player->CenterOnCell(a_cell);
			}
			else
			{
				player->CenterOnCell(cellName.c_str());
			}
		};

		UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
		queue->AddTask(task);

		return true;
	}

	void InstallHook()
	{
		SKSE::Trampoline& trampoline = SKSE::GetTrampoline();

		trampoline.write_call<5>(Offsets::ConsoleCommands::CenterOnCell_Hook.address() + 0x5B, CenterOnCell_Hook);
		trampoline.write_call<5>(Offsets::ConsoleCommands::CenterOnWorld_Hook.address() + 0x104, CenterOnCell_Hook);
		trampoline.write_call<5>(Offsets::ConsoleCommands::CenterOnExterior_Hook.address() + 0x109, CenterOnCell_Hook);

		trampoline.write_call<5>(Offsets::ConsoleCommands::ServeTime_Hook.address() + 0xE, ServeTime_Hook);
		REL::safe_write(Offsets::ConsoleCommands::ServeTime_Hook.address() + 0xE + 0x5, std::uint8_t(0x90));

		trampoline.write_call<5>(Offsets::ConsoleCommands::SaveGame_Hook.address() + 0xC4, Save_Hook);
	}
}
