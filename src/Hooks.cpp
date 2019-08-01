#include "Hooks.h"

#include "skse64_common/BranchTrampoline.h"  // g_trampoline
#include "skse64_common/Relocation.h"  // RelocAddr, RelocPtr
#include "skse64_common/SafeWrite.h"  // SafeWrite

#include "skse64/GameData.h" //BGSSaveLoadManager

#include "xbyak/xbyak.h"

#include "HookShare.h"  // _RegisterHook_t

#include "RE/Skyrim.h"

#include "Events.h"  // MenuOpenCloseEventHandler::BlockInput()
#include "Offsets.h"
#include "Settings.h" //unpausedMenus
#include "Tasks.h" //SleepWaitDelegate, SaveGameDelegate, ServeTimeDelegate
#include "Utility.h" //strToInt

#include <thread> //std::this_thread::sleep_for
#include <chrono> //std::chrono::seconds

namespace Hooks
{

	class FavoritesMenuEx
	{
	public:
		static void InstallHook()
		{
			//Fix for hotkeys
			SafeWrite16(Offsets::FavoritesMenu_Hook.GetUIntPtr(), 0x9090);
		}
	};

	class BookMenuEx
	{
	public:
		static void InstallHook()
		{
			//Fix for book menu not appearing
			SafeWrite16(Offsets::BookMenu_Hook.GetUIntPtr(), 0x9090);
		}
	};

	class LockpickingMenuEx
	{
	public:

		static RE::TESObjectREFR* GetLockpickingTarget()
		{
			RE::TESObjectREFRPtr* refptr = reinterpret_cast<RE::TESObjectREFRPtr*>(Offsets::LockpickingMenu_Target.GetUIntPtr());
			return refptr->get();
		}

		static void InstallHook()
		{
			//Fix for lockpicking menu not appearing
			SafeWrite16(Offsets::LockpickingMenu_Hook.GetUIntPtr(), 0x9090);
		}
	};

	class InventoryMenuEx
	{
	public:
		static void DropItem_Hook(RE::Actor* a_thisActor, RE::RefHandle& a_droppedItemHandle, RE::TESForm* a_item, RE::BaseExtraList* a_extraList, UInt32 a_count, void* a_arg5, void* a_arg6)
		{
			//Looks like some other thread moves it before the calculation is complete, resulting in the item being moved to the coc marker
			//This is an ugly workaround, but it should work good enought

			RE::MenuManager* mm = RE::MenuManager::GetSingleton();
			mm->numPauseGame++;
			std::this_thread::sleep_for(std::chrono::milliseconds(10));

			//arg5 and arg6 seems to be an UInt64 (a pointer maybe?)
			a_thisActor->DropItem(a_droppedItemHandle, a_item, a_extraList, a_count, a_arg5, a_arg6);

			mm->numPauseGame--;
		}

		static void InstallHook()
		{
			g_branchTrampoline.Write6Call(Offsets::InventoryMenu_DropItem_Hook.GetUIntPtr(), uintptr_t(DropItem_Hook));
		}
	};


	class TweenMenuEx
	{
	public:
		static void InstallHook()
		{
			//Fix for camera movement
			UInt8 codes[] = { 0x90, 0x90, 0x90, 0x90, 0x90 };
			SafeWriteBuf(Offsets::TweenMenu_Hook.GetUIntPtr(), codes, sizeof(codes));
		}
	};

	class ContainerMenuEx
	{
	public:
		enum ContainerMode
		{
			kMode_Loot = 0,
			kMode_Steal = 1,
			kMode_Pickpocket = 2,
			kMode_FollowerTrade = 3
		};

		static ContainerMode GetContainerMode()
		{
			UInt8* mode = reinterpret_cast<UInt8*>(Offsets::ContainerMenu_Mode.GetUIntPtr());
			return static_cast<ContainerMode>(*mode);
		}

		static RE::TESObjectREFR* GetContainerRef()
		{

			UInt32* handle = reinterpret_cast<UInt32*>(Offsets::ContainerMenu_Target.GetUIntPtr());
			RE::TESObjectREFRPtr refptr = nullptr;
			if (RE::TESObjectREFR::LookupByHandle(*handle, refptr))
			{
				RE::TESObjectREFR* ref = refptr.get();
				return ref;
			};
			return nullptr;
		}

