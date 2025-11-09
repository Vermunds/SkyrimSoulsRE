#include "UIBlurManagerEx.h"
#include "Offsets.h"
#include "Settings.h"

namespace SkyrimSoulsRE
{
	void UIBlurManagerEx::IncrementBlurCount_Hook(RE::UIBlurManager* a_this)
	{
		Settings* settings = Settings::GetSingleton();
		if (!settings->disableBlur)
		{
			uint16_t& blurCount = a_this->blurCount;

			// Apply effect
			auto applyFunc = reinterpret_cast<uint64_t (*)(RE::TESImageSpaceModifier*, float, uint64_t)>(Offsets::TESImagespaceModifier::Apply.address());
			applyFunc(a_this->blurEffect, 1.0f, 0);

			++blurCount;
		}
	}

	void UIBlurManagerEx::InstallHook()
	{
		SKSE::GetTrampoline().write_branch<5>(RE::Offset::UIBlurManager::IncrementBlurCount.address(), (std::uintptr_t)IncrementBlurCount_Hook);
	}
}
