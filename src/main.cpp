#include "EngineFixesChecker.h"
#undef MessageBox

#include "HookUtils.h"
#include "SkyrimSoulsRE.h"
#include "SteamOverlayFix.h"
#include "Version.h"

constexpr auto MESSAGEBOX_WARNING = 0x00001030L;  // MB_OK | MB_ICONWARNING | MB_SYSTEMMODAL

namespace
{
	void CheckEngineFixes(SkyrimSoulsRE::Settings* a_settings)
	{
		bool engineFixesPresent = REX::W32::GetModuleHandleA("EngineFixes.dll") &&
		                          SkyrimSoulsRE::EngineFixesConfig::load_config("Data/SKSE/Plugins/EngineFixes.toml") &&
		                          SkyrimSoulsRE::EngineFixesConfig::patchMemoryManager &&
		                          SkyrimSoulsRE::EngineFixesConfig::fixGlobalTime;

		if (engineFixesPresent)
		{
			SKSE::log::info("SSE Engine Fixes detected.");
			return;
		}

		SKSE::log::warn("SSE Engine Fixes not detected, or certain features are not enabled.");
		SKSE::log::warn("To ensure best functionality, the following Engine Fixes features must be enabled : Memory Manager patch, Global Time Fix");
		if (!a_settings->hideEngineFixesWarning)
		{
			REX::W32::MessageBoxA(nullptr, "SSE Engine Fixes not detected, or certain features are not enabled. This will not prevent Skyrim Souls RE from running, but to ensure best functionality, the following Engine Fixes features must be enabled:\n\n- Memory Manager patch\n- Global Time Fix\n\nThe Memory Manager patch prevents the false save corruption bug that tends to happen with this mod, and the Global Time fix fixes the behaviour of some menus when using the slow-motion feature.\n\nYou can disable this warning in the .ini.", "Skyrim Souls RE - Warning", MESSAGEBOX_WARNING);
		}
	}

	void CheckModCompatibility(SkyrimSoulsRE::Settings* a_settings)
	{
		if (REX::W32::GetModuleHandleA("DialogueMovementEnabler.dll"))
		{
			SKSE::log::info("Dialogue Movement Enabler detected. Enabling compatibility.");
			a_settings->isUsingDialogueMovementEnabler = true;
		}
		else
		{
			SKSE::log::info("Dialogue Movement Enabler not detected. Disabling compatibility.");
			a_settings->isUsingDialogueMovementEnabler = false;
		}

		if (REX::W32::GetModuleHandleA("gotobed.dll"))
		{
			SKSE::log::info("Go To Bed detected. Enabling compatibility.");
			a_settings->isUsingGoToBed = true;
		}
		else
		{
			SKSE::log::info("Go To Bed not detected. Disabling compatibility.");
			a_settings->isUsingGoToBed = false;
		}
	}

	// This will run immediately after SKSE::MessagingInterface::kDataLoaded
	void (*_PostDataLoaded)(RE::MemoryManager*) = nullptr;
	void PostDataLoaded_Hook(RE::MemoryManager* a_this)
	{
		_PostDataLoaded(a_this);

		SkyrimSoulsRE::InstallMenuHooks();
		SKSE::log::info("Menu hooks installed.");
		SkyrimSoulsRE::HookUtils::LogHooks();
	}
}

static void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type)
	{
	case SKSE::MessagingInterface::kPostLoad:
		{
			SkyrimSoulsRE::Settings* settings = SkyrimSoulsRE::Settings::GetSingleton();
			CheckEngineFixes(settings);
			CheckModCompatibility(settings);
		}
		break;
	}
}

extern "C"
{
	DLLEXPORT SKSE::PluginVersionData SKSEPlugin_Version = []() {
		SKSE::PluginVersionData v{};
		v.PluginVersion(REL::Version{ Version::MAJOR, Version::MINOR, Version::PATCH, 0 });
		v.PluginName(Version::NAME);
		v.AuthorName(Version::AUTHOR);
		v.UsesAddressLibrary();
		v.UsesUpdatedStructs();
		v.CompatibleVersions({ SKSE::RUNTIME_SSE_1_6_1170, SKSE::RUNTIME_SSE_1_6_1179 });
		return v;
	}();

	DLLEXPORT bool SKSEPlugin_Load(SKSE::LoadInterface* a_skse)
	{
		assert(SKSE::log::log_directory().has_value());
		auto path = SKSE::log::log_directory().value() / std::filesystem::path(Version::NAME.data() + ".log"s);
		auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path.string(), true);
		auto log = std::make_shared<spdlog::logger>("global log", std::move(sink));

		log->set_level(spdlog::level::trace);
		log->flush_on(spdlog::level::trace);

		spdlog::set_default_logger(std::move(log));
		spdlog::set_pattern("%s(%#): [%^%l%$] %v", spdlog::pattern_time_type::local);

		SKSE::log::info("{} v{} -({})", Version::FORMATTED_NAME, Version::STRING, __TIMESTAMP__);

		SKSE::AllocTrampoline(1 << 9, true);
		SKSE::Init(a_skse, false);

		// jota2rz-fork: install Steam-overlay pause fix as early as possible.
		// IAT-patches steam_api64!SteamAPI_RegisterCallback so Skyrim's
		// registration for GameOverlayActivated_t is silently dropped.
		SkyrimSoulsRE::SteamOverlayFix::Install();

		const SKSE::MessagingInterface* messaging = SKSE::GetMessagingInterface();
		if (messaging->RegisterListener("SKSE", MessageHandler))
		{
			SKSE::log::info("Messaging interface registration successful.");
		}
		else
		{
			SKSE::log::critical("Messaging interface registration failed.");
			return false;
		}

		SkyrimSoulsRE::LoadSettings();

		SkyrimSoulsRE::InstallHooks();
		_PostDataLoaded = reinterpret_cast<decltype(_PostDataLoaded)>(SkyrimSoulsRE::HookUtils::WriteCall<5>(Offsets::Main::InitData.address() + 0x421, (std::uintptr_t)PostDataLoaded_Hook));

		SKSE::log::info("Hooks installed.");

		SKSE::log::info("Skyrim Souls RE loaded.");

		return true;
	}
};
