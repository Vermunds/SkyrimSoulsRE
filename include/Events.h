#pragma once

#include <vector>  // vector

#include "RE/BSFixedString.h"  // BSFixedString
#include "RE/BSTEvent.h"  // BSTEventSink, EventResult, BSTEventSource
#include "RE/MenuOpenCloseEvent.h"  // MenuOpenCloseEvent


namespace SkyrimSoulsRE
{
	extern UInt8 unpausedMenuCount;

	class MenuOpenCloseEventHandler : public RE::BSTEventSink<RE::MenuOpenCloseEvent>
	{
	public:
		virtual RE::EventResult ReceiveEvent(RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_dispatcher) override;

		static void Init();

		static bool BlockInput(const char* a_exclude);

	private:
		bool IsInWhiteList(RE::BSFixedString& a_name);

		static std::vector<RE::BSFixedString>	_whiteList;
	};


	extern MenuOpenCloseEventHandler g_menuOpenCloseEventHandler;
}
