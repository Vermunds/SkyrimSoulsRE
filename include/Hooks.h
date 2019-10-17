#pragma once

#include "HookShare.h"  // _RegisterForCanProcess_t

#include "RE/FxDelegate.h"
#include "RE/BGSSaveLoadManager.h"

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

	void Register_Func(RE::FxDelegate* a_delegate, HookType a_menu);

	void InstallHooks(HookShare::RegisterForCanProcess_t* a_register);

}
