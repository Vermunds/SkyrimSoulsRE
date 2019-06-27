#pragma once

namespace Offsets {
	RelocAddr <uintptr_t *> FavoritesMenu_Hook = 0x8781D5; //1.5.80
	RelocAddr <uintptr_t *> BookMenu_Hook = 0x85764E; //1.5.80
	RelocAddr <uintptr_t *> LockpickingMenu_Hook = 0x8963E0; //1.5.80
	RelocAddr <uintptr_t *> TweenMenu_Hook = 0x8D0C85; //1.5.80
	RelocAddr <uintptr_t *> Papyrus_Hook = 0x9B8750; //1.5.80
	RelocAddr <uintptr_t *> IsInMenuMode_Hook = 0x2F26B75; //1.5.80
	RelocAddr <uintptr_t *> JournalMenu_Hook = 0x586DE0; //1.5.80
	RelocAddr <uintptr_t *> SleepWaitMenuControls_Hook = 0x5B35B1; //1.5.80
	RelocAddr <uintptr_t *> SleepWaitMenuStart_Hook = 0x8BDD60; //1.5.80
	RelocAddr <uintptr_t *> EventDelayer_Hook = 0xC15E00; //1.5.80 //TODO: Find a better hook point for this
}
