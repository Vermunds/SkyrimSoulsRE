#include "SteamOverlayFix.h"

#include <cstring>

// jota2rz-fork: minimal PE + Win32 declarations. We don't include <Windows.h>
// because it collides with CommonLibSSE's REX::W32 wrappers used elsewhere in
// SoulsRE. Just declare exactly what we need.

extern "C" {
	using DWORD    = unsigned long;
	using PVOID    = void*;

	__declspec(dllimport) PVOID __stdcall GetModuleHandleW(const wchar_t*);
	__declspec(dllimport) PVOID __stdcall GetProcAddress(PVOID, const char*);
	__declspec(dllimport) int __stdcall  VirtualProtect(PVOID, std::size_t, DWORD, DWORD*);
	__declspec(dllimport) int __cdecl    _stricmp(const char*, const char*);
}

namespace
{
	// --- PE structures (Windows.h subset, kept minimal) ------------------

	struct IMAGE_DOS_HEADER
	{
		std::uint16_t e_magic;      // Must be 'MZ' = 0x5A4D
		std::uint16_t e_ignored[29];
		std::int32_t  e_lfanew;     // Offset to IMAGE_NT_HEADERS
	};
	static_assert(sizeof(IMAGE_DOS_HEADER) == 64);

	struct IMAGE_DATA_DIRECTORY
	{
		std::uint32_t VirtualAddress;
		std::uint32_t Size;
	};

	struct IMAGE_OPTIONAL_HEADER64
	{
		std::uint16_t        Magic;
		std::uint8_t         reserved[110];  // opaque up to DataDirectory
		IMAGE_DATA_DIRECTORY DataDirectory[16];
	};

	struct IMAGE_FILE_HEADER
	{
		std::uint16_t Machine;
		std::uint16_t NumberOfSections;
		std::uint32_t TimeDateStamp;
		std::uint32_t PointerToSymbolTable;
		std::uint32_t NumberOfSymbols;
		std::uint16_t SizeOfOptionalHeader;
		std::uint16_t Characteristics;
	};

	struct IMAGE_NT_HEADERS64
	{
		std::uint32_t           Signature;   // Must be 'PE\0\0' = 0x4550
		IMAGE_FILE_HEADER       FileHeader;
		IMAGE_OPTIONAL_HEADER64 OptionalHeader;
	};

	struct IMAGE_IMPORT_DESCRIPTOR
	{
		std::uint32_t OriginalFirstThunk;  // RVA to IMAGE_THUNK_DATA[] (INT)
		std::uint32_t TimeDateStamp;
		std::uint32_t ForwarderChain;
		std::uint32_t Name;                // RVA to null-terminated DLL name
		std::uint32_t FirstThunk;          // RVA to IMAGE_THUNK_DATA[] (IAT)
	};

	struct IMAGE_IMPORT_BY_NAME
	{
		std::uint16_t Hint;
		char          Name[1];  // variable-length
	};

	union IMAGE_THUNK_DATA64
	{
		std::uint64_t ForwarderString;
		std::uint64_t Function;
		std::uint64_t Ordinal;
		std::uint64_t AddressOfData;  // RVA to IMAGE_IMPORT_BY_NAME
	};

	constexpr std::uint16_t IMAGE_DOS_SIGNATURE = 0x5A4D;   // 'MZ'
	constexpr std::uint32_t IMAGE_NT_SIGNATURE  = 0x00004550; // 'PE\0\0'
	constexpr std::size_t   IMAGE_DIRECTORY_ENTRY_IMPORT = 1;
	constexpr std::uint64_t IMAGE_ORDINAL_FLAG_64 = 0x8000000000000000ULL;
	constexpr DWORD         PAGE_READWRITE = 0x04;

	// --- Steamworks callback IDs -----------------------------------------

	constexpr int k_iSteamFriendsCallbacks              = 300;
	constexpr int kGameOverlayActivated_iCallback        = k_iSteamFriendsCallbacks + 31;  // 331

	// --- Hook state ------------------------------------------------------

	// The Steamworks C ABI: void SteamAPI_RegisterCallback(void* pCallback, int iCallback);
	using SteamAPI_RegisterCallback_t = void(__cdecl*)(void* pCallback, int iCallback);

	SteamAPI_RegisterCallback_t g_originalRegister = nullptr;
	bool                        g_installed        = false;

	// Diagnostic counters. We keep them lightweight -- log only the first
	// suppression and the install line. Every subsequent pass-through of
	// SteamAPI_RegisterCallback is silent to avoid log spam (Skyrim
	// registers dozens of callbacks at startup).
	std::uint64_t g_suppressedOverlayCount = 0;

	// Our replacement for SteamAPI_RegisterCallback. Cdecl ABI matches
	// Steamworks C exports (steam_api64.dll uses __cdecl).
	void __cdecl OurRegisterCallback(void* pCallback, int iCallback)
	{
		if (iCallback == kGameOverlayActivated_iCallback)
		{
			// Skyrim tried to register for GameOverlayActivated_t. We
			// silently drop the registration -- Steam never notifies
			// Skyrim, Skyrim never pauses on overlay-activated events.
			++g_suppressedOverlayCount;
			if (g_suppressedOverlayCount == 1)
			{
				SKSE::log::info(
					"[jota2rz-steam-cb] Suppressed GameOverlayActivated_t "
					"registration (this prevents Steam overlay from pausing "
					"the game); subsequent suppressions are silent");
			}
			return;
		}

		if (g_originalRegister)
		{
			g_originalRegister(pCallback, iCallback);
		}
	}

