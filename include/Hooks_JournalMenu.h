#pragma once

#include "RE/JournalMenu.h"

namespace SkyrimSoulsRE::Hooks
{
	namespace JournalMenu
	{
		void Register_Func(RE::JournalMenu* a_journalMenu);
		void InstallHook();
	}
}