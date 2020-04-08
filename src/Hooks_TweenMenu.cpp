#include "Offsets.h"

namespace SkyrimSoulsRE::Hooks
{
	namespace TweenMenu
	{
		void InstallHook()
		{
			//Fix for camera movement
			UInt8 codes[] = { 0x90, 0x90, 0x90, 0x90, 0x90 };
			SKSE::SafeWriteBuf(Offsets::TweenMenu_Hook.GetUIntPtr(), codes, sizeof(codes));
		}
	}
}