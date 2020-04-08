#include "SKSE/API.h"
#include "Offsets.h"

namespace SkyrimSoulsRE::Hooks
{
	namespace FavoritesMenu
	{
		void InstallHook()
		{
			//Fix for hotkeys not working
			//FavoritesHandler
			SKSE::SafeWrite16(Offsets::FavoritesMenu_Hook.GetUIntPtr(), 0x9090);
		}
	};
}