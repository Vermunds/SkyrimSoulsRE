#include "skse64_common/BranchTrampoline.h"  // g_branchTrampoline
#include "skse64_common/skse_version.h"  // RUNTIME_VERSION
#include "skse64/PluginAPI.h"  // PluginHandle, SKSEMessagingInterface, SKSETaskInterface, SKSEInterface, PluginInfo

#include "Events.h"  // g_menuOpenCloseEventHandler
#include "Hooks.h"  // InstallHooks()
#include "Settings.h"  // loadSettings
#include "version.h"  // SKYRIMSOULSRE_VERSION_VERSTRING
#include "Tasks.h"

#include "HookShare.h"  // _RegisterForCanProcess_t

#include "RE/MenuManager.h"  // MenuManager


static PluginHandle	g_pluginHandle = kPluginHandle_Invalid;
static SKSEMessagingInterface* g_messaging = 0;

void HooksReady(SKSEMessagingInterface::Message* a_msg)
{
	using HookShare::RegisterForCanProcess_t;

	switch (a_msg->type) {
	case HookShare::kType_CanProcess:
		if (a_msg->dataLen == HookShare::kAPIVersionMajor) {
			RegisterForCanProcess_t* _RegisterForCanProcess = static_cast<RegisterForCanProcess_t*>(a_msg->data);
			Hooks::InstallHooks(_RegisterForCanProcess);
			_MESSAGE("Hooks registered");

			SkyrimSoulsRE::MenuOpenCloseEventHandler::Init();
			_MESSAGE("Menu open/close whitelist initialized");
		} else {
			_FATALERROR("An incompatible version of Hook Share SSE was loaded! Expected (%i), found (%i)!\n", HookShare::kAPIVersionMajor, a_msg->type);
		}
	}
}

void PluginsLoaded(SKSEMessagingInterface::Message* a_msg)
{
	g_messaging->RegisterListener(g_pluginHandle, "HookShareSSE", HooksReady);
}

void MessageHandler(SKSEMessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSEMessagingInterface::kMessage_PostPostLoad:
		if (g_messaging->RegisterListener(g_pluginHandle, "HookShareSSE", HooksReady)) {
			_MESSAGE("Registered HookShareSSE listener");
		}
		else {
			_FATALERROR("HookShareSSE not loaded!\n");
		}
		break;
	case SKSEMessagingInterface::kMessage_DataLoaded:
		RE::MenuManager* mm = RE::MenuManager::GetSingleton();
		mm->GetEventSource<RE::MenuOpenCloseEvent>()->AddEventSink(&SkyrimSoulsRE::g_menuOpenCloseEventHandler);
		_MESSAGE("Menu open/close event handler sinked");
		break;
	}
}

extern "C" {
	bool SKSEPlugin_Query(const SKSEInterface* a_skse, PluginInfo* a_info)
	{
		SKSE::Logger::OpenRelative(FOLDERID_Documents, L"\\My Games\\Skyrim Special Edition\\SKSE\\SkyrimSoulsRE.log");
		SKSE::Logger::SetPrintLevel(SKSE::Logger::Level::kDebugMessage);
		SKSE::Logger::SetFlushLevel(SKSE::Logger::Level::kDebugMessage);
		SKSE::Logger::UseLogStamp(true);

		_MESSAGE("SkyrimSoulsRE v%s", SKYRIMSOULSRE_VERSION_VERSTRING);

		a_info->infoVersion = PluginInfo::kInfoVersion;
		a_info->name = "SkyrimSoulsRE";
		a_info->version = 1;

		g_pluginHandle = a_skse->GetPluginHandle();

		if (a_skse->isEditor) {
			_FATALERROR("Loaded in editor, marking as incompatible!\n");
			return false;
		}

		if ((a_skse->runtimeVersion != RUNTIME_VERSION_1_5_80) && (a_skse->runtimeVersion != RUNTIME_VERSION_1_5_73)) {
			_FATALERROR("Unsupported runtime version %08X!\n", a_skse->runtimeVersion);
			return false;
		}

		if (g_branchTrampoline.Create(1024 * 8)) {
			_MESSAGE("Branch trampoline creation successful");
		} else {
			_FATALERROR("Branch trampoline creation failed!\n");
			return false;
		}

		if (g_localTrampoline.Create(1024 * 64, nullptr))
		{
			_MESSAGE("Local trampoline creation successful");
		}
		else {
			_FATALERROR("Local trampoline creation failed!\n");
			return false;
		}

		return true;
	}

	bool SKSEPlugin_Load(const SKSEInterface* a_skse)
	{
		_MESSAGE("SkyrimSoulsRE loaded");

		g_messaging = (SKSEMessagingInterface*)a_skse->QueryInterface(kInterface_Messaging);
		if (g_messaging->RegisterListener(g_pluginHandle, "SKSE", MessageHandler)) {
			_MESSAGE("Messaging interface registration successful");
		} else {
			_FATALERROR("Messaging interface registration failed!\n");
			return false;
		}

		SkyrimSoulsRE::LoadSettings();
		_MESSAGE("Settings successfully loaded.");

		if (Tasks::g_task = (SKSETaskInterface *)a_skse->QueryInterface(kInterface_Task)) {
			_MESSAGE("Task interface registration successful");
		} else {
			_FATALERROR("Task interface registration failed!\n");
			return false;
		}

		return true;
	}
};
