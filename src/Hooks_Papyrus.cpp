#include "Offsets.h"
#include "Events.h"
#include "SKSE/API.h"

namespace SkyrimSoulsRE::Hooks
{
	namespace Papyrus
	{
		bool* isInMenuMode_1;
		bool* isInMenuMode_2;

		bool IsInMenuMode()
		{
			if (*isInMenuMode_1 || *isInMenuMode_2 || SkyrimSoulsRE::unpausedMenuCount) {
				return true;
			}
			return false;
		}

		void InstallHook() {
			//IsInMenuMode hook
			isInMenuMode_1 = reinterpret_cast<bool*>(Offsets::IsInMenuMode_Original.GetUIntPtr());
			isInMenuMode_2 = reinterpret_cast<bool*>(Offsets::IsInMenuMode_Original.GetUIntPtr() + 0x1);
			SKSE::GetTrampoline()->Write5Branch(Offsets::IsInMenuMode_Hook.GetUIntPtr(), (uintptr_t)IsInMenuMode);
			SKSE::SafeWrite16(Offsets::IsInMenuMode_Hook.GetUIntPtr() + 0x5, 0x9090);
		}
	}
}