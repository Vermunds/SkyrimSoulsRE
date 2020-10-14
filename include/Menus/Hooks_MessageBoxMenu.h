#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	class MessageBoxMenuEx : public RE::MessageBoxMenu
	{
	public:
		static RE::IMenu* Creator();
		static void InstallHook();

		static void ButtonPress_Hook(const RE::FxDelegateArgs& a_args);

		static inline RE::FxDelegateHandler::CallbackFn* _ButtonPress;
	};
}