		static void UpdateEquipment_Hook(void* a_unk, RE::PlayerCharacter* a_player)
		{
			RE::MenuManager* mm = RE::MenuManager::GetSingleton();
			RE::UIStringHolder* strHolder = RE::UIStringHolder::GetSingleton();
			if (mm->IsMenuOpen(strHolder->containerMenu))
			{
				RE::TESObjectREFR* target = GetContainerRef();

				if (target)
				{
					ContainerMode mode = GetContainerMode();
					if (target->Is(RE::FormType::ActorCharacter) && mode == kMode_FollowerTrade)
					{
						RE::Actor* containerOwner = reinterpret_cast<RE::Actor*>(target);
						Tasks::UpdateInventoryDelegate::RegisterTask(a_unk, containerOwner);
						return;
					}
				}
			}

			void(*UpdateInventory_Original)(void*, RE::Actor*);
			UpdateInventory_Original = reinterpret_cast<void(*)(void*, RE::Actor*)>(RE::Offset::InventoryMenu::InventoryData::Update);
			return UpdateInventory_Original(a_unk, a_player);
		}

		static void InstallHook()
		{
			////Fix for trading with followers
			g_branchTrampoline.Write5Branch(Offsets::ContainerMenu_TransferItem_Original.GetUIntPtr() + 0x105, (uintptr_t)UpdateEquipment_Hook);

			//Fix for equipping directly from follower inventory
			g_branchTrampoline.Write5Call(Offsets::ContainerMenu_TransferItemEquip_Original.GetUIntPtr() + 0x14A, (uintptr_t)UpdateEquipment_Hook);

			//Another call here
			g_branchTrampoline.Write5Call(Offsets::Unk_UpdateInventory_Call.GetUIntPtr(), (uintptr_t)UpdateEquipment_Hook);
		}
	};

	class SleepWaitMenuEx
	{
	public:

		static RE::IMenu::Result ProcessMessage(RE::IMenu* a_thisMenu, RE::UIMessage* a_message)
		{
			if (a_message->message == RE::UIMessage::Message::kScaleform)
			{
				return a_thisMenu->RE::IMenu::ProcessMessage(a_message);
			}
			RE::IMenu::Result(*ProcessMessage_Original)(RE::IMenu * a_thisMenu, RE::UIMessage * msg);
			ProcessMessage_Original = reinterpret_cast<RE::IMenu::Result(*)(RE::IMenu*, RE::UIMessage*)>(Offsets::SleepWaitMenu_ProcessMessage_Original.GetUIntPtr());
			return ProcessMessage_Original(a_thisMenu, a_message);
		}

		static void StartSleepWait(const RE::FxDelegateArgs& a_args) {
			RE::MenuManager* mm = RE::MenuManager::GetSingleton();
			RE::UIStringHolder* strHolder = RE::UIStringHolder::GetSingleton();

			mm->GetMenu(strHolder->sleepWaitMenu)->flags |= RE::IMenu::Flag::kPauseGame;
			mm->numPauseGame++;

			Tasks::SleepWaitDelegate::RegisterTask(a_args);
		}

		static void InstallHook()
		{
			//fix for controls not working
			SafeWrite64(Offsets::SleepWaitMenu_ProcessMessage_Hook.GetUIntPtr(), (uintptr_t)ProcessMessage);

		}
	};

	class MessageBoxMenuEx
	{
	public:
		static void RegisterForServeTime()
		{
			Tasks::ServeTimeDelegate::RegisterTask();
		}

		static void InstallHook() {
			//Fix for serving time in jail
			g_branchTrampoline.Write5Branch(Offsets::MessageBoxMenu_ServeTime_Hook.GetUIntPtr(), (uintptr_t)RegisterForServeTime);
		}
	};

	class JournalMenuEx
	{
	public:

		static bool SaveGame_Hook(RE::BGSSaveLoadManager* a_this, BGSSaveLoadManagerEx::SaveMode a_mode, BGSSaveLoadManagerEx::DumpFlag a_dumpFlag, const char* a_saveName)
		{
			Tasks::SaveGameDelegate::RegisterTask(a_dumpFlag, a_saveName);
			return true;
		}

