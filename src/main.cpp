#include "SkyrimSoulsRE.h"
#include "version.h"

#include <filesystem>

constexpr auto MESSAGE_BOX_TYPE = 0x00001010L; // MB_OK | MB_ICONERROR | MB_SYSTEMMODAL

void MessageHandler_DME(SKSE::MessagingInterface::Message* a_msg)
{
	SkyrimSoulsRE::Settings* settings = SkyrimSoulsRE::Settings::GetSingleton();
	settings->isUsingDME = true;
	SKSE::log::info("Dialogue Movement Enabler detected. Enabling compatibility.");
}

void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kPostPostLoad:
		if (!SKSE::GetMessagingInterface()->RegisterListener("Dialogue Movement Enabler", MessageHandler_DME))
		{
			SKSE::log::info("Dialogue Movement Enabler not detected. Disabling compatibility.");
			// disabled by default
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

		SKSE::log::info("Skyrim Souls RE - Updated v" + std::string(SKYRIMSOULSRE_VERSION_VERSTRING));

		a_info->infoVersion = SKSE::PluginInfo::kVersion;
		a_info->name = "Skyrim Souls RE";
		a_info->version = SKYRIMSOULSRE_VERSION_MAJOR;

		if (a_skse->IsEditor()) {
			SKSE::log::critical("Loaded in editor, marking as incompatible!");
			return false;
		}

		if (a_skse->RuntimeVersion() < SKSE::RUNTIME_1_5_39) {
			SKSE::log::critical("Unsupported runtime version " + a_skse->RuntimeVersion().string());
			SKSE::WinAPI::MessageBox(nullptr, std::string("Unsupported runtime version " + a_skse->RuntimeVersion().string()).c_str(), "Skyrim Souls RE - Error", MESSAGE_BOX_TYPE);
			return false;
		}

		SKSE::AllocTrampoline(1 << 8, true);

		//Check for kassents version
		if (std::filesystem::exists("Data/SKSE/Plugins/skyrimsouls.dll"))
		{
			SKSE::log::critical("A different version of Skyrim Souls is detected.");
			SKSE::WinAPI::MessageBox(nullptr, "A different version of Skyrim Souls is detected. The updated version will be disabled.", "Skyrim Souls RE - Error", MESSAGE_BOX_TYPE);
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
		} else {
			SKSE::log::critical("Messaging interface registration failed.");
			SKSE::WinAPI::MessageBox(nullptr, "Messaging interface registration failed.", "Skyrim Souls RE - Error", MESSAGE_BOX_TYPE);
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
