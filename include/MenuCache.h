#pragma once

namespace SkyrimSoulsRE::MenuCache
{
	RE::GFxMovieView* CreateInstance_Hook(RE::GFxMovieDefImpl* a_this, const RE::GFxMovieDef::MemoryParams& a_memParams, bool a_initFirstFrame);
	void InstallHook();
}
