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
			if (!a_this->blurCount)
			{
				RE::ImageSpaceModifierInstanceForm::Trigger(a_this->blurEffect, 1.0f, 0);
			}

			++a_this->blurCount;
		}
	}

	void UIBlurManagerEx::InstallHook()
	{
		SKSE::GetTrampoline().write_branch<5>(RE::Offset::UIBlurManager::IncrementBlurCount.address(), (std::uintptr_t)IncrementBlurCount_Hook);
	}
}
