#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	// menuDepth = 3
	// flags = kPausesGame | kUpdateUsesCursor | kInventoryItemMenu | kCustomRendering
	// context = kFavorites
	class FavoritesMenuEx :
		public RE::FavoritesMenu  // 00
	{
	public:
		static RE::IMenu* Creator();
		static void InstallHook();
	};
}
