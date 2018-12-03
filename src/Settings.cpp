#include "settings.h"

#include <fstream>  // ifstream
#include <exception>  // exception
#include <vector>  // vector
#include <string>  // string

#include "json.hpp"  // json


namespace SkyrimSoulsRE
{
	bool Settings::loadSettings()
	{
		using nlohmann::json;

		std::ifstream istream(FILE_NAME);
		if (!istream.is_open()) {
			_ERROR("[ERROR] Failed to open .json file!\n");
		}
		json j;
		try {
			istream >> j;
			json::iterator it;
			for (auto& setting : settings) {
				it = j.find(setting->key());

				if (it == j.end()) {
					_ERROR("[ERROR] Failed to find (%s) within .json!\n", setting->key().c_str());
					continue;
				}

				switch (it->type()) {
				case json::value_t::array:
				{
					json jArr = it.value();
					setting->assign(jArr);
					break;
				}
				case json::value_t::string:
				{
					std::string str = it.value();
					setting->assign(str);
					break;
				}
				case json::value_t::boolean:
				{
					bool b = it.value();
					setting->assign(b);
					break;
				}
				case json::value_t::number_integer:
				case json::value_t::number_unsigned:
				{
					int num = it.value();
					setting->assign(num);
					break;
				}
				case json::value_t::number_float:
				{
					float num = it.value();
					setting->assign(num);
					break;
				}
				default:
					_DMESSAGE("[ERROR] Parsed value is of invalid type (%s)!\n", j.type_name());
				}
			}
		} catch (std::exception& e) {
			_ERROR("[ERROR] Failed to parse .json file!\n");
			_ERROR(e.what());
			istream.close();
			return false;
		}

		istream.close();
		return true;
	}


	void Settings::dump()
	{
		_DMESSAGE("=== SETTINGS DUMP BEGIN ===");
		for (auto& setting : settings) {
			setting->dump();
		}
		_DMESSAGE("=== SETTINGS DUMP END ===");
	}


	aSetting<std::string>	Settings::unpausedMenus("unpausedMenus", false, { "magicMenu", "barterMenu", "containerMenu", "journalMenu", "favoritesMenu", "tutorialMenu" });

	const char*				Settings::FILE_NAME = "Data\\SKSE\\Plugins\\SkyrimSoulsRE.json";
}
