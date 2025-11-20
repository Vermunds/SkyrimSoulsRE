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
		static RE::IMenu* Creator();  //Only runs at game startup, the menu stays open in the background
		static void InstallHook();
	};
}
