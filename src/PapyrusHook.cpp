#include "PapyrusHook.h"
#include "Settings.h"

namespace SkyrimSoulsRE
{
	namespace Papyrus
	{
		bool* isInMenuMode_1;
		bool* isInMenuMode_2;

		bool IsInMenuMode()
		{
			return *isInMenuMode_1 || *isInMenuMode_2 || SkyrimSoulsRE::GetUnpausedMenuCount() || RE::UI::GetSingleton()->IsMenuOpen(RE::Console::MENU_NAME);
		}

		void InstallHook() {
			isInMenuMode_1 = reinterpret_cast<bool*>(Offsets::Papyrus::IsInMenuMode::Value1.address());
			isInMenuMode_2 = reinterpret_cast<bool*>(Offsets::Papyrus::IsInMenuMode::Value2.address());
			SKSE::GetTrampoline().write_branch<5>(Offsets::Papyrus::IsInMenuMode::Hook.address(), IsInMenuMode);
		}
	}
}
