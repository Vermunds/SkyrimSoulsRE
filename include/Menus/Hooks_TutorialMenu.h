#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	// menuDepth = 10
	// flags = kPausesGame | kModal | kUpdateUsesCursor
	// context = kMenuMode

	class TutorialMenuEx : RE::TutorialMenu
	{
	public:
		static RE::IMenu* Creator();
		static void InstallHook();
	};
}
