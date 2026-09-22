#pragma once

namespace SkyrimSoulsRE
{
	class SaveHandler
	{
	public:
		static SaveHandler* GetSingleton();
		static void InstallHook();

	private:
		SaveHandler() {};
		~SaveHandler() {};
		SaveHandler(const SaveHandler&) = delete;
		SaveHandler& operator=(const SaveHandler&) = delete;

		static bool SaveGame_Hook(RE::BGSSaveLoadManager* a_this, std::uint32_t a_deviceID, std::uint32_t a_outputStats, const char* a_fileName);
		static void BGSSaveLoadManager_ProcessEvents_Hook();

		std::string m_currentFileName;
		bool m_isSaving = false;

		using SaveGame_t = decltype(&SaveGame_Hook);
		static inline REL::Relocation<SaveGame_t> _SaveGame;
	};
}
