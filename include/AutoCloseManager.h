#pragma once

#include <map>

namespace SkyrimSoulsRE
{
	class AutoCloseManager
	{
	public:
		void CheckAutoClose(std::string_view a_menuName);
		void InitAutoClose(std::string_view a_menuName, RE::RefHandle a_refHandle, bool a_checkForDeath);

		static AutoCloseManager* GetSingleton();

	private:
		AutoCloseManager() {};
		~AutoCloseManager() {};
		AutoCloseManager(const AutoCloseManager&) = delete;
		AutoCloseManager& operator=(const AutoCloseManager&) = delete;

		struct AutoCloseData
		{
			RE::RefHandle targetRefHandle = 0;
			bool initiallyDisabled = false;
			float initialDistance = 0.0f;
			float minDistance = 0.0f;
			bool checkForDeath = false;
			bool dialogueMode = false;

			void PrintDebugInfo(std::string_view a_menuName);
		};

		std::unordered_map<std::string_view, AutoCloseData> _autoCloseDataMap;

		void CloseMenu(AutoCloseData& a_data, std::string_view a_menuName, const std::string& a_reason);
		float GetBBDistance(const RE::TESObjectREFR* a_refA, const RE::TESObjectREFR* a_refB);
	};
}