		static void InstallHook()
		{
			g_branchTrampoline.Write5Branch(Offsets::JournalMenu_SaveGame_Hook.GetUIntPtr(), (uintptr_t)SaveGame_Hook);
			g_branchTrampoline.Write5Call(Offsets::JournalMenu_SaveGame_Overwrite_Hook.GetUIntPtr(), (uintptr_t)SaveGame_Hook);
			g_branchTrampoline.Write5Call(Offsets::JournalMenu_SaveGame_Overwrite_Hook.GetUIntPtr() + 0x22, (uintptr_t)SaveGame_Hook);
		}
	};

	class ConsoleEx
	{
	public:

		class CommandData
		{
		public:
			std::string command;
			std::vector<std::string> arguments;
			UInt32 numArgs;
		};

		static CommandData* ParseCommand(std::string a_fullCommand)
		{
			CommandData * data = new CommandData();

			bool argumentExpected = false;

			for (auto ch : a_fullCommand) {
				if (ch == ' ') {
					if (!(data->command.empty()))
					{
						argumentExpected = true;
					}
				} else {

					if (argumentExpected)
					{
						argumentExpected = false;
						data->numArgs++;
						data->arguments.push_back(std::string());
					}

					if (data->numArgs != 0) {
						std::tolower(ch);
						data->arguments[data->numArgs - 1].push_back(ch);
					} else {
						std::tolower(ch);
						data->command.push_back(ch);
					}
				}
			}
			return data;
		}

		static void ExecuteCommand_Hook(const RE::FxDelegateArgs& a_args)
		{
			const std::string commandStr = a_args[0].GetString();

			CommandData* data = ParseCommand(commandStr);

			if (data->command == "save" || data->command == "savegame")
			{

				BGSSaveLoadManagerEx::DumpFlag flag = BGSSaveLoadManagerEx::DumpFlag::kNone;

				if (data->numArgs == 1)
				{
					Tasks::SaveGameDelegate::RegisterTask(flag, data->arguments[0].c_str());
					return;
				}
				else if (data->numArgs > 1 && 5 >= data->numArgs)
				{
					for (int i = 1; i < data->numArgs; i++)
					{
						std::string arg = data->arguments[i];
						SInt32 num = 0;

						if (SkyrimSoulsRE::StrToInt(num, arg.c_str()))
						{
							flag |= (BGSSaveLoadManagerEx::DumpFlag)(num << (i - 1));
						}
						else
						{
							return;
						}
					}
					Tasks::SaveGameDelegate::RegisterTask(flag, data->arguments[0].c_str());
					return;
				}

			}
			else if (data->command == "servetime" && data->numArgs == 0)
			{
				Tasks::ServeTimeDelegate::RegisterTask();
				return;
			}

			void(*ExecuteCommand_Original)(const RE::FxDelegateArgs&);
			ExecuteCommand_Original = reinterpret_cast<void(*)(const RE::FxDelegateArgs&)>(Offsets::Console_ExecuteCommand_Original.GetUIntPtr());
			return ExecuteCommand_Original(a_args);
		}
	};

	class SlowTimeHook
	{
	public:
		static void InstallHook()
		{
			//Fix for book menu and sleep wait menu slowing down
			//This will also make all havok physics normal speed, but I can't do anyting about it
			UInt8 codes[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
			SafeWriteBuf(Offsets::GlobalTimescale_Hook.GetUIntPtr(), codes, sizeof(codes));
			SafeWriteBuf(Offsets::GlobalTimescale_Hook.GetUIntPtr() + 0xA, codes, sizeof(codes));
			SafeWriteBuf(Offsets::GlobalTimescale_Hook.GetUIntPtr() + 0x2C, codes, sizeof(codes));
			SafeWriteBuf(Offsets::GlobalTimescale_Hook.GetUIntPtr() + 0x36, codes, sizeof(codes));
		}
	};

	class PapyrusEx
	{
	public:
		static bool* isInMenuMode_1;
		static bool* isInMenuMode_2;

		static bool IsInMenuMode()
		{
			if (*isInMenuMode_1 || *isInMenuMode_2 || SkyrimSoulsRE::unpausedMenuCount) {
				return true;
			}
			return false;
		}

