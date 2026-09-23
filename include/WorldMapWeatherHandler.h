#pragma once

class WorldMapWeatherHandler
{
public:
	static RE::TESWeather* GetUniqueWeather(const std::string& a_fileName);

private:
	WorldMapWeatherHandler() = default;
};
