#pragma once

#include "skse64/GameEvents.h"  // BSTEventSink, MenuOpenCloseEvent, EventResult, EventDispatcher
#include "skse64/GameMenus.h"  // UIStringHolder
#include "skse64/GameTypes.h"  // BSFixedString

#include <vector>  // vector


namespace SkyrimSoulsRE
{
	class MenuOpenCloseEventHandler : public BSTEventSink<MenuOpenCloseEvent>
	{
	public:
		virtual EventResult ReceiveEvent(MenuOpenCloseEvent* a_event, EventDispatcher<MenuOpenCloseEvent>* a_dispatcher) override;

		static bool BlockInput();
		static void Init();

	private:
		bool IsInWhiteList(BSFixedString& a_name);

		static std::vector<BSFixedString>	_whiteList;
		static UInt32						_numPauseGame;
		static UInt32						_numPauseGameBuffer;
		static bool							_cleanUp;
	};


	extern MenuOpenCloseEventHandler g_menuOpenCloseEventHandler;
}
