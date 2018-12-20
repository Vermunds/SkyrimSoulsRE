#pragma once

#include "Json2Settings.h"  // Json2Settings


namespace SkyrimSoulsRE
{
	class Settings : public Json2Settings::Settings
	{
	public:
		Settings() = delete;

		static bool	loadSettings(bool a_dumpParse = false);


		static aSetting<std::string> unpausedMenus;

	private:
		static constexpr char* FILE_NAME = "Data\\SKSE\\Plugins\\SkyrimSoulsRE.json";
	};
}
