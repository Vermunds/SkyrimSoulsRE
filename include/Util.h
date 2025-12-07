#pragma once

namespace SkyrimSoulsRE::Util
{
	std::string GetRefDebugString(const RE::TESObjectREFR* a_form);
	std::wstring TranslateUIString(RE::GFxMovieView* a_view, const std::wstring& a_key);
}
