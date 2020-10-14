#include "AutoCloseManager.h"

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

		float maxDistance = data->dialogueMode ? settings->autoCloseDistanceDialogueContext : settings->autoCloseDistance;
		float tolerance = data->dialogueMode ? 350.0 : 50.0;

		if (data->target)
		{
			float currentDistance = GetDistance(player->GetPosition(), player->GetHeight(), data->target->GetPosition());
			bool tooFarOnOpen = (data->initialDistance > maxDistance) ? true : false;

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

				if (!tooFarOnOpen && currentDistance > maxDistance)
				{
					//Normal case
					uiMessageQueue->AddMessage(a_menuName, RE::UI_MESSAGE_TYPE::kHide, 0);
				}
				else if (tooFarOnOpen)
				{
					//Container was opened when it was too far
					if (currentDistance > maxDistance && currentDistance > (data->minDistance + tolerance)) //Close only if the distance is increasing
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
		RE::UI* ui = RE::UI::GetSingleton();

		AutoCloseData* data;

		std::string menuName = a_menuName.c_str();

		if (_autoCloseDataMap.find(menuName) == _autoCloseDataMap.end()) {
			data = new AutoCloseData();
			_autoCloseDataMap.emplace(menuName, data);
		}
		else {
			data = _autoCloseDataMap.at(menuName);
		}

		data->dialogueMode = (a_menuName == RE::DialogueMenu::MENU_NAME);

		// Check for parent menus -> copy the parent menus autoclose data
		// Menus we never auto close: Tutorial Menu (maybe possible but I don't care), MessageBox (can't really autoclose this one, as there is no way I know of to "cancel" actions. The player has to decide.)
		// Dialogue -> Container
		if (a_menuName == RE::ContainerMenu::MENU_NAME)
		{
			if (CheckMenuStack<RE::DialogueMenu>(data, a_ref))
				return;
		}

		// Dialogue -> Gift
		if (a_menuName == RE::GiftMenu::MENU_NAME)
		{
			if (CheckMenuStack<RE::DialogueMenu>(data, a_ref))
				return;
		}

		// Dialogue -> Barter
		if (a_menuName == RE::BarterMenu::MENU_NAME)
		{
			if (CheckMenuStack<RE::DialogueMenu>(data, a_ref))
				return;
		}

		//Dialogue -> Training
		if (a_menuName == RE::TrainingMenu::MENU_NAME)
		{
			if (CheckMenuStack<RE::DialogueMenu>(data, a_ref))
				return;
		}

		// Container -> Book
		if (a_menuName == RE::BookMenu::MENU_NAME && !a_ref && ui->IsMenuOpen(RE::ContainerMenu::MENU_NAME))
		{
			//This can fail if the player somehow opens a book that is NOT opened from the container menu
			AutoCloseData* containerData = _autoCloseDataMap.at(RE::ContainerMenu::MENU_NAME.data());
			data->target = containerData->target;
			data->initialDistance = containerData->initialDistance;
			data->minDistance = containerData->minDistance;
			data->initiallyDisabled = containerData->initiallyDisabled;
			data->checkForDeath = containerData->checkForDeath;
			data->dialogueMode = containerData->dialogueMode;
			return;
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