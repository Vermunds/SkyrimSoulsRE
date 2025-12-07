#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	class MessageBoxMenuEx : public RE::MessageBoxMenu
	{
	public:
		static RE::IMenu* Creator();
		static void InstallHook();

		RE::UI_MESSAGE_RESULTS ProcessMessage_Hook(RE::UIMessage& a_message);

		using ProcessMessage_t = decltype(&RE::MessageBoxMenu::ProcessMessage);
		static inline REL::Relocation<ProcessMessage_t> _ProcessMessage;
	};
}
