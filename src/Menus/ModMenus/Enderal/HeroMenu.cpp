#include "Menus/ModMenus/Enderal/HeroMenu.h"

namespace SkyrimSoulsRE
{
	bool HeroMenu::IsOpen()
	{
		RE::UI* ui = RE::UI::GetSingleton();
		if (!ui->IsMenuOpen(MENU_NAME))
		{
			return false;
		}

		RE::GPtr<RE::IMenu> menu = ui->GetMenu(MENU_NAME);
		RE::GFxMovieDef* movieDef = menu && menu->uiMovie ? menu->uiMovie->GetMovieDef() : nullptr;
		const char* fileURL = movieDef ? movieDef->GetFileURL() : nullptr;

		return fileURL && std::string_view(fileURL).contains(MOVIE_NAME);
	}

	void HeroMenu::ProcessInput(RE::InputEvent* const* a_event)
	{
		// EnderalSE.dll closes the Hero Menu from its own MenuControls hook, but only while the game is paused.
		// Do the same while it is unpaused, so the menu can still be closed.
		if (!a_event || RE::UI::GetSingleton()->GameIsPaused())
		{
			return;
		}

		RE::UserEvents* userEvents = RE::UserEvents::GetSingleton();

		for (RE::InputEvent* evn = *a_event; evn; evn = evn->next)
		{
			RE::ButtonEvent* buttonEvent = evn->AsButtonEvent();
			if (!buttonEvent || !buttonEvent->IsDown())
			{
				continue;
			}

			if (buttonEvent->userEvent == userEvents->cancel || buttonEvent->userEvent == userEvents->quickStats)
			{
				if (IsOpen())
				{
					RE::UIMessageQueue::GetSingleton()->AddMessage(MENU_NAME, RE::UI_MESSAGE_TYPE::kHide, nullptr);
				}
				return;
			}
		}
	}
}
