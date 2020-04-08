#pragma once
#include "RE/Skyrim.h"

namespace SkyrimSoulsRE::Hooks
{
	namespace ContainerMenu
	{
		enum ContainerMode
		{
			kMode_Loot = 0,
			kMode_Steal = 1,
			kMode_Pickpocket = 2,
			kMode_FollowerTrade = 3
		};

		ContainerMode GetContainerMode();
		RE::TESObjectREFR* GetContainerRef();
		void InstallHook();
	}
}
