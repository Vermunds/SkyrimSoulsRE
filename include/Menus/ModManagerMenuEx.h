#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	// menuDepth = 9
	// flags = kPausesGame | kUsesMenuContext | kModal | kDisablePauseMenu | kRequiresUpdate | kTopmostRenderedMenu | kUpdateUsesCursor
	// context = kMenuMode
	class ModManagerMenuEx : public RE::ModManagerMenu
	{
	public:
		static RE::IMenu* Creator();
		static void InstallHook();
	};

}
