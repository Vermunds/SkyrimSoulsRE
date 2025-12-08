#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	class HUDMenuEx : public RE::HUDMenu
	{
	public:
		static inline double stealthMeterPosX;
		static inline double stealthMeterPosY;

		RE::UI_MESSAGE_RESULTS ProcessMessage_Hook(RE::UIMessage& a_message);  //04

		void SetSkyrimSoulsMode(bool a_isEnabled);
		void UpdateHUD();

		static RE::IMenu* Creator();
		static void InstallHook();

		using ProcessMessage_t = decltype(&RE::HUDMenu::ProcessMessage);
		static inline REL::Relocation<ProcessMessage_t> _ProcessMessage;
	};
}
