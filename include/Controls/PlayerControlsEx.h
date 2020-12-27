#pragma once
#include "RE/P/PlayerControls.h"

namespace SkyrimSoulsRE
{
	class PlayerControlsEx : public RE::PlayerControls
	{
	public:
		static void InstallHook();
	};
}
