#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	class BarterMenuEx : public RE::BarterMenu
	{
	public:
		RE::UI_MESSAGE_RESULTS ProcessMessage_Hook(RE::UIMessage& a_message);  //04

		void UpdateBottomBar();

		static RE::IMenu* Creator();
		static void InstallHook();

		using ProcessMessage_t = decltype(&RE::BarterMenu::ProcessMessage);
		static inline REL::Relocation<ProcessMessage_t> _ProcessMessage;
	};
}
