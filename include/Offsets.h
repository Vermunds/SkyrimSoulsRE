#pragma once
#include "REL/Relocation.h"

namespace Offsets
{
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

		static constexpr REL::ID RemoveAllItems(static_cast<std::uint64_t>(441568));
		static constexpr REL::ID RemoveAllItems_Hook1(static_cast<std::uint64_t>(16121));   // + 0x3A
		static constexpr REL::ID RemoveAllItems_Hook2(static_cast<std::uint64_t>(441567));  // + 0x55

		static constexpr REL::ID ResetInventory_TESObjectREFR_Hook(static_cast<std::uint64_t>(19802));  // + 0x204
	}

	namespace Job
	{
		static constexpr REL::ID UI(static_cast<std::uint64_t>(39042));  // + 0xB
	}

	namespace Main
	{
		static constexpr REL::ID Update(static_cast<std::uint64_t>(36564));  // + 0xADF
		static constexpr REL::ID Render(static_cast<std::uint64_t>(36555));  // + 0x5CA
	}

	namespace Menus
	{
		namespace BarterMenu
		{
			static constexpr REL::ID UpdateBottomBar(static_cast<std::uint64_t>(50957));
		}

		namespace BookMenu
		{
			static constexpr REL::ID ProcessMessage(static_cast<std::uint64_t>(51049));
		}

		namespace ContainerMenu
		{
			static constexpr REL::ID UpdateBottomBar(static_cast<std::uint64_t>(51143));
		}

		namespace CraftingMenu
		{
			static constexpr REL::ID UpdateBottomBar(static_cast<std::uint64_t>(51458));
		}

		namespace DialogueMenu
		{
			static constexpr REL::ID UpdateAutoCloseTimer_Hook(static_cast<std::uint64_t>(37541));  // + 0x6E8
		}

		namespace FavoritesMenu
		{
			static constexpr REL::ID CanProcess(static_cast<std::uint64_t>(51538));
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
		static constexpr REL::ID ScreenEdgeCameraMoveHook(static_cast<std::uint64_t>(42338));  // + 0x6D8

		static constexpr REL::ID GetExecuteConsoleCommandsSingleton(static_cast<std::uint64_t>(52950));
		static constexpr REL::ID ExecuteConsoleCommands(static_cast<std::uint64_t>(52952));
	}

	namespace Papyrus::IsInMenuMode
	{
		static constexpr REL::ID Hook(static_cast<std::uint64_t>(56833));
		static constexpr REL::ID Value1(static_cast<std::uint64_t>(403436));
		static constexpr REL::ID Value2(static_cast<std::uint64_t>(403437));
	}

	namespace PlayerCharacter::PlayerSkills
	{
		static constexpr REL::ID CanLevelUp(static_cast<std::uint64_t>(41565));
		static constexpr REL::ID GetXPAndLevelUpThreshold(static_cast<std::uint64_t>(41560));
	}

	namespace ScrapHeap
	{
		static constexpr REL::ID HookedFunc(static_cast<std::uint64_t>(82084));
	}

	namespace TESFurniture
	{
		static constexpr REL::ID Activate(static_cast<std::uint64_t>(17420));  // + 0x16A
	}

	namespace UI
	{
		static constexpr REL::ID ProcessMessages(static_cast<std::uint64_t>(82082));
		static constexpr REL::ID AdvanceMovies(static_cast<std::uint64_t>(82083));
	}
}
