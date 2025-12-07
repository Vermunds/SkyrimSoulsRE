#include "AutoCloseManager.h"
#include "Settings.h"
#include "Util.h"

#include <cmath>

namespace SkyrimSoulsRE
{
	RE::TESObjectREFR* GetReferenceFromHandle(RE::RefHandle a_handle)
	{
		RE::TESObjectREFRPtr refptr = nullptr;
		RE::TESObjectREFR::LookupByHandle(a_handle, refptr);
		return refptr.get();
	}

	void AutoCloseManager::CheckAutoClose(std::string_view a_menuName)
	{
		auto it = _autoCloseDataMap.find(a_menuName);
		if (it == _autoCloseDataMap.end())
		{
			return;
		}
		AutoCloseData& data = it->second;

		Settings* settings = Settings::GetSingleton();
		const float maxDistance = settings->autoCloseDistance;
		const float tolerance = settings->autoCloseTolerance;

		RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
		RE::UIMessageQueue* uiMessageQueue = RE::UIMessageQueue::GetSingleton();

		RE::TESObjectREFR* target = GetReferenceFromHandle(data.targetRefHandle);

		if (data.dialogueMode)
		{
			RE::UI* ui = RE::UI::GetSingleton();
			RE::MenuTopicManager* mtm = RE::MenuTopicManager::GetSingleton();
			REX::W32::EnterCriticalSection(&mtm->criticalSection);

			if (ui->IsMenuOpen(RE::DialogueMenu::MENU_NAME))
			{
				if (!mtm->speaker || !mtm->speaker.get() || mtm->speaker.get().get() != target)
				{
					CloseMenu(data, a_menuName, "Parent Dialogue Menu target changed");
				}
			}
			else
			{
				CloseMenu(data, a_menuName, "Parent Dialogue Menu closed");
			}

			REX::W32::LeaveCriticalSection(&mtm->criticalSection);
			return;
		}

		if (target)
		{
			float currentDistance = GetBBDistance(player, target);
			bool tooFarOnOpen = data.initialDistance > maxDistance;

			if ((target->IsDisabled() && !data.initiallyDisabled) || target->IsMarkedForDeletion())
			{
				CloseMenu(data, a_menuName, "Target got disabled or deleted");
				return;
			}

			if (data.checkForDeath && target->Is(RE::FormType::ActorCharacter))
			{
				if (target->IsDead(true))
				{
					CloseMenu(data, a_menuName, "Target character died");
					return;
				}
			}

			if (settings->autoCloseMenus)
			{
				if (!tooFarOnOpen && currentDistance > maxDistance)
				{
					//Normal case
					CloseMenu(data, a_menuName, "Target moved out of range.");
				}
				else if (tooFarOnOpen)
				{
					//Target was opened when it was too far
					if (currentDistance > maxDistance && currentDistance > (data.minDistance + tolerance))  //Close only if the distance is increasing
					{
						CloseMenu(data, a_menuName, "Target is too far and the distance has increased further");
						return;
					}
				}
			}
			data.minDistance = (currentDistance < data.minDistance) ? currentDistance : data.minDistance;
		}
	}

