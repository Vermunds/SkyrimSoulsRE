# Skyrim Souls RE
[![Nexus Mods](https://img.shields.io/badge/NexusMods-Download-orange)](https://www.nexusmods.com/skyrimspecialedition/mods/27859)
[![GitHub release](https://img.shields.io/github/v/release/Vermunds/SkyrimSoulsRE)](https://github.com/Vermunds/SkyrimSoulsRE/releases)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](./LICENSE.md)

A mod for The Elder Scrolls V: Skyrim - Special Edition.

This mod unpauses all of Skyrim's in-game menus.

## Features
- Unpauses all of Skyrim’s game menus, individually configurable.
- Optional movement and camera-controls while menus are open.
- Optional slow-motion effect while menus are open.
- Fully integrated into game world with custom auto-close logic for relevant menus.
- Combat alert overlay for warning the user for dangers.
- Optionally disable background blur of menus.
- Fully customizable through an included .ini file.
- Fixed crashes and race conditions from earlier versions of similar mods.

## Download
Available on [Nexusmods](https://www.nexusmods.com/skyrimspecialedition/mods/27859) or via Github Releases.

## INI Configuration

The configuration file is located at:

`<Skyrim install folder>/Data/SKSE/Plugins/SkyrimSoulsRE.ini`

The configuration file is fully commented for clarity. Lines starting with `#` are comments and have no effect in-game.  
Delete the .ini file and launch the game once to regenerate the defaults in case you lost them.  
Note that some mod managers may place this file in different locations than the one above. In this case, refer to the documentation of your mod manager of choice.

## Information for mod creators
### Papyrus changes
This mod will change the papyrus function `Utility.IsInMenuMode()`. It will return true if an unpaused menu is open. However, `Utility.wait()` will no longer be usable to detect if a menu is open. If you used `Utility.wait()` for detecting menus, here is a recommended way to do it in order to be compatible with this mod:

```papyrus
While (Utility.IsInMenuMode())
    Utility.Wait(0.1)
EndWhile
```
In most cases, simply replacing the affected lines of `Utility.wait()` with the snippet above will make the mod compatible.  
Note that this change will NOT affect menus that are unpaused by default and are not covered by Skyrim Souls RE, like `DialogueMenu` or `CraftingMenu`.  
 It will also not affect `Console` as this menu is technically always open in the background.  
For reference, the following menus ARE affected:
- `BarterMenu`
- `BookMenu`
- `ContainerMenu`
- `FavoritesMenu`
- `GiftMenu`
- `InventoryMenu`
- `JournalMenu`
- `LevelUpMenu`
- `LockpickingMenu`
- `MagicMenu`
- `MapMenu`
- `MessageBoxMenu`
- `ModManagerMenu`
- `SleepWaitMenu`
- `StatsMenu`
- `TrainingMenu`
- `TutorialMenu`
- `TweenMenu`
- `CustomMenu`

### SWF assumptions
The mod makes some assumptions about where it can find various UI elements on the Scaleform frontend:
- ContainerMenu
    - Variables:
        - `_root.Menu_mc.isViewingContainer` - whether the player is viewing the container or the player inventory. Checked every frame.
        - `_root.Menu_mc.itemCardFadeHolder.StealTextInstance.PercentTextInstance` - the steal chance percent text instance. Modified every frame.
- HUDMenu
    - Variables:
        - `_root.HUDMovieBaseInstance.StealthMeterInstance` - Used by the logic to keep the stealth meter visible when a menu opens. Can be disabled by setting `bDisableHUDModifications` to true in the ini.
        - `_root.HUDMovieBaseInstance.StealthMeterInstance.SneakTextHolder.SneakTextClip` - same as above
        - `_root.HUDMovieBaseInstance.StealthMeterInstance.SneakTextHolder.SneakTextClip.SneakTextInstance` - same as above
        - `_root.HUDMovieBaseInstance.QuestUpdateBaseInstance` - used to always show quest updates even in menus
        - `_root.HUDMovieBaseInstance.MessagesBlock` - used to always show notifications even in menus
- JournalMenu
    - Variables:
        - `_root.QuestJournalFader.Menu_mc.BottomBar_mc.DateText` - Time/Date text instance. Modified every frame
        - `_root.QuestJournalFader.Menu_mc.SystemFader.Page_mc` - Save/Load list page instance.
        - `_root.QuestJournalFader.Menu_mc.SystemFader.Page_mc.SaveLoadListHolder.selectedIndex` - Selected save/load game index in the list
        - `_root.QuestJournalFader.Menu_mc.SystemFader.Page_mc.iSaveDelayTimerID` - Internal delay ID after the save/load is confirmed 
    - Functions:
        - `clearInterval` - Used by save/load to reset the timer
        - `_root.QuestJournalFader.Menu_mc.SystemFader.Page_mc.DoSaveGame` - Used by the save/load hooks
        - `_global.skse.StartRemapMode` - Related to key remapping in MCM menus
        - `_global.skse.EndRemapMode` - Related to key remapping in MCM menus
- MagicMenu
    - Variables:
        - `_root.inventoryLists.categoryList._entryClipManager._clipPool.listEnumeration` - used by active effect updates
        - `_root.inventoryLists.categoryList._entryClipManager._clipPool[].enabled` - used by active effect updates
        - `_root.inventoryLists.categoryList._entryClipManager._clipPool[]._alpha` - used by active effect updates
        - `_root.inventoryLists.categoryList._entryClipManager._clipPool.listEnumeration[].filterFlag` - used by active effect updates
    - Functions:
        - `_root.inventoryLists.categoryList._entryClipManager._clipPool.listEnumeration.at` - used by active effect updates
- MapMenu
    - Variables:
        - `_root.bottomBar.DateText` - Time/Date text instance. Modified every frame
- SleepWaitMenu
    - Functions:
        - `_root.SleepWaitMenu_mc.SetCurrentTime` - Time/Date setter function. Called every frame
- TweenMenu
    - Variables:
        - `_root.TweenMenu_mc.BottomBarTweener_mc.BottomBar_mc.DateText` - Time/Date text instance. Modified every frame

**Note, this list is for informative purposes only and may not be 100% correct or up-to-date in all cases. It may also change in the future. Please check the actual implementation in case of any doubts.**  
If you create a HUD/GUI mod that makes breaking changes to these paths please contact me and I will do my best to implement support for it.

### Native code changes (for DLL developers)

The mod hooks all affected menu Creator functions and re-registers its custom creators. As the original function is still called, you usually don't have to care about this too much, but if you take a reference to such a function, you should do so **after** the `kDataLoaded` SKSE callback has been executed.

The mod also detours various registered `FxDelegate` callbacks. The underlying functions are generally not modified, but they are no longer executed on the GUI thread! This means that these functions are now latent and will be executed in the main thread at a later time. This is necessary as these functions were never designed to be ran while the game is also running on an another thread, thus not doing so would cause crashes and race conditions.

The mod rewrites these functions to a thread-safe implemenattion every time its owner menu opens, immediately after the Creator function is called.

In case of registering or hooking custom menu callbacks (via FxDelegate) the registered functions must also be thread safe!

The mod will not take ownership of functions it doesn't know of. It will also not work with custom menus it doesn't know of.

If you're creating a custom menu, you may contact me in order to add support for it.

## Build

To build this mod refer to my wrapper project [here](https://github.com/Vermunds/SkyrimSE-Mods).

## License
This software is available under the MIT License. See LICENSE.md for details.
