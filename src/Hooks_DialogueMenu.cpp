#include "Offsets.h"
#include "Events.h"
#include "RE/Skyrim.h"
#include "SKSE/API.h"

namespace SkyrimSoulsRE::Hooks
{
	namespace DialogueMenu
	{
		//Prevent dialogue from closing when an another menu is open
		void UpdateAutoCloseTimer(uintptr_t a_unk, float a_delta)
		{
			uintptr_t unk = *(reinterpret_cast<uintptr_t*>(a_unk + 0x10));
			if (unk)
			{
				float* timer = reinterpret_cast<float*>(unk + 0x340);
				if (SkyrimSoulsRE::unpausedMenuCount)
				{
					*timer = 120.0;
				}
				else
				{
					*timer += a_delta; //a_delta is negative
				}
			}
		}

		void InstallHook()
		{
			SKSE::GetTrampoline()->Write5Call(Offsets::DialogueMenu_UpdateAutoCloseTimer_Hook.GetUIntPtr(), (uintptr_t)UpdateAutoCloseTimer);
		}
	}
}