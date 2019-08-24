#pragma once

namespace Offsets {
	const RelocAddr<uintptr_t*> FavoritesMenu_Hook = 0x8781D5; //1.5.80
	const RelocAddr<uintptr_t*> BookMenu_Hook = 0x85764E; //1.5.80
	const RelocAddr<uintptr_t*> LockpickingMenu_Hook = 0x8963E0; //1.5.80
	const RelocAddr<uintptr_t*> TweenMenu_Hook = 0x8D0C85; //1.5.80
	const RelocAddr<uintptr_t*> MessageBoxMenu_ServeTime_Hook = 0x21A915; //1.5.80
	const RelocAddr<uintptr_t*> StartSleepWait_Original = 0x8BDD60; //1.5.80

	const RelocAddr<uintptr_t*> IsInMenuMode_Hook = 0x9B8750; //1.5.80
	const RelocAddr<uintptr_t*> IsInMenuMode_Original = 0x2F26B75; //1.5.80

	const RelocAddr<uintptr_t*> LockpickingMenu_Target = 0x2F4E240; //1.5.80
	const RelocAddr<uintptr_t*> ContainerMenu_Mode = 0x2F4C328; //1.5.80
	const RelocAddr<uintptr_t*> ContainerMenu_Target = 0x2F4C3F0; //ContainerMenu_Mode + 0xC8 //1.5.80
	const RelocAddr<uintptr_t*> DrawNextFrame_Hook = 0xECD489; //1.5.80

	const RelocAddr<uintptr_t*> BookMenu_Target = 0x2F4BF78; //1.5.80

	const RelocAddr<uintptr_t*> SleepWaitMenu_ProcessMessage_Hook = 0x16BB530; //1.5.80
	const RelocAddr<uintptr_t*> SleepWaitMenu_ProcessMessage_Original = 0x8BD8C0; //1.5.80

	const RelocAddr<uintptr_t*> MenuControls_ReceiveEvent_Hook = 0x16B8320; //1.5.80
	const RelocAddr<uintptr_t*> MenuControls_ReceiveEvent_Original = 0x8A7CA0; //1.5.80

	const RelocAddr<uintptr_t*> MenuOpenHandler_CanProcess_Hook = 0x16B8498; //1.5.80
	const RelocAddr<uintptr_t*> MenuOpenHandler_CanProcess_Original = 0x8AACF0; //1.5.80

	const RelocAddr<uintptr_t*> ContainerMenu_TransferItem_Original = 0x85E150; //1.5.80
	const RelocAddr<uintptr_t*> ContainerMenu_TransferItemEquip_Original = 0x85E2A0; //1.5.80
	const RelocAddr<uintptr_t*> Unk_UpdateInventory_Call = 0x8601AB; //1.5.80

	const RelocAddr<uintptr_t*> InventoryMenu_DropItem_Hook = 0x88E356; //1.5.80

	const RelocAddr<uintptr_t*> FxDelegate_Callback_Hook = 0x17BAED0; //1.5.80
	const RelocAddr<uintptr_t*> FxDelegate_Callback_Original = 0xED6720; //1.5.80

	const RelocAddr<uintptr_t*> JournalMenu_SaveGame_Hook = 0x8D9BFB; //1.5.80
	const RelocAddr<uintptr_t*> JournalMenu_SaveGame_Overwrite_Hook = 0x5794FE; //1.5.80

	const RelocAddr<uintptr_t*> Console_ExecuteCommand_Original = 0x85A760; //1.5.80

	const RelocAddr<uintptr_t*> SaveGame_Original = 0x586DE0; //1.5.80

	//PlayerControls
	const RelocAddr<uintptr_t*> MovementHandler_ProcessButton_Hook = 0x166F1D8; //1.5.80

	//MenuControls
	const RelocAddr<uintptr_t*> DirectionHandler_CanProcess_Hook = 0x16B8428; //1.5.80
	const RelocAddr<uintptr_t*> DirectionHandler_CanProcess_Original = 0x8AABD0; //1.5.80

	const RelocAddr<uintptr_t*> GlobalTimescaleMultipler = 0x1E05B2C; //1.5.80
	const RelocAddr<uintptr_t*> GlobalTimescaleMultipler_Havok = 0x1E05B28; //1.5.80

	const RelocAddr<uintptr_t*> RequestScreenshot_Original = 0x5C0960; //1.5.80

	const RelocAddr<uintptr_t*> CreateScreenshot_Original = 0x5C09F0; //1.5.80

	const RelocAddr<uintptr_t*> MessageBoxButtonPress_Original = 0x8AB9E0; //1.5.80

}
