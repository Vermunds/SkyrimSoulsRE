#pragma once

#include "RE/Skyrim.h"
#include "REL/Relocation.h"
#include "REX/W32.h"
#include "SKSE/SKSE.h"

using namespace std::literals;

namespace logger
{
	template <class... T>
	using trace = REX::TRACE<T...>;
	template <class... T>
	using debug = REX::DEBUG<T...>;
	template <class... T>
	using info = REX::INFO<T...>;
	template <class... T>
	using warn = REX::WARN<T...>;
	template <class... T>
	using error = REX::ERROR<T...>;
	template <class... T>
	using critical = REX::CRITICAL<T...>;
}

#define DLLEXPORT __declspec(dllexport)
