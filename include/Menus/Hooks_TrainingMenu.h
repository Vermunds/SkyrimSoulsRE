#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	// menuDepth = 3
	// flags = kPausesGame | kUsesMenuContext | kUpdateUsesCursor
	// kUsesCursor if gamepad disabled
	// context = kNone
	class TrainingMenuEx : public RE::TrainingMenu
	{
	public:
		static RE::IMenu* Creator();
		static void InstallHook();
	};
}