#pragma once
#include "REL/Relocation.h"

namespace Offsets
{
	namespace ConsoleCommands
	{
		static constexpr REL::ID CenterOnCell_Hook(static_cast<std::uint64_t>(22873));      // + 0x5B
		static constexpr REL::ID CenterOnWorld_Hook(static_cast<std::uint64_t>(22876));     // + 0x104
		static constexpr REL::ID CenterOnExterior_Hook(static_cast<std::uint64_t>(22875));  // + 0x109
		static constexpr REL::ID ServeTime_Hook(static_cast<std::uint64_t>(22628));         // + 0xE
		static constexpr REL::ID SaveGame_Hook(static_cast<std::uint64_t>(22940));          // + 0xC4
	}

	namespace BGSTerrainManager
	{
		static constexpr REL::ID TerrainManager_UpdateFunc(static_cast<std::uint64_t>(39102));  // + 0x5D
	}

	namespace BSAudioManager
	{
		static constexpr REL::ID Hook(static_cast<std::uint64_t>(40449));  // + 0xC6; + 0x12E
		static constexpr REL::ID SetListenerPosition(static_cast<std::uint64_t>(67708));
		static constexpr REL::ID SetListenerRotation(static_cast<std::uint64_t>(67709));
	}

	namespace BSWin32KeyboardDevice
	{
		static constexpr REL::ID Process(static_cast<std::uint64_t>(68782));  // + 0x2CB
	}

	namespace GlobalTimescaleMultiplier
	{
		static constexpr REL::ID Value1(static_cast<std::uint64_t>(388442));
		static constexpr REL::ID Value2(static_cast<std::uint64_t>(388443));
	}

	namespace ItemMenuUpdater
	{
		static constexpr REL::ID RequestItemListUpdate(static_cast<std::uint64_t>(52849));

		static constexpr REL::ID RemoveAllItems(static_cast<std::uint64_t>(16118));
		static constexpr REL::ID RemoveAllItems_Hook1(static_cast<std::uint64_t>(16121));  // + 0x16
		static constexpr REL::ID RemoveAllItems_Hook2(static_cast<std::uint64_t>(19809));  // + 0x36
		static constexpr REL::ID RemoveAllItems_Hook3(static_cast<std::uint64_t>(22005));  // + 0xBA
		static constexpr REL::ID RemoveAllItems_Hook4(static_cast<std::uint64_t>(37496));  // + 0x255
		static constexpr REL::ID RemoveAllItems_Hook5(static_cast<std::uint64_t>(56215));  // + 0x46

		static constexpr REL::ID RemoveItem_Actor(static_cast<std::uint64_t>(37797));
		static constexpr REL::ID RemoveItem_TESObjectREFR(static_cast<std::uint64_t>(19689));

		static constexpr REL::ID RemoveItem_Hook1(static_cast<std::uint64_t>(22002));  // + 0x9A (6 call) - Actor - when called from Console (Drop)
		static constexpr REL::ID RemoveItem_Hook2(static_cast<std::uint64_t>(56125));  // + 0xDB (6 call) - Actor - when called from Payprus (DropObject)
		static constexpr REL::ID RemoveItem_Hook3(static_cast<std::uint64_t>(24715));  // + 0x472 (6 call) - Actor - when other NPCs interact
		static constexpr REL::ID RemoveItem_Hook4(static_cast<std::uint64_t>(17887));  // + 0x26A (6 call) - TESObjectREFR - when other NPCs interact
	}

	namespace Menus
	{
		namespace BarterMenu
		{
			static constexpr REL::ID UpdateBottomBar(static_cast<std::uint64_t>(50957));
		}

		namespace ContainerMenu
		{
			static constexpr REL::ID UpdateBottomBar(static_cast<std::uint64_t>(51143));
		}

		namespace DialogueMenu
		{
			static constexpr REL::ID UpdateAutoCloseTimer_Hook(static_cast<std::uint64_t>(37541));  // + 0x6E8
		}

