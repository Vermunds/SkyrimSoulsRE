#include "Controls/BSWin32KeyboardDeviceEx.h"
#include "Offsets.h"
#include <Windows.h>

namespace SkyrimSoulsRE
{
	// The current keyboard layout does not update on the thread that processes inputs when the game is unpaused.
	// To avoid this, we use ToUnicodeEx and request the keyboard layout for the main thread.
	uint32_t BSWin32KeyboardDeviceEx::ToUnicode_Hook(uint32_t wVirtKey, uint32_t wScanCode, const uint8_t* lpKeyState, wchar_t* pwszBuff, int cchBuff, uint32_t wFlags)
	{
		RE::Main* main = RE::Main::GetSingleton();
		if (!main)
		{
			// Fallback
			return ToUnicode(wVirtKey, wScanCode, lpKeyState, pwszBuff, cchBuff, wFlags);
		}

		return ToUnicodeEx(wVirtKey, wScanCode, lpKeyState, pwszBuff, cchBuff, wFlags, GetKeyboardLayout(main->threadID));
	}

	void BSWin32KeyboardDeviceEx::InstallHook()
	{
		SKSE::GetTrampoline().write_call<6>(Offsets::BSWin32KeyboardDevice::Process.address() + 0x2CB, (std::uintptr_t)ToUnicode_Hook);
	}
}
