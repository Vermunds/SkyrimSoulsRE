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
		static void ItemSelect_Hook(RE::FavoritesMenu* a_this, RE::BGSEquipSlot* a_slot);

		static RE::IMenu* Creator();
		static void InstallHook();

		static inline RE::FxDelegateHandler::CallbackFn* _ItemSelect;
	};
}
