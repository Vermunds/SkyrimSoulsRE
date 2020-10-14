#pragma once
#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	template<class T>
	class InputHandlerEx : public T
	{
	public:
		using CanProcess_t = decltype(&T::CanProcess);
		static inline REL::Relocation<CanProcess_t> _CanProcess;

		bool CanProcess_Hook(RE::InputEvent* a_event);

		static void InstallHook(REL::Relocation<std::uintptr_t> a_vtbl, std::uint64_t a_offset);
	};

	template<class T>
	inline bool InputHandlerEx<T>::CanProcess_Hook(RE::InputEvent* a_event)
	{
		RE::UI* ui = RE::UI::GetSingleton();
		Settings* settings = Settings::GetSingleton();

		std::uint32_t unpausedMenuCount = SkyrimSoulsRE::GetUnpausedMenuCount();

		if (settings->enableMovementInMenus && typeid(T) == typeid(RE::MovementHandler))
		{
			return _CanProcess(this, a_event);
		}

		if (settings->enableGamepadCameraMove && typeid(T) == typeid(RE::LookHandler))
		{
			if (a_event->eventType == RE::INPUT_EVENT_TYPE::kThumbstick)
			{
				return _CanProcess(this, a_event);
			}
		}

		if (unpausedMenuCount || ui->IsMenuOpen(RE::Console::MENU_NAME))
		{
			return false;
		}

		return _CanProcess(this, a_event);
	}

	template<class T>
	inline void InputHandlerEx<T>::InstallHook(REL::Relocation<std::uintptr_t> a_vtbl, std::uint64_t a_offset)
	{
		_CanProcess = a_vtbl.write_vfunc(a_offset, &CanProcess_Hook);
	}
}