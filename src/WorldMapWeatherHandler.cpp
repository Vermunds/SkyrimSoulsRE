#include <json/json.h>

#include "WorldMapWeatherHandler.h"

// Code borrowed from Unique Map Weather by doodlum
// https://github.com/doodlum/skyrim-map-weather
// This is only going to run if Unique Map Weather is installed

namespace
{
	auto GetFormFromIdentifier(const std::string& a_identifier) -> RE::TESForm*
	{
		std::istringstream ss{ a_identifier };
		std::string plugin, id;

		std::getline(ss, plugin, '|');
		std::getline(ss, id);
		RE::FormID relativeID;
		std::istringstream{ id } >> std::hex >> relativeID;
		const auto dataHandler = RE::TESDataHandler::GetSingleton();
		return dataHandler ? dataHandler->LookupForm(relativeID, plugin) : nullptr;
	}
}

RE::BSResourceNiBinaryStream& operator>>(RE::BSResourceNiBinaryStream& a_sin, Json::Value& a_root)
{
	Json::CharReaderBuilder fact;
	std::unique_ptr<Json::CharReader> const reader{ fact.newCharReader() };

	auto size = a_sin.stream->totalSize;
	auto buffer = std::make_unique<char[]>(size);
	a_sin.read(buffer.get(), size);

	auto begin = buffer.get();
	auto end = begin + size;

	std::string errs;
	bool ok = reader->parse(begin, end, std::addressof(a_root), std::addressof(errs));

	if (!ok)
	{
		throw std::runtime_error{ errs };
	}

	return a_sin;
}

RE::TESWeather* WorldMapWeatherHandler::GetUniqueWeather(const std::string& worldspaceID)
{
	auto fileName = std::filesystem::path{ worldspaceID };
	fileName.replace_extension("json");
	fileName = std::filesystem::path{ "MapWeathers" } / fileName;
	RE::BSResourceNiBinaryStream a_fileStream{ fileName.string() };

	if (!a_fileStream.good())
	{
		return nullptr;
	}

	Json::Value root;
	a_fileStream >> root;

	SKSE::log::info("Reading file {}", fileName.string());

	Json::Value weathers = root["weathers"];
	if (weathers.isArray())
	{
		for (auto& weather : weathers)
		{
			if (!weather.isObject())
			{
				SKSE::log::warn("Failed to fetch weather from {}", fileName.string());
				continue;
			}

			auto formID = weather["formID"].asString();

			if (formID.empty())
			{
				SKSE::log::warn("Weather missing form ID in {}", fileName.string());
				continue;
			}

			auto weatherRef = skyrim_cast<RE::TESWeather*>(GetFormFromIdentifier(formID));

			if (!weatherRef)
			{
				SKSE::log::warn("'{}' did not correspond to a weather reference in {}", formID, fileName.string());
				continue;
			}

			return weatherRef;
		}
	}
	return nullptr;
}
