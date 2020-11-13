#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	class HUDMenuEx : public RE::HUDMenu
	{
	public:
		static inline double stealthMeterPosX;
		static inline double stealthMeterPosY;

		void SetSkyrimSoulsMode(bool a_isEnabled);
		void UpdateHUD();

		static RE::IMenu* Creator();
		static void InstallHook();
	};
}
