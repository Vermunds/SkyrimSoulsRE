#include "SkyrimSoulsRE.h"
#include "version.h"

#include <Windows.h>
#include <spdlog\include\spdlog\sinks\msvc_sink.h>
#include <spdlog\include\spdlog\sinks\basic_file_sink.h>
#include <spdlog\spdlog.h>
#include <filesystem>

void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kDataLoaded:
		SkyrimSoulsRE::InstallMenuHooks();
		break;
	}
}

extern "C" {
	bool SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
	{
		auto path = SKSE::log::log_directory() / "SkyrimSoulsRE.log";
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

		if (a_skse->RuntimeVersion() != SKSE::RUNTIME_1_5_97) {
			SKSE::log::critical("Unsupported runtime version " + a_skse->RuntimeVersion().string());
			MessageBoxA(nullptr, std::string("Unsupported runtime version " + a_skse->RuntimeVersion().string()).c_str(), "Skyrim Souls RE - Error", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
			return false;
		}

		if (SKSE::AllocTrampoline(1 << 8))
		{
			SKSE::log::info("Trampoline creation successful.");
		}
		else {
			SKSE::log::critical("Trampoline creation failed.");
			MessageBoxA(nullptr, "Trampoline creation failed.", "Skyrim Souls RE - Error", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
			return false;
		}

		//Check for kassents version
		if (std::filesystem::exists("Data/SKSE/Plugins/skyrimsouls.dll"))
		{
			SKSE::log::critical("A different version of Skyrim Souls is detected.");
			MessageBoxA(nullptr, "A different version of Skyrim Souls is detected. The updated version will be disabled.", "Skyrim Souls RE - Error", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
			return false;
		}

		//Check if Address Library is available
		std::string fileName = "Data/SKSE/Plugins/version-" + a_skse->RuntimeVersion().string() + ".bin";
		if (!std::filesystem::exists(fileName))
		{
			SKSE::log::critical("A different version of Skyrim Souls is detected. The updated version will be disabled.");
			MessageBoxA(nullptr, std::string("Address Library for SKSE Plugins not found for current runtime version " + a_skse->RuntimeVersion().string() + "\nThe mod will be disabled.").c_str(), "Skyrim Souls RE - Error", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
			return false;
		}

		return true;
	}

	bool SKSEPlugin_Load(SKSE::LoadInterface* a_skse)
	{
		if (!SKSE::Init(a_skse)) {
			SKSE::log::critical("SKSE init failed.");
			MessageBoxA(nullptr, "SKSE init failed.", "Skyrim Souls RE - Error", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
			return false;
		}

		auto messaging = SKSE::GetMessagingInterface();
		if (messaging->RegisterListener("SKSE", MessageHandler)) {
			SKSE::log::info("Messaging interface registration successful.");
		} else {
			SKSE::log::critical("Messaging interface registration failed.");
			MessageBoxA(nullptr, "Messaging interface registration failed.", "Skyrim Souls RE - Error", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
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
