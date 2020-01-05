#pragma once

#include "RE/BSFixedString.h"  // BSFixedString
#include "RE/BSTEvent.h"  // BSTEventSink, BSEventNotifyControl, BSTEventSource
#include "RE/MenuOpenCloseEvent.h"  // MenuOpenCloseEvent

namespace SkyrimSoulsRE
{
	extern UInt32 unpausedMenuCount;

	extern bool justOpenedContainer;
	extern bool justOpenedLockpicking;
	extern bool justOpenedBook;

	extern bool isInSlowMotion;

	class MenuOpenCloseEventHandler : public RE::BSTEventSink<RE::MenuOpenCloseEvent>
	{
	public:
		virtual RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_dispatcher) override;

		static bool BlockInput();

	private:
		bool IsInWhiteList(const RE::BSFixedString& a_name);
	};

	extern MenuOpenCloseEventHandler g_menuOpenCloseEventHandler;
}
