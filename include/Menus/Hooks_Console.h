#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	// menuDepth = 12
	// flags = kPausesGame | kAlwaysOpen | kUsesCursor | kAllowSaving
	// context = kConsole
	class ConsoleEx : public RE::Console
	{
	public:
		static void ExecuteCommand_Callback_Hook(const RE::FxDelegateArgs& a_args);

		static RE::IMenu* Creator();  //Only runs at game startup, the menu stays open in the background
		static void InstallHook();
	};
}
