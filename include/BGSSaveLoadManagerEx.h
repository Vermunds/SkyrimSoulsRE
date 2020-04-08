#pragma once
#include "RE/BGSSaveLoadManager.h"

namespace SkyrimSoulsRE
{
	class BGSSaveLoadManagerEx : public RE::BGSSaveLoadManager
	{
	public:
		enum OutputFlag : UInt32
		{
			kNone = 0,
			kUnk01 = 1,
			kUnk02 = 1 << 1,
			kUnk03 = 1 << 2,
			kUnk04 = 1 << 3
		};

		enum SaveType
		{
			kAutoSave = 1,
			kSave = 2,
			kQuickSave = 4
		};

		bool SaveGame(SaveType a_saveType, OutputFlag a_outputFlag, const char* a_fileName);
	};
}