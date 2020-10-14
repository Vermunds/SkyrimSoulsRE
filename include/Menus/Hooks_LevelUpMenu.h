#pragma once
#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	// menuDepth = 3
	// flags = kPausesGame | kModal | kDisablePauseMenu
	// kUsesCursor is gamepad is disabled
	// context = kMenuMode
	class LevelUpMenuEx : public RE::LevelUpMenu
	{
	public:
		static RE::IMenu* Creator();

		static void InstallHook();
	};
}