		static void InstallHook() {
			//IsInMenuMode hook
			isInMenuMode_1 = reinterpret_cast<bool*>(Offsets::IsInMenuMode_Original.GetUIntPtr());
			isInMenuMode_2 = reinterpret_cast<bool*>(Offsets::IsInMenuMode_Original.GetUIntPtr() + 0x1);
			g_branchTrampoline.Write5Branch(Offsets::IsInMenuMode_Hook.GetUIntPtr(), (uintptr_t)IsInMenuMode);
			SafeWrite16(Offsets::IsInMenuMode_Hook.GetUIntPtr() + 0x5, 0x9090);
		}
	};
	bool* PapyrusEx::isInMenuMode_1 = nullptr;
	bool* PapyrusEx::isInMenuMode_2 = nullptr;

	//PlayerControls
	class MovementHandlerEx
	{
	public:
		static void ProcessButton_Hook(RE::MovementHandler* a_this, RE::ButtonEvent* a_event, RE::MovementData* a_data)
		{
			RE::InputStringHolder* inStr = RE::InputStringHolder::GetSingleton();
			RE::MenuManager* mm = RE::MenuManager::GetSingleton();
			RE::UIStringHolder* strHolder = RE::UIStringHolder::GetSingleton();

			if (a_event && a_event->deviceType == RE::DeviceType::kKeyboard)
			{
				if (a_event->GetControlID() == inStr->forward)
				{
					a_data->autoRun = 0;
					a_data->movementY = 1.0;
				}
				else if (a_event->GetControlID() == inStr->back)
				{
					a_data->autoRun = 0;
					a_data->movementY = -1.0;
				}
				else if (a_event->GetControlID() == inStr->strafeLeft)
				{
					a_data->movementX = -1.0;
				}
				else if (a_event->GetControlID() == inStr->strafeRight)
				{
					a_data->movementX = 1.0;
				}

				if (SkyrimSoulsRE::unpausedMenuCount && !(mm->IsMenuOpen(strHolder->dialogueMenu)))
				{
					if (a_event->GetControlID() == inStr->up)
					{
						a_data->autoRun = 0;
						a_data->movementY = 1.0;
					}
					else if (a_event->GetControlID() == inStr->down)
					{
						a_data->autoRun = 0;
						a_data->movementY = -1.0;
					}
					else if (a_event->GetControlID() == inStr->left)
					{
						a_data->movementX = -1.0;
					}
					else if (a_event->GetControlID() == inStr->right)
					{
						a_data->movementX = 1.0;
					}
				}
			}
		}

		static void InstallHook()
		{
			SafeWrite64(Offsets::MovementHandler_ProcessButton_Hook.GetUIntPtr(), (uintptr_t)ProcessButton_Hook);
		}
	};

	class MenuControlsEx
	{
	public:

		//Fix for SkyUI favorites menu
		static RE::EventResult ReceiveEvent_Hook(RE::MenuControls* a_this, RE::InputEvent** a_event, RE::BSTEventSource<RE::InputEvent*>* a_source)
		{
			RE::MenuManager* mm = RE::MenuManager::GetSingleton();
			RE::UIStringHolder* strHolder = RE::UIStringHolder::GetSingleton();
			RE::InputStringHolder* inStr = RE::InputStringHolder::GetSingleton();

			RE::InputEvent* nullEvent = nullptr;
			RE::InputEvent** events = &nullEvent;

			if (SkyrimSoulsRE::unpausedMenuCount && mm->IsMenuOpen(strHolder->favoritesMenu))
			{
				if (a_event && *a_event)
				{
					UInt32 index = 0;
					for (RE::InputEvent* evn = *a_event; evn; evn = evn->next)
					{
						if (evn && evn->deviceType == RE::DeviceType::kKeyboard)
						{
							if (evn->GetControlID() == inStr->strafeLeft || evn->GetControlID() == inStr->strafeRight)
							{
								continue;
							}
						}
						events[index] = evn;
						if (index != 0)
						{
							events[index - 1]->next = evn;
						}
						index++;
					}
				}
			}
			else {
				events = a_event;
			}

			RE::EventResult(*ReceiveEvent_Original)(RE::MenuControls * a_this, RE::InputEvent * *a_event, RE::BSTEventSource<RE::InputEvent*> * a_source);
			ReceiveEvent_Original = reinterpret_cast<RE::EventResult(*)(RE::MenuControls * a_this, RE::InputEvent * *a_event, RE::BSTEventSource<RE::InputEvent*> * a_source)>(Offsets::MenuControls_ReceiveEvent_Original.GetUIntPtr());;
			return ReceiveEvent_Original(a_this, events, a_source);
		}

