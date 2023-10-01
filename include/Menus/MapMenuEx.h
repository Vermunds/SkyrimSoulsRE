#pragma once
#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	// menuDepth = 3
	// flags = kPausesGame | kUsesCursor | kRendersOffscreenTargets | kCustomRendering
	// context = kMap
	class MapMenuEx : public RE::MapMenu
	{
	public:
		class MapMenuCellLoadedEventHandler : RE::BSTEventSink<RE::TESCellFullyLoadedEvent>
		{
		public:
			// Event handler for newly loaded cells, as we have to turn on map mode rendering
			// Otherwise they will show up in full-resolution as if they were viewed from above, usually as black squares

			void Register();
			void Unregister();

			RE::BSEventNotifyControl ProcessEvent(const RE::TESCellFullyLoadedEvent* a_event, RE::BSTEventSource<RE::TESCellFullyLoadedEvent>* a_eventSource) override;
		};
		static inline MapMenuCellLoadedEventHandler mapMenuCellLoadedEventHandler;
		static inline bool cellRenderingUpdateNeeded = false;

		void AdvanceMovie_Hook(float a_interval, std::uint32_t a_currentTime);  // 05
		RE::UI_MESSAGE_RESULTS ProcessMessage_Hook(RE::UIMessage& a_message);   // 04

		void UpdatePlayerMarkerPosition();
		void UpdateClock();

		static RE::IMenu* Creator();
		static void InstallHook();

		static void BGSTerrainManager_Update_Hook(RE::BGSTerrainManager* a_this, std::uint64_t a_unk1, std::uint64_t a_unk2);
		static bool UpdateClouds_Hook(RE::NiAVObject* a_obj, RE::NiUpdateData* a_data);

		using ProcessMessage_t = decltype(&RE::MapMenu::ProcessMessage);
		static inline REL::Relocation<ProcessMessage_t> _ProcessMessage;

		using AdvanceMovie_t = decltype(&RE::MapMenu::AdvanceMovie);
		static inline REL::Relocation<AdvanceMovie_t> _AdvanceMovie;

		using TerrainManagerUpdate_t = decltype(BGSTerrainManager_Update_Hook);
		static inline std::function<TerrainManagerUpdate_t> _TerrainManagerUpdate;

		using UpdateClouds_t = decltype(UpdateClouds_Hook);
		static inline std::function<UpdateClouds_t> _UpdateClouds;
	};
}
