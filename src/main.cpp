#include "EngineFixesChecker.h"
#undef MessageBox

#include "SkyrimSoulsRE.h"
#include "version.h"

#include <filesystem>

constexpr auto MESSAGEBOX_ERROR = 0x00001010L; // MB_OK | MB_ICONERROR | MB_SYSTEMMODAL
constexpr auto MESSAGEBOX_WARNING = 0x00001030L; // MB_OK | MB_ICONWARNING | MB_SYSTEMMODAL
void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kPostLoad:
	{
		SkyrimSoulsRE::Settings* settings = SkyrimSoulsRE::Settings::GetSingleton();

		bool engineFixesPresent = false;
		if (GetModuleHandle("EngineFixes.dll")) {
			if (SkyrimSoulsRE::EngineFixesConfig::load_config("Data/SKSE/Plugins/EngineFixes.toml")) {
				if (SkyrimSoulsRE::EngineFixesConfig::patchMemoryManager.get() && SkyrimSoulsRE::EngineFixesConfig::fixGlobalTime.get()) {
					SKSE::log::info("SSE Engine Fixes detected.");
					engineFixesPresent = true;
				}
			}
		}

		if (!engineFixesPresent) {
			SKSE::log::warn("SSE Engine Fixes not detected, or certain features are not enabled.");
			SKSE::log::warn("To ensure best functionality, the following Engine Fixes features must be enabled : Memory Manager patch, Global Time Fix");
			if (!settings->hideEngineFixesWarning) {
				SKSE::WinAPI::MessageBox(nullptr, "SSE Engine Fixes not detected, or certain features are not enabled. This will not prevent Skyrim Souls RE from running, but to ensure best functionality, the following Engine Fixes features must be enabled:\n\n- Memory Manager patch\n- Global Time Fix\n\nThe Memory Manager patch prevents the false save corruption bug that tends to happen with this mod, and the Global Time fix fixes the behaviour of some menus when using the slow-motion feature.\n\nIf you can't install SSE Engine Fixes for some reason (you're using Skyrim Together for example), you can disable this warning in the .ini.", "Skyrim Souls RE - Warning", MESSAGEBOX_WARNING);
			}
		}

		if (GetModuleHandle("DialogueMovementEnabler.dll"))
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
	bool SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
	{
		assert(SKSE::log::log_directory().has_value());
		auto path = SKSE::log::log_directory().value() / std::filesystem::path("SkyrimSoulsRE.log");
		auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path.string(), true);
		auto log = std::make_shared<spdlog::logger>("global log", std::move(sink));

		log->set_level(spdlog::level::trace);
		log->flush_on(spdlog::level::trace);

		spdlog::set_default_logger(std::move(log));
		spdlog::set_pattern("%g(%#): [%^%l%$] %v", spdlog::pattern_time_type::local);

		SKSE::log::info("Skyrim Souls RE - Updated v" + std::string(SKYRIMSOULSRE_VERSION_VERSTRING) + " - (" + std::string(__TIMESTAMP__) + ")");

		a_info->infoVersion = SKSE::PluginInfo::kVersion;
		a_info->name = "Skyrim Souls RE";
		a_info->version = SKYRIMSOULSRE_VERSION_MAJOR;

		if (a_skse->IsEditor()) {
			SKSE::log::critical("Loaded in editor, marking as incompatible!");
			return false;
		}

		if (a_skse->RuntimeVersion() < SKSE::RUNTIME_1_5_39) {
			SKSE::log::critical("Unsupported runtime version " + a_skse->RuntimeVersion().string());
			SKSE::WinAPI::MessageBox(nullptr, std::string("Unsupported runtime version " + a_skse->RuntimeVersion().string()).c_str(), "Skyrim Souls RE - Error", MESSAGEBOX_ERROR);
			return false;
		}

		SKSE::AllocTrampoline(1 << 9, true);

		//Check for kassents version
		if (std::filesystem::exists("Data/SKSE/Plugins/skyrimsouls.dll"))
		{
			SKSE::log::critical("A different version of Skyrim Souls is detected.");
			SKSE::WinAPI::MessageBox(nullptr, "A different version of Skyrim Souls is detected. The updated version will be disabled.", "Skyrim Souls RE - Error", MESSAGEBOX_ERROR);
			return false;
		}

		return true;
	}

	bool SKSEPlugin_Load(SKSE::LoadInterface* a_skse)
	{
		SKSE::Init(a_skse);

		auto messaging = SKSE::GetMessagingInterface();
		if (messaging->RegisterListener("SKSE", MessageHandler)) {
			SKSE::log::info("Messaging interface registration successful.");
		}
		else {
			SKSE::log::critical("Messaging interface registration failed.");
			SKSE::WinAPI::MessageBox(nullptr, "Messaging interface registration failed.", "Skyrim Souls RE - Error", MESSAGEBOX_ERROR);
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