		static void InstallHook()
		{
			SafeWrite64(Offsets::MenuControls_ReceiveEvent_Hook.GetUIntPtr(), (uintptr_t)ReceiveEvent_Hook);
		}
	};

	//MenuControls
	class DirectionHandlerEx
	{
	public:
		static bool CanProcess(RE::MenuEventHandler* a_this, RE::InputEvent* a_event)
		{
			RE::MenuManager* mm = RE::MenuManager::GetSingleton();

			if (SkyrimSoulsRE::unpausedMenuCount && !(mm->GameIsPaused()))
			{
				return false;
			}
			bool(*CanProcess_Original)(RE::MenuEventHandler*, RE::InputEvent*);
			CanProcess_Original = reinterpret_cast<bool(*)(RE::MenuEventHandler*, RE::InputEvent*)>(Offsets::DirectionHandler_CanProcess_Original.GetUIntPtr());;
			return CanProcess_Original(a_this, a_event);
		}

		static void InstallHook()
		{
			SafeWrite64(Offsets::DirectionHandler_CanProcess_Hook.GetUIntPtr(), (uintptr_t)CanProcess);
		}
	};

	//MenuControls
	class MenuOpenHandlerEx
	{
	public:
		static bool CanProcess(RE::MenuOpenHandler* a_this, RE::InputEvent* a_event)
		{
			if (SkyrimSoulsRE::unpausedMenuCount)
			{
				return false;
			}
			bool(*CanProcess_Original)(RE::MenuOpenHandler*, RE::InputEvent*);
			CanProcess_Original = reinterpret_cast<bool(*)(RE::MenuOpenHandler*, RE::InputEvent*)>(Offsets::MenuOpenHandler_CanProcess_Original.GetUIntPtr());;
			return CanProcess_Original(a_this, a_event);
		}

		static void InstallHook()
		{
			SafeWrite64(Offsets::MenuOpenHandler_CanProcess_Hook.GetUIntPtr(), (uintptr_t)CanProcess);
		}
	};

	class AutoCloseHandler
	{
	public:
		//Initial position of player and the container
		static bool containerTooFarWhenOpened;
		static bool lockpickingTooFarWhenOpened;
		static float containerInitialDistance;
		static float lockpickingInitialDistance;

		inline static float GetDistance(RE::NiPoint3 a_playerPos, RE::NiPoint3 a_refPos)
		{
			//Get distance from feet and head, return the smaller
			float distanceHead = sqrt(pow(a_playerPos.x - a_refPos.x, 2) + pow(a_playerPos.y - a_refPos.y, 2) + pow((a_playerPos.z + 150) - a_refPos.z, 2));
			float distanceFeet = sqrt(pow(a_playerPos.x - a_refPos.x, 2) + pow(a_playerPos.y - a_refPos.y, 2) + pow(a_playerPos.z - a_refPos.z, 2));
			if (distanceHead < distanceFeet) {
				return distanceHead;
			}
			return distanceFeet;
		}

