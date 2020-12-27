#include "Menus/Hooks_DialogueMenu.h"

namespace SkyrimSoulsRE
{
	//Prevent dialogue from closing when an another menu is open
	void DialogueMenuEx::UpdateAutoCloseTimer_Hook(uintptr_t a_unk, float a_delta)
	{
		uintptr_t unk = *(reinterpret_cast<uintptr_t*>(a_unk + 0x10));
		if (unk)
		{
			float* timer = reinterpret_cast<float*>(unk + 0x340);
			if (SkyrimSoulsRE::GetUnpausedMenuCount())
			{
				*timer = 120.0;
			}
			else
			{
				*timer += a_delta; //a_delta is negative
			}
		}
	}

	void DialogueMenuEx::AdvanceMovie_Hook(float a_interval, std::uint32_t a_currentTime)
	{
		std::uint32_t unpausedMenuCount = SkyrimSoulsRE::GetUnpausedMenuCount();

		if (this->uiMovie->GetVisible() && unpausedMenuCount)
		{
			this->uiMovie->SetVisible(false);
			this->depthPriority = 0;
		}
		else if (!(this->uiMovie->GetVisible()) && !unpausedMenuCount)
		{
			this->uiMovie->SetVisible(true);
			this->depthPriority = 3;
		}

		_AdvanceMovie(this, a_interval, a_currentTime);
	}

	void DialogueMenuEx::InstallHook()
	{
		//Hook AdvanceMovie
		REL::Relocation<std::uintptr_t> vTable(Offsets::Menus::DialogueMenu::Vtbl);
		_AdvanceMovie = vTable.write_vfunc(0x5, &DialogueMenuEx::AdvanceMovie_Hook);

		SKSE::GetTrampoline().write_call<5>(Offsets::Menus::DialogueMenu::UpdateAutoCloseTimer_Hook.address() + 0x4F9, (uintptr_t)UpdateAutoCloseTimer_Hook);
	}
}
