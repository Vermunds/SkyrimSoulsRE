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

void ShowErrorMessage(std::string a_error, std::string a_desc = "")
{
	SKSE::log::critical(a_error.c_str());
	if (a_desc != "")
	{
		MessageBoxA(nullptr, (a_error + "\n\n" + a_desc + "\n\nPress OK to continue with the mod disabled.").c_str(), "Skyrim Souls RE - Error", MB_OK | MB_ICONERROR | MB_DEFBUTTON2 | MB_SYSTEMMODAL);
	}
	else
	{
		MessageBoxA(nullptr, (a_error + "\n\nPress OK to continue with the mod disabled.").c_str(), "Skyrim Souls RE - Error", MB_OK | MB_ICONERROR | MB_DEFBUTTON2 | MB_SYSTEMMODAL);
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
			ShowErrorMessage("Unsupported runtime version " + a_skse->RuntimeVersion().string());
			return false;
		}

		if (SKSE::AllocTrampoline(1 << 8))
		{
			SKSE::log::info("Trampoline creation successful.");
		}
		else {
			ShowErrorMessage("Trampoline creation failed!");
			return false;
		}

		//Check for kassents version
		if (std::filesystem::exists("Data/SKSE/Plugins/skyrimsouls.dll"))
		{
			ShowErrorMessage("A different version of Skyrim Souls is detected.", "Remove any old versions you have installed, or you won't be able to use this updated version.");
			return false;
		}

		//Check if Address Library is available
		std::string fileName = "Data/SKSE/Plugins/version-" + std::to_string(a_skse->RuntimeVersion()[0]) + "-" + std::to_string(a_skse->RuntimeVersion()[1]) + "-" + std::to_string(a_skse->RuntimeVersion()[2]) + "-" + std::to_string(a_skse->RuntimeVersion()[3]) + ".bin";
		if (!std::filesystem::exists(fileName))
		{
			ShowErrorMessage("Address Library for SKSE Plugins not found for current runtime version " + a_skse->RuntimeVersion().string(), "This mod requires it to function. Please install it before continuing.");
			return false;
		}

		return true;
	}

	bool SKSEPlugin_Load(SKSE::LoadInterface* a_skse)
	{
		if (!SKSE::Init(a_skse)) {
			ShowErrorMessage("SKSE initialization failed!", "Check your SKSE installation.");
			return false;
		}

		SKSE::log::info("Skyrim Souls RE loaded.");

		auto messaging = SKSE::GetMessagingInterface();
		if (messaging->RegisterListener("SKSE", MessageHandler)) {
			SKSE::log::info("Messaging interface registration successful.");
		} else {
			ShowErrorMessage("Messaging interface registration failed.");
			return false;
		}

		SkyrimSoulsRE::LoadSettings();
		SKSE::log::info("Settings successfully loaded.");

		SkyrimSoulsRE::InstallHooks();
		SKSE::log::info("Hooks installed.");

		return true;
	}
};
