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

		static RE::IMenu* Creator();
		static void InstallHook();

	protected:
		static bool SetHudMode_Hook(RE::GFxValue::ObjectInterface* a_this, void* a_data, RE::GFxValue* a_result, const char* a_name, const RE::GFxValue* a_args, RE::UPInt a_numArgs, bool a_isDObj);  // GFxValue::ObjectInterface::Invoke

		using ProcessMessage_t = decltype(&RE::HUDMenu::ProcessMessage);
		static inline REL::Relocation<ProcessMessage_t> _ProcessMessage;
	};
}
