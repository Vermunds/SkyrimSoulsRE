#pragma once

#include "HookShare.h"  // _RegisterHook_t


namespace Hooks
{
	enum Menu
	{
		kMenu_None,
		kMenu_Favorites
	};


	const char* GetMenuName(Menu a_menu);

	void InstallHooks(HookShare::_RegisterHook_t* a_register);
}
