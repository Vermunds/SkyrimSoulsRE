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

		class MapSky
		{
		public:
			MapSky();

			void Apply(RE::Sky* a_sky);
			void Finish(RE::Sky* a_sky);

		private:
			struct SkyState
			{
			public:
				void SaveState(RE::Sky* a_sky);
				void RestoreState(RE::Sky* a_sky) const;

			private:
				RE::TESClimate* currentClimate;
				RE::TESWeather* currentWeather;
				RE::TESWeather* lastWeather;
				RE::TESWeather* defaultWeather;
				RE::TESWeather* overrideWeather;
				RE::TESRegion* region;
				float currentGameHour;
				float currentWeatherPct;
				REX::EnumSet<RE::Sky::Flags> flags;
				std::uint32_t lastMoonPhaseUpdate;
				std::uint32_t unk174;
				std::uint32_t unk178;
				std::uint32_t unk17C;
				float fogPower;
				REX::EnumSet<RE::Sky::Mode> mode;
				float flash;
				std::uint32_t flashTime;
				RE::ObjectRefHandle currentRoom;
				RE::ObjectRefHandle previousRoom;
				RE::BGSLightingTemplate* extLightingOverride;
			};

			static inline auto ImageSpaceManager_Func1 = reinterpret_cast<void (*)(RE::ImageSpaceManager*, RE::ImageSpaceBaseData*)>(Offsets::ImageSpaceManager::MapWeatherFunc1.address());
			static inline auto ImageSpaceManager_Func2 = reinterpret_cast<void (*)(RE::ImageSpaceManager*)>(Offsets::ImageSpaceManager::MapWeatherFunc2.address());
			static inline auto ImageSpaceManager_Func3 = reinterpret_cast<void (*)(RE::ImageSpaceManager*)>(Offsets::ImageSpaceManager::MapWeatherFunc3.address());

			static inline auto Sky_UpdateSunGlareLensFlare = reinterpret_cast<void (*)(RE::Sky*)>(Offsets::Sky::UpdateSunGlareLensFlare.address());
			static inline auto Sky_UpdatePartial = reinterpret_cast<void (*)(RE::Sky*, float)>(Offsets::Sky::UpdatePartial.address());

			RE::TESWeather* m_mapWeather;
		};

		static inline MapMenuCellLoadedEventHandler mapMenuCellLoadedEventHandler;
		static inline bool cellRenderingUpdateNeeded = false;

		RE::UI_MESSAGE_RESULTS ProcessMessage_Hook(RE::UIMessage& a_message);  // 04

		void UpdatePlayerMarkerPosition();
		void Update();

		static RE::IMenu* Creator();
		static void InstallHook();

		static void BGSTerrainManager_Update_Hook(RE::BGSTerrainManager* a_this, std::uint64_t a_unk1, std::uint64_t a_unk2);
		static bool UpdateClouds_Hook(RE::NiAVObject* a_obj, RE::NiUpdateData* a_data);
		static bool UpdatePlayer_Hook(RE::PlayerCharacter* a_this);
		static void Sky_Update_Hook(RE::Sky* a_this, float a_timeDelta);

		using ProcessMessage_t = decltype(&RE::MapMenu::ProcessMessage);
		static inline REL::Relocation<ProcessMessage_t> _ProcessMessage;

		using TerrainManagerUpdate_t = decltype(BGSTerrainManager_Update_Hook);
		static inline std::function<TerrainManagerUpdate_t> _TerrainManagerUpdate;

		using UpdateClouds_t = decltype(UpdateClouds_Hook);
		static inline std::function<UpdateClouds_t> _UpdateClouds;

		using Sky_Update_t = decltype(Sky_Update_Hook);
		static inline std::function<Sky_Update_t> _SkyUpdate;

		static inline char lastTimeDateString[200] = { 0 };
		static inline std::unique_ptr<MapSky> mapSky;
	};
}
