#pragma once
#include "REL/Relocation.h"

namespace Offsets
{
	namespace ConsoleCommands
	{
		static constexpr REL::ID CenterOnCell_Hook(static_cast<std::uint64_t>(22398));      // + 0x5B
		static constexpr REL::ID CenterOnWorld_Hook(static_cast<std::uint64_t>(22401));     // + 0x108
		static constexpr REL::ID CenterOnExterior_Hook(static_cast<std::uint64_t>(22400));  // + 0x118
		static constexpr REL::ID ServeTime_Hook(static_cast<std::uint64_t>(22145));         // + 0xE
		static constexpr REL::ID SaveGame_Hook(static_cast<std::uint64_t>(22465));          // + 0xC4
	}

	namespace BSAudioManager
	{
		static constexpr REL::ID Hook(static_cast<std::uint64_t>(39377));  // + 0xBC; + 0x124
		static constexpr REL::ID SetListenerPosition(static_cast<std::uint64_t>(66445));
		static constexpr REL::ID SetListenerRotation(static_cast<std::uint64_t>(66446));
	}

	namespace GlobalTimescaleMultiplier
	{
		static constexpr REL::ID Value1(static_cast<std::uint64_t>(511882));
		static constexpr REL::ID Value2(static_cast<std::uint64_t>(511883));
	}

	namespace ItemMenuUpdater
	{
		static constexpr REL::ID RequestItemListUpdate(static_cast<std::uint64_t>(51911));

		static constexpr REL::ID RemoveAllItems(static_cast<std::uint64_t>(15878));
		static constexpr REL::ID RemoveAllItems_Hook1(static_cast<std::uint64_t>(15881));  // + 0x16
		static constexpr REL::ID RemoveAllItems_Hook2(static_cast<std::uint64_t>(19382));  // + 0x36
		static constexpr REL::ID RemoveAllItems_Hook3(static_cast<std::uint64_t>(21523));  // + 0xBA
		static constexpr REL::ID RemoveAllItems_Hook4(static_cast<std::uint64_t>(36496));  // + 0x230
		static constexpr REL::ID RemoveAllItems_Hook5(static_cast<std::uint64_t>(55684));  // + 0x46

		static constexpr REL::ID RemoveItem_Actor(static_cast<std::uint64_t>(36781));
		static constexpr REL::ID RemoveItem_TESObjectREFR(static_cast<std::uint64_t>(19263));

		static constexpr REL::ID RemoveItem_Hook1(static_cast<std::uint64_t>(21520));  // + 0x9A (6 call) - Actor - when called from Console (Drop)
		static constexpr REL::ID RemoveItem_Hook2(static_cast<std::uint64_t>(55596));  // + 0xD9 (6 call) - Actor - when called from Payprus (DropObject)
		static constexpr REL::ID RemoveItem_Hook3(static_cast<std::uint64_t>(24211));  // + 0x461 (6 call) - Actor - when other NPCs interact
		static constexpr REL::ID RemoveItem_Hook4(static_cast<std::uint64_t>(17485));  // + 0x268 (6 call) - TESObjectREFR - when other NPCs interact
	}

	namespace Menus
	{
		namespace BarterMenu
		{
			static constexpr REL::ID Vtbl(static_cast<std::uint64_t>(267991));

			static constexpr REL::ID UpdateBottomBar(static_cast<std::uint64_t>(50013));
		}

		namespace BookMenu
		{
			static constexpr REL::ID Vtbl(static_cast<std::uint64_t>(268064));
		}

		namespace Console
		{
			static constexpr REL::ID Vtbl(static_cast<std::uint64_t>(268119));
		}

		namespace ContainerMenu
		{
			static constexpr REL::ID Vtbl(static_cast<std::uint64_t>(268222));

			static constexpr REL::ID UpdateBottomBar(static_cast<std::uint64_t>(50214));
		}

		namespace DialogueMenu
		{
			static constexpr REL::ID Vtbl(static_cast<std::uint64_t>(268589));

			static constexpr REL::ID UpdateAutoCloseTimer_Hook(static_cast<std::uint64_t>(36540));  // + 0x4F9
		}

		namespace FavoritesMenu
		{
			static constexpr REL::ID Vtbl(static_cast<std::uint64_t>(268567));

			static constexpr REL::ID CanProcess(static_cast<std::uint64_t>(50644));

			static constexpr REL::ID SelectItem_HookFunc_1(static_cast<std::uint64_t>(50640));
			static constexpr REL::ID SelectItem_HookFunc_2(static_cast<std::uint64_t>(50643));

			static constexpr REL::ID ItemSelect(static_cast<std::uint64_t>(50654));
		}

		namespace GiftMenu
		{
			static constexpr REL::ID Vtbl(static_cast<std::uint64_t>(268697));

