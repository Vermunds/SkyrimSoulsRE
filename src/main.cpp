#include "skse64_common/skse_version.h"  // RUNTIME_VERSION

#include "Events.h"  // g_menuOpenCloseEventHandler
#include "Hooks.h"  // InstallHooks()
#include "Settings.h"  // loadSettings
#include "version.h"  // SKYRIMSOULSRE_VERSION_VERSTRING
#include "Tasks.h"

#include "RE/UI.h"

#include "SKSE/API.h"

void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSEMessagingInterface::kMessage_DataLoaded:
		RE::UI* ui = RE::UI::GetSingleton();
		ui->GetEventSource<RE::MenuOpenCloseEvent>()->AddEventSink(&SkyrimSoulsRE::g_menuOpenCloseEventHandler);
		_MESSAGE("Menu open/close event handler sinked");
		break;
	}
}

extern "C" {
	bool SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
	{
		SKSE::Logger::OpenRelative(FOLDERID_Documents, L"\\My Games\\Skyrim Special Edition\\SKSE\\SkyrimSoulsRE.log");
		SKSE::Logger::SetPrintLevel(SKSE::Logger::Level::kDebugMessage);
		SKSE::Logger::SetFlushLevel(SKSE::Logger::Level::kDebugMessage);
		SKSE::Logger::UseLogStamp(true);

		_MESSAGE("SkyrimSoulsRE v%s", SKYRIMSOULSRE_VERSION_VERSTRING);

		a_info->infoVersion = SKSE::PluginInfo::kVersion;
		a_info->name = "SkyrimSoulsRE";
		a_info->version = SKYRIMSOULSRE_VERSION_MAJOR;

		if (a_skse->IsEditor()) {
			_FATALERROR("Loaded in editor, marking as incompatible!\n");
			return false;
		}

		if (a_skse->RuntimeVersion() != RUNTIME_VERSION_1_5_97) {
			_FATALERROR("Unsupported runtime version %08X!\n", a_skse->RuntimeVersion());
			return false;
		}

		
		if (SKSE::AllocBranchTrampoline(1024 * 8))
		{
			_MESSAGE("Branch trampoline creation successful");
		} else {
			_FATALERROR("Branch trampoline creation failed!\n");
			return false;
		}
		
		if (SKSE::AllocLocalTrampoline(1024 * 8))
		{
			_MESSAGE("Local trampoline creation successful");
		}
		else {
			_FATALERROR("Local trampoline creation failed!\n");
			return false;
		}

		return true;
	}

	bool SKSEPlugin_Load(SKSE::LoadInterface* a_skse)
	{
		if (!SKSE::Init(a_skse)) {
			_FATALERROR("SKSE init failed!");
			return false;
		}
		_MESSAGE("Skyrim Souls RE loaded");

		auto messaging = SKSE::GetMessagingInterface();
		if (messaging->RegisterListener("SKSE", MessageHandler)) {
			_MESSAGE("Messaging interface registration successful");
		} else {
			_FATALERROR("Messaging interface registration failed!\n");
			return false;
		}

		SkyrimSoulsRE::LoadSettings();
		_MESSAGE("Settings successfully loaded.");

		Hooks::InstallHooks();
		_MESSAGE("Hooks installed.");

		return true;
	}
};
