#pragma once

#include "RE/Skyrim.h"

namespace SkyrimSoulsRE::Hooks
{
	namespace SleepWaitMenu
	{
		void Register_Func(RE::SleepWaitMenu* a_sleepWaitMenu);
		void InstallHook();
	}
}