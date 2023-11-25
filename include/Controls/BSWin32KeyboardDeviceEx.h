#pragma once

namespace SkyrimSoulsRE
{
	class BSWin32KeyboardDeviceEx : RE::BSWin32KeyboardDevice
	{
		static uint32_t ToUnicode_Hook(uint32_t wVirtKey, uint32_t wScanCode, const uint8_t* lpKeyState, wchar_t* pwszBuff, int cchBuff, uint32_t wFlags);

	public:
		static void InstallHook();
	};
}
