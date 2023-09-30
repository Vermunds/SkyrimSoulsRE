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