			static constexpr REL::ID UpdateBottomBar(static_cast<std::uint64_t>(50682));
		}

		namespace HUDMenu
		{
			static constexpr REL::ID Vtbl(static_cast<std::uint64_t>(268816));
		}

		namespace InventoryMenu
		{
			static constexpr REL::ID Vtbl(static_cast<std::uint64_t>(269049));

			static constexpr REL::ID UpdateBottomBar(static_cast<std::uint64_t>(50986));
		}

		namespace JournalMenu
		{
			static constexpr REL::ID Vtbl(static_cast<std::uint64_t>(271141));

			static constexpr REL::ID RemapHandler_Vtbl(static_cast<std::uint64_t>(271032));
		}

		namespace LevelUpMenu
		{
			static constexpr REL::ID Vtbl(static_cast<std::uint64_t>(269130));
		}

		namespace LockpickingMenu
		{
			static constexpr REL::ID Hook(51071);  // + 0xE0
			static constexpr REL::ID Vtbl(static_cast<std::uint64_t>(269215));
		}

		namespace MagicMenu
		{
			static constexpr REL::ID Vtbl(static_cast<std::uint64_t>(269321));

			static constexpr REL::ID UpdateItemList(static_cast<std::uint64_t>(51163));
			static constexpr REL::ID AddSpells(static_cast<std::uint64_t>(37827));
			static constexpr REL::ID AddShout(static_cast<std::uint64_t>(51197));
			static constexpr REL::ID UpdateBottomBar(static_cast<std::uint64_t>(51162));
		}

		namespace MapMenu
		{
			static constexpr REL::ID Vtbl(static_cast<std::uint64_t>(270809));
			static constexpr REL::ID Ctor(static_cast<std::uint64_t>(52206));

			static constexpr REL::ID LocalMapUpdaterFunc(static_cast<std::uint64_t>(52225));

			namespace MapLookHandler
			{
				static constexpr REL::ID Vtbl(static_cast<std::uint64_t>(270806));
			}

			namespace MapMoveHandler
			{
				static constexpr REL::ID Vtbl(static_cast<std::uint64_t>(270807));

			}

			namespace MapZoomHandler
			{
				static constexpr REL::ID Vtbl(static_cast<std::uint64_t>(270808));
			}
		}

		namespace MessageBoxMenu
		{
			static constexpr REL::ID Vtbl(static_cast<std::uint64_t>(269561));
		}

		namespace ModManagerMenu
		{
			static constexpr REL::ID Vtbl(static_cast<std::uint64_t>(257372));
		}

		namespace StatsMenu
		{
			static constexpr REL::ID Vtbl(static_cast<std::uint64_t>(269955));

			static constexpr REL::ID ProcessMessage(static_cast<std::uint64_t>(51638));
			static constexpr REL::ID CanProcess(static_cast<std::uint64_t>(51645));

			static constexpr REL::ID OpenStatsMenuAfterSleep_Hook(static_cast<std::uint64_t>(39346));
		}

		namespace SleepWaitMenu
		{
			static constexpr REL::ID Vtbl(static_cast<std::uint64_t>(269872));
		}

		namespace TrainingMenu
		{
			static constexpr REL::ID Vtbl(static_cast<std::uint64_t>(270141));
		}

		namespace TutorialMenu
		{
			static constexpr REL::ID Vtbl(static_cast<std::uint64_t>(270169));
		}

		namespace TweenMenu
		{
			static constexpr REL::ID Vtbl(static_cast<std::uint64_t>(270195));

			static constexpr REL::ID Camera_Hook(51833);  // + 0x5A5
		}
	}

	namespace MenuControls
	{
		static constexpr REL::ID Vtbl(static_cast<std::uint64_t>(269528));
	}

	namespace Misc
	{
		static constexpr REL::ID CreateSaveScreenshot(static_cast<std::uint64_t>(35886));
		static constexpr REL::ID CursorPosition(static_cast<std::uint64_t>(525564));
		static constexpr REL::ID ScreenEdgeCameraMoveHook(static_cast<std::uint64_t>(41259));  // + 0x241
	}

	namespace Papyrus::IsInMenuMode
	{
		static constexpr REL::ID Hook(static_cast<std::uint64_t>(56476));
		static constexpr REL::ID Value1(static_cast<std::uint64_t>(516934));
		static constexpr REL::ID Value2(static_cast<std::uint64_t>(516935));
	}

	namespace UnpausedTaskQueue
	{
		static constexpr REL::ID Hook(static_cast<std::uint64_t>(35551));  // + 0x11F
		static constexpr REL::ID OriginalFunc(static_cast<std::uint64_t>(35565));
	}
}
