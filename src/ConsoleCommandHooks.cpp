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
		class SaveTask : public UnpausedTask
		{
			std::int32_t deviceID;
			std::uint32_t outputStats;
			std::string fileName;
		public:

			SaveTask(std::int32_t a_deviceID, std::uint32_t a_outputStats, std::string a_fileName)
			{
				this->deviceID = a_deviceID;
				this->outputStats = a_outputStats;
				this->fileName = a_fileName;
				this->usesDelay = true;
				this->beginTime = std::chrono::high_resolution_clock::now();
				this->delayTimeMS = std::chrono::milliseconds(Settings::GetSingleton()->saveDelayMS);
			}

			void Run() override
			{
				BGSSaveLoadManagerEx* saveLoadManager = static_cast<BGSSaveLoadManagerEx*>(RE::BGSSaveLoadManager::GetSingleton());
				if (saveLoadManager->Save_Impl(deviceID, outputStats, fileName.c_str()))
				{
					RE::ConsoleLog::GetSingleton()->Print("Saved.");
				}
				else
				{
					RE::ConsoleLog::GetSingleton()->Print("Save failed.");
				}
				
				RE::UI::GetSingleton()->numPausesGame--;
			}
		};
		//Create save screenshot
		reinterpret_cast<void(*)()>(Offsets::Misc::CreateSaveScreenshot.address())();
		RE::UI::GetSingleton()->numPausesGame++;

		UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
		queue->AddTask(new SaveTask(a_deviceID, a_outputStats, std::string(a_fileName)));

		return true;
	}

	void ServeTime_Hook(RE::PlayerCharacter* a_player)
	{
		class ServeTimeTask : public UnpausedTask
		{
		public:

			ServeTimeTask() {}

			void Run() override
			{
				RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
				return player->ServePrisonTime();
			}
		};

		UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
		queue->AddTask(new ServeTimeTask());
	}

	bool CenterOnCell_Hook(RE::PlayerCharacter* a_player, const char* a_cellName, RE::TESObjectCELL* a_cell)
	{
		class CenterOnCellTask : public UnpausedTask
		{
			std::string cellName;
			RE::TESObjectCELL* cell;

		public:

			CenterOnCellTask(std::string a_cellName, RE::TESObjectCELL* a_cell)
			{
				this->cellName = a_cellName;
				this->cell = a_cell;
			}

			void Run() override
			{
				RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();		
				
				if (cellName == "")
				{
					player->CenterOnCell(cell);
				}
				else
				{
					player->CenterOnCell(cellName.c_str());
				}
			}
		};
		std::string cellName = a_cellName ? std::string(a_cellName) : "";

		UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
		queue->AddTask(new CenterOnCellTask(cellName, a_cell));

		return true;
	}

	void InstallHook()
	{
		SKSE::Trampoline& trampoline = SKSE::GetTrampoline();

		trampoline.write_call<5>(Offsets::ConsoleCommands::CenterOnCell_Hook.address() + 0x5B, CenterOnCell_Hook);

		trampoline.write_call<5>(Offsets::ConsoleCommands::CenterOnWorld_Hook.address() + 0x108, CenterOnCell_Hook);

		trampoline.write_call<5>(Offsets::ConsoleCommands::CenterOnExterior_Hook.address() + 0x118, CenterOnCell_Hook);

		trampoline.write_call<5>(Offsets::ConsoleCommands::ServeTime_Hook.address() + 0xE, ServeTime_Hook);
		REL::safe_write(Offsets::ConsoleCommands::ServeTime_Hook.address() + 0xE + 0x5, std::uint8_t(0x90));

		trampoline.write_call<5>(Offsets::ConsoleCommands::SaveGame_Hook.address() + 0xC4, Save_Hook);
	}
}
