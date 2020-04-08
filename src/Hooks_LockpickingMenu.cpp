#include "RE/TESObjectREFR.h"
#include "Offsets.h"

namespace SkyrimSoulsRE::Hooks
{
	namespace LockpickingMenu
	{
		RE::TESObjectREFR* GetLockpickingTarget()
		{
			RE::TESObjectREFRPtr* refptr = reinterpret_cast<RE::TESObjectREFRPtr*>(Offsets::LockpickingMenu_Target.GetUIntPtr());
			return refptr->get();
		}

		void InstallHook()
		{
			//Fix for lockpicking menu not appearing
			SKSE::SafeWrite16(Offsets::LockpickingMenu_Hook.GetUIntPtr(), 0x9090);
		}
	}
}