	// --- IAT hooking primitive -------------------------------------------

	// Walks the import table of the module at `moduleBase`, finds the entry
	// for `dllName!funcName`, and replaces the IAT slot's value with
	// `newFunc`. Writes the original slot value into `*outOriginal` so the
	// caller can chain-call it.
	//
	// Returns true on success.
	bool PatchImport(void* moduleBase, const char* dllName,
	                 const char* funcName, void* newFunc, void** outOriginal)
	{
		auto* base    = static_cast<std::uint8_t*>(moduleBase);
		auto* dosHdr  = reinterpret_cast<IMAGE_DOS_HEADER*>(base);
		if (dosHdr->e_magic != IMAGE_DOS_SIGNATURE)
		{
			SKSE::log::error("[jota2rz-steam-cb] PatchImport: bad DOS signature");
			return false;
		}

		auto* ntHdrs = reinterpret_cast<IMAGE_NT_HEADERS64*>(base + dosHdr->e_lfanew);
		if (ntHdrs->Signature != IMAGE_NT_SIGNATURE)
		{
			SKSE::log::error("[jota2rz-steam-cb] PatchImport: bad NT signature");
			return false;
		}

		auto& importDir = ntHdrs->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
		if (importDir.Size == 0)
		{
			SKSE::log::error("[jota2rz-steam-cb] PatchImport: no import table");
			return false;
		}

		auto* imports = reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(base + importDir.VirtualAddress);
		for (; imports->Name != 0; ++imports)
		{
			const auto* moduleName = reinterpret_cast<const char*>(base + imports->Name);
			if (_stricmp(moduleName, dllName) != 0)
			{
				continue;
			}

			// Found the DLL. Walk its thunks looking for our function name.
			// OriginalFirstThunk (INT) preserves the by-name info even after
			// the loader has resolved FirstThunk (IAT) to actual addresses.
			auto* nameThunk = reinterpret_cast<IMAGE_THUNK_DATA64*>(base + imports->OriginalFirstThunk);
			auto* iatThunk  = reinterpret_cast<IMAGE_THUNK_DATA64*>(base + imports->FirstThunk);
			if (imports->OriginalFirstThunk == 0)
			{
				// Bound import: no separate INT, only IAT. Fall back to IAT
				// walk (its entries are still by-name before binding).
				nameThunk = iatThunk;
			}

			for (; nameThunk->AddressOfData != 0; ++nameThunk, ++iatThunk)
			{
				// Skip ordinal imports (rare, and we look up by name).
				if (nameThunk->Ordinal & IMAGE_ORDINAL_FLAG_64)
				{
					continue;
				}
				auto* nameData = reinterpret_cast<IMAGE_IMPORT_BY_NAME*>(base + nameThunk->AddressOfData);
				if (std::strcmp(nameData->Name, funcName) != 0)
				{
					continue;
				}

				// Found it. Patch the IAT slot.
				void** slot = reinterpret_cast<void**>(&iatThunk->Function);
				DWORD  oldProtect = 0;
				if (!VirtualProtect(slot, sizeof(void*), PAGE_READWRITE, &oldProtect))
				{
					SKSE::log::error(
						"[jota2rz-steam-cb] PatchImport: VirtualProtect(RW) failed for {}!{}",
						dllName, funcName);
					return false;
				}
				*outOriginal = *slot;
				*slot        = newFunc;
				VirtualProtect(slot, sizeof(void*), oldProtect, &oldProtect);

				SKSE::log::info(
					"[jota2rz-steam-cb] PatchImport OK: {}!{} slot=0x{:x} original=0x{:x} newFunc=0x{:x}",
					dllName, funcName,
					reinterpret_cast<std::uintptr_t>(slot),
					reinterpret_cast<std::uintptr_t>(*outOriginal),
					reinterpret_cast<std::uintptr_t>(newFunc));
				return true;
			}

			// Correct DLL but function not in imports.
			SKSE::log::warn(
				"[jota2rz-steam-cb] PatchImport: {} not imported from {}",
				funcName, dllName);
			return false;
		}

		// DLL not in imports.
		SKSE::log::warn("[jota2rz-steam-cb] PatchImport: {} not in import list", dllName);
		return false;
	}
}

namespace SkyrimSoulsRE::SteamOverlayFix
{
	void Install()
	{
		if (g_installed)
		{
			return;
		}

		// Hook the SkyrimSE.exe main-module import for
		// steam_api64!SteamAPI_RegisterCallback.
		void* exeBase = GetModuleHandleW(nullptr);
		if (!exeBase)
		{
			SKSE::log::error("[jota2rz-steam-cb] GetModuleHandleW(NULL) returned null");
			return;
		}

		void* original = nullptr;
		if (!PatchImport(exeBase, "steam_api64.dll", "SteamAPI_RegisterCallback",
		                 reinterpret_cast<void*>(&OurRegisterCallback), &original))
		{
			SKSE::log::error("[jota2rz-steam-cb] Install failed");
			return;
		}

		g_originalRegister = reinterpret_cast<SteamAPI_RegisterCallback_t>(original);
		g_installed        = true;
	}
}
