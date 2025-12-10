#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	class HUDMenuEx : public RE::HUDMenu
	{
	public:
		inline constexpr static RE::UI_MESSAGE_TYPE SET_SKYRIMSOULS_HUD_MODE_MESSAGE_TYPE = static_cast<RE::UI_MESSAGE_TYPE>(11001);

		static inline double stealthMeterPosX;
		static inline double stealthMeterPosY;

		RE::UI_MESSAGE_RESULTS ProcessMessage_Hook(RE::UIMessage& a_message);  //04

		static RE::IMenu* Creator();
		static void InstallHook();

	protected:
		void SetSkyrimSoulsMode(bool a_isEnabled);

		using ProcessMessage_t = decltype(&RE::HUDMenu::ProcessMessage);
		static inline REL::Relocation<ProcessMessage_t> _ProcessMessage;
	};
}
