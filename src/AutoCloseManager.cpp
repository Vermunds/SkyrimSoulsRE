#include "AutoCloseManager.h"
#include "Settings.h"

#include <cmath>

namespace SkyrimSoulsRE
{
	void AutoCloseManager::CheckAutoClose(RE::BSFixedString a_menuName)
	{
		Settings* settings = Settings::GetSingleton();
		RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
		RE::UIMessageQueue* uiMessageQueue = RE::UIMessageQueue::GetSingleton();

		std::shared_ptr<AutoCloseData> data;

		if (_autoCloseDataMap.find(a_menuName.c_str()) != _autoCloseDataMap.end())
		{
			data = _autoCloseDataMap.at(a_menuName.c_str());
		}
		else
		{
			return;
		};

		if (data->dialogueMode)
		{
			RE::UI* ui = RE::UI::GetSingleton();
			RE::MenuTopicManager* mtm = RE::MenuTopicManager::GetSingleton();

			if (ui->IsMenuOpen(RE::DialogueMenu::MENU_NAME))
			{
				if (!mtm->speaker || !mtm->speaker.get() || mtm->speaker.get().get() != data->target)
				{
					uiMessageQueue->AddMessage(a_menuName, RE::UI_MESSAGE_TYPE::kHide, nullptr);
				}
			}
			else
			{
				uiMessageQueue->AddMessage(a_menuName, RE::UI_MESSAGE_TYPE::kHide, nullptr);
			}
			return;
		}

		float maxDistance = settings->autoCloseDistance;
		float tolerance = settings->autoCloseTolerance;

		if (data->target)
		{
			float currentDistance = GetDistance(player->GetPosition(), player->GetHeight(), data->target->GetPosition());
			bool tooFarOnOpen = (data->initialDistance > maxDistance) ? true : false;

			if ((data->target->IsDisabled() && !data->initiallyDisabled) || data->target->IsMarkedForDeletion())
			{
				uiMessageQueue->AddMessage(a_menuName, RE::UI_MESSAGE_TYPE::kHide, nullptr);
				return;
			}

			if (data->checkForDeath && data->target->Is(RE::FormType::ActorCharacter))
			{
				if (data->target->IsDead(true))
				{
					uiMessageQueue->AddMessage(a_menuName, RE::UI_MESSAGE_TYPE::kHide, nullptr);
					return;
				}
			}

			if (settings->autoCloseMenus)
			{
				if (!tooFarOnOpen && currentDistance > maxDistance)
				{
					//Normal case
					uiMessageQueue->AddMessage(a_menuName, RE::UI_MESSAGE_TYPE::kHide, nullptr);
				}
				else if (tooFarOnOpen)
				{
					//Target was opened when it was too far
					if (currentDistance > maxDistance && currentDistance > (data->minDistance + tolerance))  //Close only if the distance is increasing
					{
						uiMessageQueue->AddMessage(a_menuName, RE::UI_MESSAGE_TYPE::kHide, nullptr);
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

		std::shared_ptr<AutoCloseData> data;

		std::string menuName = a_menuName.c_str();

		if (_autoCloseDataMap.find(menuName) == _autoCloseDataMap.end())
		{
			data = std::make_shared<AutoCloseData>();
			_autoCloseDataMap.emplace(menuName, data);
		}
		else
		{
			data = _autoCloseDataMap.at(menuName);
		}

		// If the dialogue and the menu target matches, close the menu only when the dialogue menu closes
		RE::MenuTopicManager* mtm = RE::MenuTopicManager::GetSingleton();
		bool isPickpocketMenu = false;

		if (menuName == RE::ContainerMenu::MENU_NAME && ui->IsMenuOpen(RE::ContainerMenu::MENU_NAME))
		{
			RE::ContainerMenu* menu = static_cast<RE::ContainerMenu*>(ui->GetMenu(RE::ContainerMenu::MENU_NAME).get());
			isPickpocketMenu = menu->GetContainerMode() == RE::ContainerMenu::ContainerMode::kPickpocket;
		}

		if (!isPickpocketMenu && ui->IsMenuOpen(RE::DialogueMenu::MENU_NAME) && !mtm->forceGoodbye && mtm->speaker && mtm->speaker.get() && a_ref == mtm->speaker.get().get())
		{
			data->target = a_ref;
			data->dialogueMode = true;
			return;
		}

		// Container -> Book
		if (a_menuName == RE::BookMenu::MENU_NAME && !a_ref && ui->IsMenuOpen(RE::ContainerMenu::MENU_NAME))
		{
			//This can fail if the player somehow opens a book that is NOT opened from the current container menu
			std::shared_ptr<AutoCloseData> containerData = _autoCloseDataMap.at(RE::ContainerMenu::MENU_NAME.data());
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
		data->dialogueMode = false;
	}

	AutoCloseManager* AutoCloseManager::GetSingleton()
	{
		static AutoCloseManager singleton;
		return &singleton;
	}

	float AutoCloseManager::GetDistance(RE::NiPoint3 a_playerPos, float a_playerHeight, RE::NiPoint3 a_targetPos)
	{
		//Get distance from feet and head, return the smaller
		float distanceHead = static_cast<float>(std::pow(a_playerPos.x - a_targetPos.x, 2) + std::pow(a_playerPos.y - a_targetPos.y, 2) + std::pow((a_playerPos.z + a_playerHeight) - a_targetPos.z, 2));
		float distanceFeet = static_cast<float>(std::pow(a_playerPos.x - a_targetPos.x, 2) + std::pow(a_playerPos.y - a_targetPos.y, 2) + std::pow(a_playerPos.z - a_targetPos.z, 2));
		return std::sqrt(std::min(distanceHead, distanceFeet));
	}
}
