#pragma once
#include "AutoTOML.hpp"

namespace SkyrimSoulsRE
{
	class EngineFixesConfig
	{
	private:
		using ISetting = AutoTOML::ISetting;

	public:
		using bSetting = AutoTOML::bSetting;
		using fSetting = AutoTOML::fSetting;
		using iSetting = AutoTOML::iSetting;

		// Patches
		static inline bSetting patchMemoryManager{ "Patches", "MemoryManager", true };

		// Fixes
		static inline bSetting fixGlobalTime{ "Fixes", "GlobalTime", true };

		static bool load_config(const std::string& a_path)
		{
			try
			{
				const auto table = toml::parse_file(a_path);
				const auto& settings = ISetting::get_settings();
				for (const auto& setting : settings)
				{
					try
					{
						setting->load(table);
					}
					catch (const std::exception& e)
					{
						SKSE::log::warn(e.what());
					}
				}
			}
			catch (const toml::parse_error& e)
			{
				std::ostringstream ss;
				ss
					<< "Error parsing file \'" << *e.source().path
					<< "\':\n"
					<< e.description()
					<< "\n  (" << e.source().begin << ")\n";
				SKSE::log::error(ss.str());
				return false;
			}

			return true;
		}
	};
}