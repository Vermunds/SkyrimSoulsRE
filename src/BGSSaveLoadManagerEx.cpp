#include "BGSSaveLoadManagerEx.h"
#include "RE/Offsets.h"

bool SkyrimSoulsRE::BGSSaveLoadManagerEx::SaveGame(SaveType a_saveType, OutputFlag a_outputFlag, const char* a_fileName)
{
	using func_t = decltype(&BGSSaveLoadManagerEx::SaveGame);
	REL::Offset<func_t> func(RE::Offset::BGSSaveLoadManager::Save);
	return func(this, a_saveType, a_outputFlag, a_fileName);
}
