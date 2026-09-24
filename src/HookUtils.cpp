#include "HookUtils.h"

namespace
{
	struct VtableHookEntry
	{
		std::uintptr_t vtableAddress;
		std::size_t index;
		std::uintptr_t originalFunc;
		std::uintptr_t hookFunc;
		std::string callsite;
	};

	struct CallHookEntry
	{
		std::uintptr_t src;
		std::size_t size;  // instruction size (5 or 6) — needed to decode live rel32
		std::uintptr_t originalDst;
		std::uintptr_t stub;      // trampoline stub address written to the rel32
		std::uintptr_t hookFunc;  // our hook function
		std::string callsite;
	};

	struct PatchEntry
	{
		std::uintptr_t address;
		std::vector<std::uint8_t> before;
		std::vector<std::uint8_t> after;
		std::string callsite;
	};

	std::mutex g_hooksMutex;
	std::vector<VtableHookEntry> g_vtableHooks;
	std::vector<CallHookEntry> g_callHooks;
	std::vector<PatchEntry> g_patches;

	// Matches the fixed layout of DbgHelp's SYMBOL_INFO (64-bit).
	// SizeOfStruct must be set to 88 (sizeof SYMBOL_INFO with Name[1], aligned).
	struct SymInfoBuffer
	{
		std::uint32_t sizeOfStruct;  // 0x00
		std::uint32_t typeIndex;     // 0x04
		std::uint64_t reserved[2];   // 0x08
		std::uint32_t index;         // 0x18
		std::uint32_t size;          // 0x1C
		std::uint64_t modBase;       // 0x20
		std::uint32_t flags;         // 0x28
		std::uint32_t pad;           // 0x2C
		std::uint64_t value;         // 0x30
		std::uint64_t address;       // 0x38
		std::uint32_t reg;           // 0x40
		std::uint32_t scope;         // 0x44
		std::uint32_t tag;           // 0x48
		std::uint32_t nameLen;       // 0x4C
		std::uint32_t maxNameLen;    // 0x50
		char name[512];              // 0x54
	};

	using SymInitialize_t = std::int32_t(__stdcall*)(REX::W32::HANDLE, const char*, std::int32_t);
	using SymSetOptions_t = std::uint32_t(__stdcall*)(std::uint32_t);
	using SymFromAddr_t = std::int32_t(__stdcall*)(REX::W32::HANDLE, std::uint64_t, std::uint64_t*, SymInfoBuffer*);
	using GetModuleBaseName_t = std::uint32_t(__stdcall*)(REX::W32::HANDLE, REX::W32::HMODULE, char*, std::uint32_t);
	using GetModuleHandleEx_t = std::int32_t(__stdcall*)(std::uint32_t, const char*, REX::W32::HMODULE*);

	struct DbgHelpSession
	{
		SymFromAddr_t symFromAddr = nullptr;
		GetModuleBaseName_t getModuleBaseName = nullptr;
		GetModuleHandleEx_t getModuleHandleEx = nullptr;
		bool initialized = false;

