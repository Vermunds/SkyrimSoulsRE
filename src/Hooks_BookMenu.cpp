#include "RE/Skyrim.h"
#include "Offsets.h"

namespace SkyrimSoulsRE::Hooks
{
	namespace BookMenu
	{
		RE::TESObjectREFR* GetBookReference()
		{
			RE::TESObjectREFRPtr* refptr = reinterpret_cast<RE::TESObjectREFRPtr*>(Offsets::BookMenu_Target.GetUIntPtr());
			return refptr->get();
		}

		void InstallHook()
		{
			//Fix for book menu not appearing
			SKSE::SafeWrite16(Offsets::BookMenu_Hook.GetUIntPtr(), 0x9090);
		}
	}
}