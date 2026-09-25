# Skyrim Souls RE
[![Nexus Mods](https://img.shields.io/badge/NexusMods-Download-orange)](https://www.nexusmods.com/skyrimspecialedition/mods/27859)
[![GitHub release](https://img.shields.io/github/v/release/Vermunds/SkyrimSoulsRE)](https://github.com/Vermunds/SkyrimSoulsRE/releases)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](./LICENSE)

A mod for The Elder Scrolls V: Skyrim - Special Edition.

This mod unpauses all of Skyrim's in-game menus.

## Features
- Unpauses all of Skyrim’s game menus, individually configurable.
- Optional movement and camera-controls while menus are open.
- Optional slow-motion effect while menus are open.
- Fully integrated into game world with custom auto-close logic for relevant menus.
- Combat alert overlay for warning the user for dangers.
- Optionally disable background blur of menus.
- Optionally also unpause only if not in combat.
- Fully customizable in-game or through an included .ini file.
- Fixed crashes and race conditions from earlier versions of similar mods.

## Download
Available on [Nexusmods](https://www.nexusmods.com/skyrimspecialedition/mods/27859) or via Github Releases.

## Configuration
The settings can be changed in-game, in the settings menu of either SKSE Menu Framework or Fuzz's Legally Intelligible Core Kit, or in `Data/SKSE/Plugins/SkyrimSoulsRE.ini`. Changes made in the menu are saved to the same file.

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
For reference, the following menus ARE affected:
- `BarterMenu`
- `BookMenu`
- `Console`
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
- `QuestMenu` - from Quest Journal Overhaul
- `BestiaryMenu` - from The Dragonborn's Bestiary

### Native code changes (for DLL developers)

The mod hooks all affected menu Creator functions and re-registers its custom creators. As the original function is still called, you usually don't have to care about this too much, but if you take a reference to such a function, you should do so **after** the `kDataLoaded` SKSE callback has been executed.

The mod will not work with custom menus it doesn't know of.

If you're creating a custom menu, you may contact me in order to add support for it, or you can also consider opening a pull request.

## Build

To build this mod refer to my wrapper project [here](https://github.com/Vermunds/SkyrimSE-Mods).

## License
This software is available under the GNU General Public License v3.0 or later, with a modding exception. See [LICENSE](./LICENSE) and [EXCEPTIONS.md](./EXCEPTIONS.md) for details.
