#pragma once

// jota2rz-fork: prevent Steam overlay from pausing the game by intercepting
// Skyrim's Steamworks callback registration.
//
// Mechanism: when the Steam overlay opens, `gameoverlayrenderer64.dll`
// causes `SteamAPI_RunCallbacks` to dispatch a `GameOverlayActivated_t`
// (Steamworks callback ID 331 = k_iSteamFriendsCallbacks + 31). Skyrim
// registers a listener for that callback in its game-init code, and the
// listener pauses the world simulation.
//
// This module IAT-patches SkyrimSE.exe's import for
// `SteamAPI_RegisterCallback` (from steam_api64.dll). When Skyrim tries to
// register a callback with ID 331, we silently skip the registration --
// Steam never notifies Skyrim about the overlay, so no pause fires.
// All other callbacks pass through untouched.
//
// This approach:
// - Doesn't require any RE:: offset guessing (immune to Skyrim patches)
// - Version-agnostic (Steamworks SDK ABI is stable)
// - Self-contained (no CommonLibSSE hook infrastructure)
// - Reversible (we could restore the original IAT entry if needed)
//
// See SteamOverlayFix.cpp for the implementation.

namespace SkyrimSoulsRE::SteamOverlayFix
{
	// Install the IAT hook. Safe to call once at plugin load time.
	// Requires that `steam_api64.dll` has been loaded by Skyrim (it always
	// is: statically imported by SkyrimSE.exe).
	void Install();
}
