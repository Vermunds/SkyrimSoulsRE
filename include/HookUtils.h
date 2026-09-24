#pragma once

namespace SkyrimSoulsRE::HookUtils
{
	namespace Detail
	{
		inline std::string FormatCallsite(const std::source_location& a_loc)
		{
			std::string_view filePath = a_loc.file_name();
			std::size_t slash = filePath.find_last_of("/\\");
			std::string_view fileName = (slash == std::string_view::npos) ? filePath : filePath.substr(slash + 1);

			return std::format("{}:{}", fileName, a_loc.line());
		}
	}
	void LogHooks();

	// Internal sinks — keep template bloat out of the header.
	void RecordVtableHook(std::uintptr_t a_vtableAddress, std::size_t a_idx, std::uintptr_t a_originalFunc, std::uintptr_t a_newFunc, std::string a_callsite);
	void RecordCallHook(std::uintptr_t a_src, std::size_t a_size, std::uintptr_t a_originalDst, std::uintptr_t a_stub, std::uintptr_t a_hookFunc, std::string a_callsite);
	void RecordPatchBytes(std::uintptr_t a_address, std::vector<std::uint8_t> a_before, std::vector<std::uint8_t> a_after, std::string a_callsite);

	template <class F>
	std::uintptr_t WriteVFunc(REL::Relocation<std::uintptr_t>& a_vtable, std::size_t a_idx, F a_newFunc, std::source_location a_loc = std::source_location::current())
	{
		std::uintptr_t original = a_vtable.write_vfunc(a_idx, a_newFunc);
		std::uintptr_t newFuncAddr = REX::UNRESTRICTED_CAST<std::uintptr_t>(a_newFunc);
		RecordVtableHook(a_vtable.address(), a_idx, original, newFuncAddr, Detail::FormatCallsite(a_loc));
		return original;
	}

	template <std::size_t N, class F>
	std::uintptr_t WriteCall(std::uintptr_t a_src, F a_newDst, std::source_location a_loc = std::source_location::current())
	{
		std::uintptr_t newDst = REX::UNRESTRICTED_CAST<std::uintptr_t>(a_newDst);
		std::uintptr_t original = REL::GetTrampoline().write_call<N>(a_src, newDst);
		std::int32_t disp = *reinterpret_cast<std::int32_t*>(a_src + N - 4);
		std::uintptr_t stub = static_cast<std::uintptr_t>(static_cast<std::int64_t>(a_src + N) + disp);
		RecordCallHook(a_src, N, original, stub, newDst, Detail::FormatCallsite(a_loc));
		return original;
	}

	template <std::size_t N, class F>
	std::uintptr_t WriteBranch(std::uintptr_t a_src, F a_newDst, std::source_location a_loc = std::source_location::current())
	{
		std::uintptr_t newDst = REX::UNRESTRICTED_CAST<std::uintptr_t>(a_newDst);
		std::uintptr_t original = REL::GetTrampoline().write_jmp<N>(a_src, newDst);
		std::int32_t disp = *reinterpret_cast<std::int32_t*>(a_src + N - 4);
		std::uintptr_t stub = static_cast<std::uintptr_t>(static_cast<std::int64_t>(a_src + N) + disp);
		RecordCallHook(a_src, N, original, stub, newDst, Detail::FormatCallsite(a_loc));
		return original;
	}

	template <class T>
	void SafeWrite(std::uintptr_t a_address, T a_value, std::source_location a_loc = std::source_location::current())
	{
		constexpr std::size_t size = sizeof(T);
		std::vector<std::uint8_t> before(size), after(size);
		std::memcpy(before.data(), reinterpret_cast<void*>(a_address), size);
		REL::WriteSafeData(a_address, a_value);
		std::memcpy(after.data(), reinterpret_cast<void*>(a_address), size);
		RecordPatchBytes(a_address, std::move(before), std::move(after), Detail::FormatCallsite(a_loc));
	}

	inline void SafeWrite(std::uintptr_t a_address, const void* a_data, std::size_t a_size, std::source_location a_loc = std::source_location::current())
	{
		std::vector<std::uint8_t> before(a_size), after(a_size);
		std::memcpy(before.data(), reinterpret_cast<void*>(a_address), a_size);
		REL::WriteSafe(a_address, a_data, a_size);
		std::memcpy(after.data(), reinterpret_cast<void*>(a_address), a_size);
		RecordPatchBytes(a_address, std::move(before), std::move(after), Detail::FormatCallsite(a_loc));
	}

	inline void SafeFill(std::uintptr_t a_address, std::uint8_t a_value, std::size_t a_size, std::source_location a_loc = std::source_location::current())
	{
		std::vector<std::uint8_t> before(a_size), after(a_size, a_value);
		std::memcpy(before.data(), reinterpret_cast<void*>(a_address), a_size);
		REL::WriteSafeFill(a_address, a_value, a_size);
		RecordPatchBytes(a_address, std::move(before), std::move(after), Detail::FormatCallsite(a_loc));
	}
}
