#pragma once

namespace SkyrimSoulsRE::Util
{
	std::string GetRefDebugString(const RE::TESObjectREFR* a_form);
	std::wstring TranslateUIString(RE::GFxMovieView* a_view, const std::wstring& a_key);
	std::string TranslateSkyUIString(RE::GFxMovieView* a_view, const std::string& a_key);
	std::string FormatTimeRemaining(float seconds, const std::string& a_dayLabel, const std::string& a_hourLabel, const std::string& a_minuteLabel, const std::string& a_secondsLabel);
	bool IsActorValueMeterUpdateNeeded(float a_oldValue, float a_oldValueMax, float a_newValue, float a_newValueMax, std::int32_t a_numSteps);
	RE::TESObjectREFR* GetReferenceFromHandle(RE::RefHandle a_handle);
}
