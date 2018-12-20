#include "common/IDebugLog.h"  // IDebugLog
#include "skse64_common/BranchTrampoline.h"  // g_branchTrampoline
#include "skse64_common/skse_version.h"  // RUNTIME_VERSION
#include "skse64/PluginAPI.h"  // PluginHandle, SKSEMessagingInterface, SKSETaskInterface, SKSEInterface, PluginInfo

#include <ShlObj.h>  // CSIDL_MYDOCUMENTS

#include "Events.h"  // g_menuOpenCloseEventHandler
#include "Hooks.h"  // InstallHooks()
#include "Settings.h"  // loadSettings
#include "version.h"  // SKYRIMSOULSRE_VERSION_VERSTRING

#include "HookShare.h"  // _RegisterForCanProcess_t

#include "RE/MenuManager.h"  // MenuManager


static PluginHandle	g_pluginHandle = kPluginHandle_Invalid;
static SKSEMessagingInterface* g_messaging = 0;


void HooksReady(SKSEMessagingInterface::Message* a_msg)
{
	using HookShare::_RegisterForCanProcess_t;

	switch (a_msg->type) {
	case HookShare::kType_CanProcess:
		if (a_msg->dataLen == HOOK_SHARE_API_VERSION_MAJOR) {
			_RegisterForCanProcess_t* _RegisterForCanProcess = static_cast<_RegisterForCanProcess_t*>(a_msg->data);
			Hooks::InstallHooks(_RegisterForCanProcess);
			_MESSAGE("[MESSAGE] Hooks registered");

			SkyrimSoulsRE::MenuOpenCloseEventHandler::Init();
			_MESSAGE("[MESSAGE] Menu open/close whitelist initialized");
		} else {
			_FATALERROR("[FATAL ERROR] An incompatible version of Hook Share SSE was loaded! Expected (%i), found (%i)!\n", HOOK_SHARE_API_VERSION_MAJOR, a_msg->type);
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
			_MESSAGE("[MESSAGE] Registered HookShareSSE listener");
		} else {
			_FATALERROR("[FATAL ERROR] HookShareSSE not loaded!\n");
		}
		break;
	case SKSEMessagingInterface::kMessage_DataLoaded:
	{
		RE::MenuManager* mm = RE::MenuManager::GetSingleton();
		mm->GetMenuOpenCloseEventSource()->AddEventSink(&SkyrimSoulsRE::g_menuOpenCloseEventHandler);
		_MESSAGE("[MESSAGE] Menu open/close event handler sinked");
		break;
	}
	}
}


extern "C" {
	bool SKSEPlugin_Query(const SKSEInterface* a_skse, PluginInfo* a_info)
	{
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Skyrim Special Edition\\SKSE\\SkyrimSoulsRE.log");
		gLog.SetPrintLevel(IDebugLog::kLevel_DebugMessage);
		gLog.SetLogLevel(IDebugLog::kLevel_DebugMessage);

		_MESSAGE("SkyrimSoulsRE v%s", SKYRIMSOULSRE_VERSION_VERSTRING);

		a_info->infoVersion = PluginInfo::kInfoVersion;
		a_info->name = "SkyrimSoulsRE";
		a_info->version = 1;

		g_pluginHandle = a_skse->GetPluginHandle();

		if (a_skse->isEditor) {
			_FATALERROR("[FATAL ERROR] Loaded in editor, marking as incompatible!\n");
			return false;
		}

		if (a_skse->runtimeVersion != RUNTIME_VERSION_1_5_62) {
			_FATALERROR("[FATAL ERROR] Unsupported runtime version %08X!\n", a_skse->runtimeVersion);
			return false;
		}

		if (g_branchTrampoline.Create(1024 * 8)) {
			_MESSAGE("[MESSAGE] Branch trampoline creation successful");
		} else {
			_FATALERROR("[FATAL ERROR] Branch trampoline creation failed!\n");
			return false;
		}

		return true;
	}


	bool SKSEPlugin_Load(const SKSEInterface* a_skse)
	{
		_MESSAGE("[MESSAGE] SkyrimSoulsRE loaded");

		g_messaging = (SKSEMessagingInterface*)a_skse->QueryInterface(kInterface_Messaging);
		if (g_messaging->RegisterListener(g_pluginHandle, "SKSE", MessageHandler)) {
			_MESSAGE("[MESSAGE] Messaging interface registration successful");
		} else {
			_FATALERROR("[FATAL ERROR] Messaging interface registration failed!\n");
			return false;
		}

		if (SkyrimSoulsRE::Settings::loadSettings()) {
			_MESSAGE("[MESSAGE] Loading settings was successful");
		} else {
			_FATALERROR("[FATAL ERROR] Loading settings failed!\n");
			return false;
		}

		return true;
	}
};
