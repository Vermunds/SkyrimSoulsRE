#pragma once

#include "RE/BGSSaveLoadManager.h"
#include "RE/IMenu.h"

namespace Hooks
{
	class BGSSaveLoadManagerEx : RE::BGSSaveLoadManager
	{
	public:
		enum DumpFlag : UInt32
		{
			kNone = 0,
			kUnk01 = 1,
			kUnk02 = 1 << 1,
			kUnk03 = 1 << 2,
			kUnk04 = 1 << 3
		};

		enum SaveMode
		{
			kAutoSave = 1,
			kSave = 2,
			kQuickSave = 4
		};
	};

	enum HookType
	{
		kSleepWaitMenu,
		kConsole,
		kMessageBoxMenu,
		kJournalMenu
	};

	void Register_Func(RE::IMenu* a_menu, HookType a_type);

	void InstallHooks();

}
