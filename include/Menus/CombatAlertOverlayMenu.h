#pragma once
#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	class CombatAlertOverlayMenu : public RE::IMenu
	{
	public:
		static constexpr std::string_view MENU_NAME = "CombatAlertOverlayMenu";
		static constexpr std::string_view FILE_NAME = "CombatAlertOverlayMenu";

		void AdvanceMovie(float a_interval, std::uint32_t a_currentTime) override;
		RE::UI_MESSAGE_RESULTS ProcessMessage(RE::UIMessage& a_message) override;

		void InitExtensions();

		static RE::IMenu* Creator();
	private:
		CombatAlertOverlayMenu();

		RE::GPtr<RE::GFxMovieView> _view;
		bool _isActive = false;
	};
}