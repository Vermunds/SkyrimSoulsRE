#include "PapyrusHook.h"

namespace SkyrimSoulsRE
{
	namespace Papyrus
	{
		bool* isInMenuMode_1;
		bool* isInMenuMode_2;

		bool IsInMenuMode()
		{
			if (*isInMenuMode_1 || *isInMenuMode_2 || SkyrimSoulsRE::GetUnpausedMenuCount()) {
				return true;
			}
			return false;
		}

		void InstallHook() {
			isInMenuMode_1 = reinterpret_cast<bool*>(Offsets::Papyrus::IsInMenuMode::Value1.address());
			isInMenuMode_2 = reinterpret_cast<bool*>(Offsets::Papyrus::IsInMenuMode::Value2.address());
			SKSE::GetTrampoline().write_call<5>(Offsets::Papyrus::IsInMenuMode::Hook.address(), IsInMenuMode);
		}
	}
}