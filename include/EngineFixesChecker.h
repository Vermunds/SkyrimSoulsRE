#pragma once
#include <toml++/toml.h>

namespace SkyrimSoulsRE
{
	class EngineFixesConfig
	{
	public:
		// Patches
		static inline bool patchMemoryManager = true;

		// Fixes
		static inline bool fixGlobalTime = true;

		static bool load_config(const std::string& a_path)
		{
			try
			{
				const toml::table tbl = toml::parse_file(a_path);
				patchMemoryManager = tbl["Patches"]["MemoryManager"].value_or(true);
				fixGlobalTime = tbl["Fixes"]["GlobalTime"].value_or(true);
			}
			catch (const toml::parse_error& e)
			{
				SKSE::log::error("Error parsing file {}: {}", a_path, e.description());
				return false;
			}

			return true;
		}
	};
}
