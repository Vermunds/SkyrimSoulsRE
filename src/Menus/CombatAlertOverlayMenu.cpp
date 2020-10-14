#include "Menus/CombatAlertOverlayMenu.h"

namespace SkyrimSoulsRE
{
	RE::IMenu* CombatAlertOverlayMenu::Creator()
	{
		return new CombatAlertOverlayMenu();
	}

	CombatAlertOverlayMenu::CombatAlertOverlayMenu()
	{
		using Context = RE::UserEvents::INPUT_CONTEXT_ID;
		using Flag = RE::UI_MENU_FLAGS;

		auto menu = static_cast<RE::IMenu*>(this);
		auto scaleformManager = RE::BSScaleformManager::GetSingleton();
		[[maybe_unused]] const auto success = scaleformManager->LoadMovieEx(menu, FILE_NAME, [](RE::GFxMovieDef* a_def) -> void {});
		assert(success);
		_view = menu->uiMovie;
		_view->SetMouseCursorCount(0);
		menu->menuFlags |= Flag::kAllowSaving;
		menu->depthPriority = 11;
		menu->inputContext = Context::kNone;
		//InitExtensions();

		RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
		if (player)
		{
			if (player->IsInCombat())
			{
				_isActive = true;
				_view->SetVisible(true);
			}
			else if (!player->IsInCombat())
			{
				_isActive = false;
				_view->SetVisible(false);
			}
		}

		_view->SetPause(true);
	}

	void CombatAlertOverlayMenu::AdvanceMovie(float a_interval, std::uint32_t a_currentTime)
	{
		std::uint32_t currentFrame = _view->GetCurrentFrame();

		if (!GetCombatAlertOverlayCount())
		{
			RE::UIMessageQueue* msgQueue = RE::UIMessageQueue::GetSingleton();
			msgQueue->AddMessage(CombatAlertOverlayMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kHide, nullptr);
		}

		RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
		if (player && currentFrame == 120)
		{
			if (player->IsInCombat() && !_isActive)
			{
				_isActive = true;
				_view->SetVisible(true);
			}
			else if (!player->IsInCombat() && _isActive)
			{
				_isActive = false;
				_view->SetVisible(false);
			}
		}
		
		std::uint32_t nextFrame = currentFrame == 120 ? 1 : currentFrame + 1;
		_view->GotoFrame(nextFrame);
	}

	RE::UI_MESSAGE_RESULTS CombatAlertOverlayMenu::ProcessMessage(RE::UIMessage& a_message)
	{
		if (a_message.menu == CombatAlertOverlayMenu::MENU_NAME)
		{
			return RE::UI_MESSAGE_RESULTS::kHandled;
		}
		return RE::UI_MESSAGE_RESULTS::kPassOn;
	}

	void CombatAlertOverlayMenu::InitExtensions()
	{
		const RE::GFxValue boolean(true);
		bool success;

		success = _view->SetVariable("_global.gfxExtensions", boolean);
		assert(success);
		success = _view->SetVariable("_global.noInvisibleAdvance", boolean);
		assert(success);
	}
}
