#pragma once

namespace SkyrimSoulsRE
{
	class UIBlurManagerEx : public RE::UIBlurManager
	{
		static void IncrementBlurCount_Hook(RE::UIBlurManager* a_this);

	public:
		static void InstallHook();
	};
}