		static void CheckShouldClose()
		{
			RE::MenuManager * mm = RE::MenuManager::GetSingleton();
			RE::UIStringHolder * strHolder = RE::UIStringHolder::GetSingleton();
			RE::PlayerCharacter * player = RE::PlayerCharacter::GetSingleton();
			RE::UIManager * uiManager = RE::UIManager::GetSingleton();
			SkyrimSoulsRE::SettingStore * settings = SkyrimSoulsRE::SettingStore::GetSingleton();

			if (mm->IsMenuOpen(strHolder->containerMenu) && settings->GetSetting("containerMenu"))
			{
				RE::TESObjectREFR * ref = ContainerMenuEx::GetContainerRef();

				if (ref) {
					if (ref->IsDisabled() || ref->IsMarkedForDeletion())
					{
						uiManager->AddMessage(strHolder->containerMenu, RE::UIMessage::Message::kClose, 0);
					}

					if (ref->Is(RE::FormType::ActorCharacter))
					{
						if (ContainerMenuEx::GetContainerMode() != ContainerMenuEx::ContainerMode::kMode_Loot && ref->IsDead(true))
						{
							uiManager->AddMessage(strHolder->containerMenu, RE::UIMessage::Message::kClose, 0);
						}
					}
					if (settings->GetSetting("autoClose")) {
						float maxDistance = static_cast<float>(settings->GetSetting("autoCloseDistance"));
						float currentDistance = GetDistance(player->pos, ref->pos);

						if (SkyrimSoulsRE::justOpenedContainer)
						{
							containerInitialDistance = currentDistance;
							SkyrimSoulsRE::justOpenedContainer = false;

							containerTooFarWhenOpened = (containerInitialDistance > maxDistance) ? true : false;
						}

						if (containerTooFarWhenOpened)
						{
							//Check if the distance is increasing
							if (currentDistance > (containerInitialDistance + 50))
							{
								uiManager->AddMessage(strHolder->containerMenu, RE::UIMessage::Message::kClose, 0);
							}
							else if ((containerInitialDistance - 50) > currentDistance) {
								//Check if it's already in range
								if (currentDistance < maxDistance)
								{
									containerTooFarWhenOpened = false;
								}
							}
						}
						else
						{
							if (currentDistance > maxDistance)
							{
								uiManager->AddMessage(strHolder->containerMenu, RE::UIMessage::Message::kClose, 0);
							}
						}
					}
				}
			}

			if (mm->IsMenuOpen(strHolder->lockpickingMenu) && settings->GetSetting("lockpickingMenu"))
			{
				RE::TESObjectREFR * ref = LockpickingMenuEx::GetLockpickingTarget();
				if (ref)
				{
					if (ref->IsDisabled() || ref->IsMarkedForDeletion())
					{
						uiManager->AddMessage(strHolder->lockpickingMenu, RE::UIMessage::Message::kClose, 0);
					}

					if (settings->GetSetting("autoClose")) {
						float maxDistance = static_cast<float>(settings->GetSetting("autoCloseDistance"));
						float currentDistance = GetDistance(player->pos, ref->pos);

						if (SkyrimSoulsRE::justOpenedLockpicking)
						{
							lockpickingInitialDistance = currentDistance;
							SkyrimSoulsRE::justOpenedLockpicking = false;

							lockpickingTooFarWhenOpened = (lockpickingInitialDistance > maxDistance) ? true : false;
						}

						if (lockpickingTooFarWhenOpened)
						{
							//Check if the distance is increasing
							if (currentDistance > (lockpickingInitialDistance + 50))
							{
								uiManager->AddMessage(strHolder->lockpickingMenu, RE::UIMessage::Message::kClose, 0);
							}
							else if ((lockpickingInitialDistance - 50) > currentDistance) {
								//Check if it's already in range
								if (currentDistance < maxDistance)
								{
									lockpickingTooFarWhenOpened = false;
								}
							}
						}
						else
						{
							if (currentDistance > maxDistance)
							{
								uiManager->AddMessage(strHolder->lockpickingMenu, RE::UIMessage::Message::kClose, 0);
							}
						}
					}
				}
			}
		}

		static void InstallHook()
		{
			struct DrawNextFrame_Code : Xbyak::CodeGenerator
			{
				DrawNextFrame_Code(void * buf, UInt64 a_checkShouldClose) : Xbyak::CodeGenerator(4096, buf)
				{
					Xbyak::Label checkShouldCloseAddress;

					call(ptr[rip + checkShouldCloseAddress]);
					add(rsp, 0x50);
					pop(rbx);
					ret();

					L(checkShouldCloseAddress);
					dq(a_checkShouldClose);
				}
			};

			void * codeBuf = g_localTrampoline.StartAlloc();
			DrawNextFrame_Code code(codeBuf, uintptr_t(CheckShouldClose));
			g_localTrampoline.EndAlloc(code.getCurr());

			g_branchTrampoline.Write5Branch(Offsets::DrawNextFrame_Hook.GetUIntPtr(), uintptr_t(code.getCode()));
		}
	};
	bool AutoCloseHandler::containerTooFarWhenOpened = false;
	bool AutoCloseHandler::lockpickingTooFarWhenOpened = false;
	float AutoCloseHandler::containerInitialDistance = 0.0;
	float AutoCloseHandler::lockpickingInitialDistance = 0.0;

