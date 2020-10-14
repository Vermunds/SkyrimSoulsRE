#include "Menus/AutoCloseManager.h"

#include "Settings.h"
#include "RE/Skyrim.h"

namespace SkyrimSoulsRE
{
	AutoCloseManager* AutoCloseManager::_singleton = nullptr;

	void AutoCloseManager::CheckAutoClose(RE::BSFixedString a_menuName)
	{
		Settings* settings = Settings::GetSingleton();
		RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
		RE::UIMessageQueue* uiMessageQueue = RE::UIMessageQueue::GetSingleton();

		AutoCloseData* data = _autoCloseDataMap.at(a_menuName.c_str());

		if (data->target)
		{
			float currentDistance = GetDistance(player->GetPosition(), player->GetHeight(), data->target->GetPosition());
			bool tooFarOnOpen = (data->initialDistance > settings->autoCloseDistance) ? true : false;

			if ((data->target->IsDisabled() && !data->initiallyDisabled) || data->target->IsMarkedForDeletion())
			{
				uiMessageQueue->AddMessage(a_menuName, RE::UI_MESSAGE_TYPE::kHide, 0);
				return;
			}

			if (data->checkForDeath && data->target->Is(RE::FormType::ActorCharacter))
			{
				if (data->target->IsDead(true))
				{
					uiMessageQueue->AddMessage(a_menuName, RE::UI_MESSAGE_TYPE::kHide, 0);
					return;
				}
			}

			if (settings->autoCloseMenus) {

				if (!tooFarOnOpen && currentDistance > settings->autoCloseDistance)
				{
					//Normal case
					uiMessageQueue->AddMessage(a_menuName, RE::UI_MESSAGE_TYPE::kHide, 0);
				}
				else if (tooFarOnOpen)
				{
					//Container was opened when it was too far
					if (currentDistance > settings->autoCloseDistance && currentDistance > (data->minDistance + 50)) //Close only if the distance is increasing
					{
						uiMessageQueue->AddMessage(a_menuName, RE::UI_MESSAGE_TYPE::kHide, 0);
						return;
					}
				}
			}
			data->minDistance = (currentDistance < data->minDistance) ? currentDistance : data->minDistance;
		}
	}

	void AutoCloseManager::InitAutoClose(RE::BSFixedString a_menuName, RE::TESObjectREFR* a_ref, bool a_checkForDeath)
	{
		RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();

		AutoCloseData* data;

		std::string menuName = a_menuName.c_str();

		if (_autoCloseDataMap.find(menuName) == _autoCloseDataMap.end()) {
			data = new AutoCloseData();
			_autoCloseDataMap.emplace(menuName, data);
		}
		else {
			data = _autoCloseDataMap.at(menuName);
		}

		data->target = a_ref;
		data->initialDistance = a_ref ? GetDistance(player->GetPosition(), player->GetHeight(), a_ref->GetPosition()) : 0.0f;
		data->minDistance = data->initialDistance;
		data->initiallyDisabled = a_ref ? a_ref->IsDisabled() : false;
		data->checkForDeath = a_checkForDeath;
	}

	AutoCloseManager* AutoCloseManager::GetSingleton()
	{
		if (_singleton)
		{
			return _singleton;
		}
		_singleton = new AutoCloseManager();
		return _singleton;
	}

	float AutoCloseManager::GetDistance(RE::NiPoint3 a_playerPos, float a_playerHeight, RE::NiPoint3 a_refPos)
	{
		//Get distance from feet and head, return the smaller
		float distanceHead = sqrt(pow(a_playerPos.x - a_refPos.x, 2) + pow(a_playerPos.y - a_refPos.y, 2) + pow((a_playerPos.z + a_playerHeight) - a_refPos.z, 2));
		float distanceFeet = sqrt(pow(a_playerPos.x - a_refPos.x, 2) + pow(a_playerPos.y - a_refPos.y, 2) + pow(a_playerPos.z - a_refPos.z, 2));
		if (distanceHead < distanceFeet) {
			return distanceHead;
		}
		return distanceFeet;
	}
}