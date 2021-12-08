#include "EngineFixesChecker.h"
#undef MessageBox

#include "SkyrimSoulsRE.h"
#include "version.h"

constexpr auto MESSAGEBOX_WARNING = 0x00001030L;  // MB_OK | MB_ICONWARNING | MB_SYSTEMMODAL

void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type)
	{
	case SKSE::MessagingInterface::kPostLoad:
		{
			SkyrimSoulsRE::Settings* settings = SkyrimSoulsRE::Settings::GetSingleton();

			bool engineFixesPresent = false;
			if (SKSE::WinAPI::GetModuleHandle("EngineFixes.dll"))
			{
				if (SkyrimSoulsRE::EngineFixesConfig::load_config("Data/SKSE/Plugins/EngineFixes.toml"))
				{
					if (SkyrimSoulsRE::EngineFixesConfig::patchMemoryManager.get() && SkyrimSoulsRE::EngineFixesConfig::fixGlobalTime.get())
					{
						SKSE::log::info("SSE Engine Fixes detected.");
						engineFixesPresent = true;
					}
				}
			}

			if (!engineFixesPresent)
			{
				SKSE::log::warn("SSE Engine Fixes not detected, or certain features are not enabled.");
				SKSE::log::warn("To ensure best functionality, the following Engine Fixes features must be enabled : Memory Manager patch, Global Time Fix");
				if (!settings->hideEngineFixesWarning)
				{
					SKSE::WinAPI::MessageBox(nullptr, "SSE Engine Fixes not detected, or certain features are not enabled. This will not prevent Skyrim Souls RE from running, but to ensure best functionality, the following Engine Fixes features must be enabled:\n\n- Memory Manager patch\n- Global Time Fix\n\nThe Memory Manager patch prevents the false save corruption bug that tends to happen with this mod, and the Global Time fix fixes the behaviour of some menus when using the slow-motion feature.\n\nIf you can't install SSE Engine Fixes for some reason (you're using Skyrim Together for example), you can disable this warning in the .ini.", "Skyrim Souls RE - Warning", MESSAGEBOX_WARNING);
				}
			}

			if (SKSE::WinAPI::GetModuleHandle("DialogueMovementEnabler.dll"))
			{
				SKSE::log::info("Dialogue Movement Enabler detected. Enabling compatibility.");
				settings->isUsingDME = true;
			}
			else
			{
				SKSE::log::info("Dialogue Movement Enabler not detected. Disabling compatibility.");
				settings->isUsingDME = false;
			}
		}
		break;
	case SKSE::MessagingInterface::kDataLoaded:
		SkyrimSoulsRE::InstallMenuHooks();
		break;
	}
}

extern "C" {

	DLLEXPORT SKSE::PluginVersionData SKSEPlugin_Version = []() {
		SKSE::PluginVersionData v{};
		v.PluginVersion(REL::Version{ Version::MAJOR, Version::MINOR, Version::PATCH, 0 });
		v.PluginName(Version::PROJECT);
		v.AuthorName("Vermunds"sv);
		v.CompatibleVersions({ SKSE::RUNTIME_LATEST });

		v.addressLibrary = true;
		v.sigScanning = false;
		return v;
	}();

	DLLEXPORT bool SKSEPlugin_Load(SKSE::LoadInterface* a_skse)
	{
		assert(SKSE::log::log_directory().has_value());
		auto path = SKSE::log::log_directory().value() / std::filesystem::path("SkyrimSoulsRE.log");
		auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path.string(), true);
		auto log = std::make_shared<spdlog::logger>("global log", std::move(sink));

		log->set_level(spdlog::level::trace);
		log->flush_on(spdlog::level::trace);

		spdlog::set_default_logger(std::move(log));
		spdlog::set_pattern("%g(%#): [%^%l%$] %v", spdlog::pattern_time_type::local);

		SKSE::log::info("Skyrim Souls RE - Updated v" + std::string(Version::NAME) + " - (" + std::string(__TIMESTAMP__) + ")");

		if (a_skse->IsEditor())
		{
			SKSE::log::critical("Loaded in editor, marking as incompatible!");
			return false;
		}

		SKSE::AllocTrampoline(1 << 9, true);
		SKSE::Init(a_skse);

		auto messaging = SKSE::GetMessagingInterface();
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
		SKSE::log::info("Settings loaded.");

		SkyrimSoulsRE::InstallHooks();
		SKSE::log::info("Hooks installed.");

		SKSE::log::info("Skyrim Souls RE loaded.");

		return true;
	}
};