		void Init()
		{
			REX::W32::HMODULE dbghelp = REX::W32::GetModuleHandleA("dbghelp.dll");
			if (!dbghelp)
			{
				return;
			}

			SymInitialize_t symInitialize = reinterpret_cast<SymInitialize_t>(REX::W32::GetProcAddress(dbghelp, "SymInitialize"));
			SymSetOptions_t symSetOptions = reinterpret_cast<SymSetOptions_t>(REX::W32::GetProcAddress(dbghelp, "SymSetOptions"));
			symFromAddr = reinterpret_cast<SymFromAddr_t>(REX::W32::GetProcAddress(dbghelp, "SymFromAddr"));

			if (!symInitialize || !symSetOptions || !symFromAddr)
			{
				return;
			}

			// psapi for module name lookup (available on all supported Windows versions)
			if (REX::W32::HMODULE psapi = REX::W32::GetModuleHandleA("psapi.dll"))
			{
				getModuleBaseName = reinterpret_cast<GetModuleBaseName_t>(REX::W32::GetProcAddress(psapi, "GetModuleBaseNameA"));
			}
			getModuleHandleEx = reinterpret_cast<GetModuleHandleEx_t>(REX::W32::GetProcAddress(REX::W32::GetModuleHandleA("kernel32.dll"), "GetModuleHandleExA"));

			// Get the game directory for PDB search.
			char gameDir[REX::W32::MAX_PATH];
			REX::W32::GetModuleFileNameA(REX::W32::GetModuleHandleA("SkyrimSE.exe"), gameDir, REX::W32::MAX_PATH);
			if (char* lastSlash = strrchr(gameDir, '\\'))
			{
				*(lastSlash + 1) = '\0';
			}

			// SYMOPT_UNDNAME | SYMOPT_LOAD_LINES | SYMOPT_DEFERRED_LOADS
			symSetOptions(0x00000002 | 0x00000010 | 0x00000004);

			// GetCurrentProcess() (-1 pseudo-handle) is our own session, independent of
			// std::stacktrace which uses a real handle from OpenProcess(GetCurrentProcessId()).
			symInitialize(REX::W32::GetCurrentProcess(), gameDir, 1 /* fInvadeProcess */);
			initialized = true;
		}
	};

	DbgHelpSession g_dbgHelp;

	struct ModuleInfo
	{
		std::string name;
		std::uintptr_t base = 0;
	};

	ModuleInfo GetModuleInfo(std::uintptr_t a_address)
	{
		if (!g_dbgHelp.getModuleHandleEx || !g_dbgHelp.getModuleBaseName)
		{
			return {};
		}

		// GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT = 0x4 | 0x2
		REX::W32::HMODULE mod = nullptr;
		if (!g_dbgHelp.getModuleHandleEx(0x6, reinterpret_cast<const char*>(a_address), &mod))
		{
			return {};
		}

		char name[REX::W32::MAX_PATH];
		if (!g_dbgHelp.getModuleBaseName(REX::W32::GetCurrentProcess(), mod, name, static_cast<std::uint32_t>(sizeof(name))))
		{
			return {};
		}

		return { std::string(name), reinterpret_cast<std::uintptr_t>(mod) };
	}

	std::string ResolveSymbol(std::uintptr_t a_address)
	{
		if (a_address == 0)
		{
			return "nullptr";
		}

		ModuleInfo mod = GetModuleInfo(a_address);
		std::string modulePrefix = mod.name.empty() ? "" : mod.name + "!";

		if (g_dbgHelp.initialized)
		{
			SymInfoBuffer buf{};
			buf.sizeOfStruct = 88;
			buf.maxNameLen = static_cast<std::uint32_t>(sizeof(buf.name));
			std::uint64_t displacement = 0;
			if (g_dbgHelp.symFromAddr(REX::W32::GetCurrentProcess(), static_cast<uint64_t>(a_address), &displacement, &buf))
			{
				if (displacement != 0)
				{
					return std::format("{}{}+0x{:X}", modulePrefix, buf.name, displacement);
				}
				return std::format("{}{}", modulePrefix, buf.name);
			}
		}

		if (mod.base != 0)
		{
			return std::format("{}+0x{:X}", mod.name, a_address - mod.base);
		}

		return std::format("0x{:016X}", a_address);
	}

	std::string FormatBytes(const std::vector<std::uint8_t>& a_bytes)
	{
		std::string result;
		for (std::uint8_t byte : a_bytes)
		{
			if (!result.empty())
			{
				result += ' ';
			}
			result += std::format("{:02X}", byte);
		}
		return result;
	}
}

namespace SkyrimSoulsRE::HookUtils
{
	void RecordVtableHook(std::uintptr_t a_vtableAddress, std::size_t a_idx, std::uintptr_t a_originalFunc, std::uintptr_t a_newFunc, std::string a_callsite)
	{
		std::lock_guard lock(g_hooksMutex);
		g_vtableHooks.push_back({ a_vtableAddress, a_idx, a_originalFunc, a_newFunc, std::move(a_callsite) });
	}

