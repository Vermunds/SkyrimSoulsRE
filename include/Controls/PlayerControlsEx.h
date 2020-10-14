#pragma once
#include "RE/BSDevices/PlayerControls.h"

namespace SkyrimSoulsRE
{
	class PlayerControlsEx : public RE::PlayerControls
	{
	public:
		static void InstallHook();
	};
}