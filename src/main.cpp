#include "common/IDebugLog.h"  // IDebugLog
#include "skse64_common/BranchTrampoline.h"  // g_branchTrampoline
#include "skse64_common/skse_version.h"  // RUNTIME_VERSION
#include "skse64/PluginAPI.h"  // PluginHandle, SKSEMessagingInterface, SKSETaskInterface, SKSEInterface, PluginInfo

#include <ShlObj.h>  // CSIDL_MYDOCUMENTS

#include "Events.h"  // g_menuOpenCloseEventHandler
#include "Hooks.h"  // InstallHooks()
#include "Settings.h"  // loadSettings

#include "HookShare.h"  // _RegisterHook_t

#include "RE/MenuManager.h"  // MenuManager


static PluginHandle	g_pluginHandle = kPluginHandle_Invalid;
static SKSEMessagingInterface* g_messaging = 0;


void HooksReady(SKSEMessagingInterface::Message* a_msg)
{
	using HookShare::_RegisterHook_t;

	_RegisterHook_t* _RegisterHook = static_cast<_RegisterHook_t*>(a_msg->data);
	Hooks::InstallHooks(_RegisterHook);
	SkyrimSoulsRE::MenuOpenCloseEventHandler::Init();
	_MESSAGE("[MESSAGE] Menu open/close whitelist initialized\n");
}


void PluginsLoaded(SKSEMessagingInterface::Message* a_msg)
{
	g_messaging->RegisterListener(g_pluginHandle, "HookShareSSE", HooksReady);
}


void MessageHandler(SKSEMessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSEMessagingInterface::kMessage_PostPostLoad:
		g_messaging->RegisterListener(g_pluginHandle, "HookShareSSE", HooksReady);
		break;
	case SKSEMessagingInterface::kMessage_DataLoaded:
	{
		RE::MenuManager* mm = RE::MenuManager::GetSingleton();
		mm->MenuOpenCloseEventDispatcher()->AddEventSink(&SkyrimSoulsRE::g_menuOpenCloseEventHandler);
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

		_MESSAGE("SkyrimSoulsRE");

		a_info->infoVersion = PluginInfo::kInfoVersion;
		a_info->name = "SkyrimSoulsRE";
		a_info->version = 1;

		g_pluginHandle = a_skse->GetPluginHandle();

		if (a_skse->isEditor) {
			_FATALERROR("[FATAL ERROR] Loaded in editor, marking as incompatible!\n");
			return false;
		} else if (a_skse->runtimeVersion != RUNTIME_VERSION_1_5_53) {
			_FATALERROR("[FATAL ERROR] Unsupported runtime version %08X!\n", a_skse->runtimeVersion);
			return false;
		}

		return true;
	}


	bool SKSEPlugin_Load(const SKSEInterface* a_skse)
	{
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

		_MESSAGE("[MESSAGE] SkyrimSoulsRE loaded");

		return true;
	}
};
