#pragma once

#include <map>

namespace SkyrimSoulsRE
{
	class AutoCloseManager
	{
	public:
		void CheckAutoClose(RE::BSFixedString a_menuName);
		void InitAutoClose(RE::BSFixedString a_menuName, RE::TESObjectREFR* a_ref, bool a_checkForDeath);

		static AutoCloseManager* GetSingleton();

	private:
		AutoCloseManager(){};
		~AutoCloseManager(){};
		AutoCloseManager(const AutoCloseManager&) = delete;
		AutoCloseManager& operator=(const AutoCloseManager&) = delete;

		struct AutoCloseData
		{
			RE::TESObjectREFR* target = nullptr;
			bool initiallyDisabled = false;
			float initialDistance = 0.0f;
			float minDistance = 0.0f;
			bool checkForDeath = false;
			bool dialogueMode = false;
		};

		template <class T>
		bool CheckMenuStack(AutoCloseData* a_inData, RE::TESObjectREFR* a_ref)
		{
			RE::UI* ui = RE::UI::GetSingleton();
			if (ui->IsMenuOpen(T::MENU_NAME) && _autoCloseDataMap.find(T::MENU_NAME.data()) != _autoCloseDataMap.end())
			{
				AutoCloseData* data = _autoCloseDataMap.at(T::MENU_NAME.data());
				if (data->target == a_ref)
				{
					a_inData->target = data->target;
					a_inData->initialDistance = data->initialDistance;
					a_inData->minDistance = data->minDistance;
					a_inData->initiallyDisabled = data->initiallyDisabled;
					a_inData->checkForDeath = data->checkForDeath;
					a_inData->dialogueMode = data->dialogueMode;
					return true;
				}
			}
			return false;
		}

		std::map<std::string, std::shared_ptr<AutoCloseData>> _autoCloseDataMap;

		float GetDistance(RE::NiPoint3 a_playerPos, float a_playerHeight, RE::NiPoint3 a_refPos);
	};
}