	void Register_Func(RE::FxDelegate* a_delegate, HookType a_type)
	{
		switch(a_type)
		{
		case kSleepWaitMenu:
			a_delegate->callbacks.GetAlt("OK")->callback = SleepWaitMenuEx::StartSleepWait;
			break;
		case kConsole:
			a_delegate->callbacks.GetAlt("ExecuteCommand")->callback = ConsoleEx::ExecuteCommand_Hook;
			break;
		}
	}

	HookShare::result_type _PlayerInputHandler_CanProcess(RE::PlayerInputHandler* a_this, RE::InputEvent* a_event)
	{
		using SkyrimSoulsRE::MenuOpenCloseEventHandler;
		using HookShare::result_type;

		if (MenuOpenCloseEventHandler::BlockInput()) {
			return result_type::kFalse;
		}
		else {
			return result_type::kContinue;
		}
	}

	HookShare::result_type _PlayerInputHandler_CanProcess_Movement(RE::PlayerInputHandler* a_this, RE::InputEvent* a_event)
	{
		using SkyrimSoulsRE::MenuOpenCloseEventHandler;
		using HookShare::result_type;

		return result_type::kTrue;
	}

	void InstallHooks(HookShare::RegisterForCanProcess_t* a_register)
	{
		using HookShare::Hook;

		SkyrimSoulsRE::SettingStore* settings = SkyrimSoulsRE::SettingStore::GetSingleton();

		a_register(Hook::kFirstPersonState, _PlayerInputHandler_CanProcess);
		a_register(Hook::kThirdPersonState, _PlayerInputHandler_CanProcess);
		a_register(Hook::kFavorites, _PlayerInputHandler_CanProcess);
		a_register(Hook::kLook, _PlayerInputHandler_CanProcess);
		a_register(Hook::kSprint, _PlayerInputHandler_CanProcess);
		a_register(Hook::kReadyWeapon, _PlayerInputHandler_CanProcess);
		a_register(Hook::kAutoMove, _PlayerInputHandler_CanProcess);
		a_register(Hook::kToggleRun, _PlayerInputHandler_CanProcess);
		a_register(Hook::kActivate, _PlayerInputHandler_CanProcess);
		a_register(Hook::kJump, _PlayerInputHandler_CanProcess);
		a_register(Hook::kShout, _PlayerInputHandler_CanProcess);
		a_register(Hook::kAttackBlock, _PlayerInputHandler_CanProcess);
		a_register(Hook::kRun, _PlayerInputHandler_CanProcess);
		a_register(Hook::kSneak, _PlayerInputHandler_CanProcess);

		if (settings->GetSetting("bEnableMovementInMenus"))
		{
			a_register(Hook::kMovement, _PlayerInputHandler_CanProcess_Movement);
		}
		else {
			a_register(Hook::kMovement, _PlayerInputHandler_CanProcess);
		}

		if (settings->GetSetting("tweenMenu")) {
			TweenMenuEx::InstallHook();
		}
		if (settings->GetSetting("favoritesMenu")) {
			FavoritesMenuEx::InstallHook();
		}
		if (settings->GetSetting("bookMenu")) {
			BookMenuEx::InstallHook();
		}
		if (settings->GetSetting("lockpickingMenu")) {
			LockpickingMenuEx::InstallHook();
		}
		if (settings->GetSetting("sleepWaitMenu")) {
			SleepWaitMenuEx::InstallHook();
		}
		if (settings->GetSetting("messageBoxMenu")) {
			MessageBoxMenuEx::InstallHook();
		}
		if (settings->GetSetting("containerMenu")) {
			ContainerMenuEx::InstallHook();
		}
		if (settings->GetSetting("inventoryMenu")) {
			InventoryMenuEx::InstallHook();
		}
		if (settings->GetSetting("journalMenu")) {
			JournalMenuEx::InstallHook();
		}

		AutoCloseHandler::InstallHook();
		PapyrusEx::InstallHook();
		MenuOpenHandlerEx::InstallHook();

		if (settings->GetSetting("bEnableMovementInMenus"))
		{
			MovementHandlerEx::InstallHook();
			MenuControlsEx::InstallHook();
			DirectionHandlerEx::InstallHook();
		}

		SlowTimeHook::InstallHook();
	}
}