	void AutoCloseManager::InitAutoClose(std::string_view a_menuName, RE::RefHandle a_refHandle, bool a_checkForDeath)
	{
		RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
		RE::UI* ui = RE::UI::GetSingleton();

		AutoCloseData& data = _autoCloseDataMap[a_menuName];
		data = AutoCloseData{};

		// If the dialogue and the menu target matches, close the menu only when the dialogue menu closes
		bool isPickpocketMenu = false;

		if (a_menuName == RE::ContainerMenu::MENU_NAME && ui->IsMenuOpen(RE::ContainerMenu::MENU_NAME))
		{
			RE::ContainerMenu* menu = static_cast<RE::ContainerMenu*>(ui->GetMenu(RE::ContainerMenu::MENU_NAME).get());
			isPickpocketMenu = menu->GetContainerMode() == RE::ContainerMenu::ContainerMode::kPickpocket;
		}

		RE::TESObjectREFR* target = GetReferenceFromHandle(a_refHandle);

		RE::MenuTopicManager* mtm = RE::MenuTopicManager::GetSingleton();
		REX::W32::EnterCriticalSection(&mtm->criticalSection);
		if (!isPickpocketMenu && ui->IsMenuOpen(RE::DialogueMenu::MENU_NAME) && !mtm->forceGoodbye && mtm->speaker && mtm->speaker.get() && target && target == mtm->speaker.get().get())
		{
			data.targetRefHandle = a_refHandle;
			data.dialogueMode = true;

			data.PrintDebugInfo(a_menuName);
			REX::W32::LeaveCriticalSection(&mtm->criticalSection);
			return;
		}
		REX::W32::LeaveCriticalSection(&mtm->criticalSection);

		// Container -> Book
		if (a_menuName == RE::BookMenu::MENU_NAME && !target && ui->IsMenuOpen(RE::ContainerMenu::MENU_NAME))
		{
			SKSE::log::info("Detected BookMenu opened from ContainerMenu. Using ContainerMenu data for auto-close.");

			//This can fail if the player somehow opens a book that is NOT opened from the current container menu
			AutoCloseData& containerData = _autoCloseDataMap.at(RE::ContainerMenu::MENU_NAME);
			data.targetRefHandle = containerData.targetRefHandle;
			data.initialDistance = containerData.initialDistance;
			data.minDistance = containerData.minDistance;
			data.initiallyDisabled = containerData.initiallyDisabled;
			data.checkForDeath = containerData.checkForDeath;
			data.dialogueMode = containerData.dialogueMode;

			data.PrintDebugInfo(a_menuName);
			return;
		}

		data.targetRefHandle = a_refHandle;
		data.initialDistance = target ? GetBBDistance(player, target) : 0.0f;
		data.minDistance = data.initialDistance;
		data.initiallyDisabled = target ? target->IsDisabled() : false;
		data.checkForDeath = a_checkForDeath;
		data.dialogueMode = false;

		data.PrintDebugInfo(a_menuName);
	}

	AutoCloseManager* AutoCloseManager::GetSingleton()
	{
		static AutoCloseManager singleton;
		return &singleton;
	}

	void AutoCloseManager::CloseMenu(AutoCloseData& data, std::string_view a_menuName, const std::string& a_reason)
	{
		SKSE::log::info("Closing {}: {}", a_menuName, a_reason);

		RE::UIMessageQueue* uiMessageQueue = RE::UIMessageQueue::GetSingleton();
		uiMessageQueue->AddMessage(a_menuName, RE::UI_MESSAGE_TYPE::kHide, nullptr);
	}

	float AutoCloseManager::GetBBDistance(const RE::TESObjectREFR* a_refA, const RE::TESObjectREFR* a_refB)
	{
		if (!a_refA || !a_refB)
		{
			return 0.0f;
		}

		RE::NiPoint3 posA = a_refA->GetPosition();
		RE::NiPoint3 posB = a_refB->GetPosition();

		RE::NiPoint3 aMinA = a_refA->GetBoundMin() + posA;
		RE::NiPoint3 aMaxA = a_refA->GetBoundMax() + posA;
		RE::NiPoint3 aMinB = a_refB->GetBoundMin() + posB;
		RE::NiPoint3 aMaxB = a_refB->GetBoundMax() + posB;

		auto axisDist = [](float minA, float maxA, float minB, float maxB) {
			if (maxA < minB)
			{
				return minB - maxA;  // A is left/below/behind B
			}
			if (maxB < maxA && maxB < minA)
			{
				return minA - maxB;  // B is left/below/behind A
			}
			return 0.0f;  // Overlapping on this axis
		};

		float dx = axisDist(aMinA.x, aMaxA.x, aMinB.x, aMaxB.x);
		float dy = axisDist(aMinA.y, aMaxA.y, aMinB.y, aMaxB.y);
		float dz = axisDist(aMinA.z, aMaxA.z, aMinB.z, aMaxB.z);

		return std::sqrt(dx * dx + dy * dy + dz * dz);
	}

	void AutoCloseManager::AutoCloseData::PrintDebugInfo(std::string_view a_menuName)
	{
		RE::TESObjectREFR* target = GetReferenceFromHandle(targetRefHandle);

		if (dialogueMode)
		{
			SKSE::log::info(
				"Auto-close data for {} | Target: {} | Dialogue mode: {}",
				a_menuName, Util::GetRefDebugString(target), dialogueMode);
		}
		else
		{
			SKSE::log::info(
				"Auto-close data for {} | Target: {} | Initial distance: {} | Initially disabled: {} | Check for death: {} | Dialogue mode: {}",
				a_menuName, Util::GetRefDebugString(target), initialDistance, initiallyDisabled, checkForDeath, dialogueMode);
		}
	}
}
