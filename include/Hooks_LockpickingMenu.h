#pragma once
#include "RE/TESObjectREFR.h"

namespace SkyrimSoulsRE::Hooks
{
	namespace LockpickingMenu
	{
		RE::TESObjectREFR* GetLockpickingTarget();
		void InstallHook();
	}
}