		namespace FavoritesMenu
		{
			static constexpr REL::ID CanProcess(static_cast<std::uint64_t>(51538));

			static constexpr REL::ID SelectItem_HookFunc_1(static_cast<std::uint64_t>(51534));  // + 0x13A; + 0x166; + 0x192
			static constexpr REL::ID SelectItem_HookFunc_2(static_cast<std::uint64_t>(51537));  // + 0x45; + 0x73

			static constexpr REL::ID ItemSelect(static_cast<std::uint64_t>(51548));
		}

		namespace GiftMenu
		{
			static constexpr REL::ID UpdateBottomBar(static_cast<std::uint64_t>(51578));
		}

		namespace InventoryMenu
		{
			static constexpr REL::ID UpdateBottomBar(static_cast<std::uint64_t>(51865));
		}

		namespace LockpickingMenu
		{
			static constexpr REL::ID Hook(51950);  // + 0xE0
		}

		namespace MagicMenu
		{
			static constexpr REL::ID UpdateItemList(static_cast<std::uint64_t>(52043));  // + 0x53; + 0x9A
			static constexpr REL::ID AddSpells(static_cast<std::uint64_t>(38781));
			static constexpr REL::ID AddShout(static_cast<std::uint64_t>(52082));
			static constexpr REL::ID UpdateBottomBar(static_cast<std::uint64_t>(52042));
		}

		namespace MapMenu
		{
			static constexpr REL::ID Ctor(static_cast<std::uint64_t>(53093));  // + 0x52D

			static constexpr REL::ID LocalMapUpdaterFunc(static_cast<std::uint64_t>(53112));  // + 0x53; + 0x9D; + 0x9F

			static constexpr REL::ID UpdateClouds_Hook(static_cast<std::uint64_t>(53148));  // + 0x10E
			static constexpr REL::ID UpdateClouds_UpdateValue(static_cast<std::uint64_t>(391006));

			static constexpr REL::ID PlayerMarkerRefHandle(static_cast<std::uint64_t>(406633));
			static constexpr REL::ID UpdatePlayerMarkerPosFunc(static_cast<std::uint64_t>(53021));

			static constexpr REL::ID EnableMapModeRenderingFunc(static_cast<std::uint64_t>(53154));
		}

		namespace StatsMenu
		{
			static constexpr REL::ID ProcessMessage(static_cast<std::uint64_t>(52510));  // + 0x84E; + 0x852; + 0xA10; + 0xFC0; + 0xFC9 ; + 0x102D; + 0x1040
			static constexpr REL::ID CanProcess(static_cast<std::uint64_t>(52518));      // + 0x46; + 0x4A

			static constexpr REL::ID OpenStatsMenuAfterSleep_Hook(static_cast<std::uint64_t>(40417));  // + 0x65; + 0x69; + 0x6A
		}

		namespace TweenMenu
		{
			static constexpr REL::ID ProcessMessage(52705);  // + 0x4F3
		}
	}

	namespace Misc
	{
		static constexpr REL::ID CreateSaveScreenshot(static_cast<std::uint64_t>(36857));
		static constexpr REL::ID ScreenEdgeCameraMoveHook(static_cast<std::uint64_t>(42338));  // + 0x6D8
	}

	namespace Papyrus::IsInMenuMode
	{
		static constexpr REL::ID Hook(static_cast<std::uint64_t>(56833));
		static constexpr REL::ID Value1(static_cast<std::uint64_t>(403436));
		static constexpr REL::ID Value2(static_cast<std::uint64_t>(403437));
	}

	namespace TESImagespaceModifier
	{
		static constexpr REL::ID Apply(static_cast<std::uint64_t>(18570));
	}

	namespace UnpausedTaskQueue
	{
		static constexpr REL::ID Hook(static_cast<std::uint64_t>(36544));  // + 0x160
	}
}
