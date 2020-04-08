#pragma once;

#include "RE/TESObjectREFR.h"

namespace SkyrimSoulsRE::Hooks
{
	namespace BookMenu
	{
		RE::TESObjectREFR* GetBookReference();
		void InstallHook();
	}
}