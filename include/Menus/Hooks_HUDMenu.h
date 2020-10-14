#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	class HUDMenuEx : public RE::HUDMenu
	{
	public:

		static inline double stealthMeterPosX;
		static inline double stealthMeterPosY;

		void SetStealPickpocketHUDMode(bool a_isEnabled);

		static RE::IMenu* Creator();
		static void InstallHook();
	};
}