	void RecordCallHook(std::uintptr_t a_src, std::size_t a_size, std::uintptr_t a_originalDst, std::uintptr_t a_stub, std::uintptr_t a_hookFunc, std::string a_callsite)
	{
		std::lock_guard lock(g_hooksMutex);
		g_callHooks.push_back({ a_src, a_size, a_originalDst, a_stub, a_hookFunc, std::move(a_callsite) });
	}

	void RecordPatchBytes(std::uintptr_t a_address, std::vector<std::uint8_t> a_before, std::vector<std::uint8_t> a_after, std::string a_callsite)
	{
		std::lock_guard lock(g_hooksMutex);
		g_patches.push_back({ a_address, std::move(a_before), std::move(a_after), std::move(a_callsite) });
	}

	void LogHooks()
	{
		g_dbgHelp.Init();
		std::lock_guard lock(g_hooksMutex);

		logger::info("--- Vtable hooks ({}) ---", g_vtableHooks.size());
		for (const VtableHookEntry& entry : g_vtableHooks)
		{
			std::uintptr_t slotAddr = entry.vtableAddress + entry.index * sizeof(void*);
			std::uintptr_t current = *reinterpret_cast<std::uintptr_t*>(slotAddr);
			if (current != entry.hookFunc)
			{
				logger::warn("  [OVERWRITTEN] vtable=0x{:016X} slot={} | original= {} | hook= {} | current= {} | from: {}",
					entry.vtableAddress, entry.index,
					ResolveSymbol(entry.originalFunc), ResolveSymbol(entry.hookFunc), ResolveSymbol(current),
					entry.callsite);
			}
			else
			{
				logger::info("  vtable=0x{:016X} slot={} | original= {} | hook= {} | current= {} | from: {}",
					entry.vtableAddress, entry.index,
					ResolveSymbol(entry.originalFunc), ResolveSymbol(entry.hookFunc), ResolveSymbol(current),
					entry.callsite);
			}
		}

		logger::info("--- Call/branch hooks ({}) ---", g_callHooks.size());
		for (const CallHookEntry& entry : g_callHooks)
		{
			std::int32_t disp = *reinterpret_cast<std::int32_t*>(entry.src + entry.size - 4);
			std::uintptr_t currentStub = static_cast<std::uintptr_t>(static_cast<std::int64_t>(entry.src + entry.size) + disp);
			if (currentStub != entry.stub)
			{
				logger::warn("  [OVERWRITTEN] loc= {} | original= {} | hook= {} | current= {} | from: {}",
					ResolveSymbol(entry.src),
					ResolveSymbol(entry.originalDst), ResolveSymbol(entry.hookFunc), ResolveSymbol(currentStub),
					entry.callsite);
			}
			else
			{
				// currentStub is an anonymous trampoline thunk — resolve hookFunc instead since that's where it leads.
				logger::info("  loc= {} | original= {} | hook= {} | current= {} | from: {}",
					ResolveSymbol(entry.src),
					ResolveSymbol(entry.originalDst), ResolveSymbol(entry.hookFunc), ResolveSymbol(entry.hookFunc),
					entry.callsite);
			}
		}

		logger::info("--- Byte patches ({}) ---", g_patches.size());
		for (const PatchEntry& entry : g_patches)
		{
			std::vector<std::uint8_t> current(entry.after.size());
			std::memcpy(current.data(), reinterpret_cast<void*>(entry.address), current.size());
			if (current != entry.after)
			{
				logger::warn("  [OVERWRITTEN] loc= {} | original= [{}] | hook= [{}] | current= [{}] | from: {}",
					ResolveSymbol(entry.address),
					FormatBytes(entry.before), FormatBytes(entry.after), FormatBytes(current),
					entry.callsite);
			}
			else
			{
				logger::info("  loc= {} | original= [{}] | hook= [{}] | current= [{}] | from: {}",
					ResolveSymbol(entry.address),
					FormatBytes(entry.before), FormatBytes(entry.after), FormatBytes(current),
					entry.callsite);
			}
		}
	}
}
