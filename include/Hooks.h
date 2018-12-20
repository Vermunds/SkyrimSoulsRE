#pragma once

#include "HookShare.h"  // _RegisterForCanProcess_t


namespace Hooks
{
	enum Menu
	{
		kMenu_None,
		kMenu_Favorites
	};


	const char* GetMenuName(Menu a_menu);

	void InstallHooks(HookShare::_RegisterForCanProcess_t* a_register);
}
