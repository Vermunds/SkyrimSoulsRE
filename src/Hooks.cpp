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

	class DialogueMenuEx
	{
	public:

		//Prevent dialogue from closing when an another menu is open
		static void UpdateAutoCloseTimer(uintptr_t a_unk, float a_delta)
		{
			uintptr_t unk = *(reinterpret_cast<uintptr_t*>(a_unk + 0x10));
			if (unk)
			{
				float* timer = reinterpret_cast<float*>(unk + 0x340);
				if (SkyrimSoulsRE::unpausedMenuCount)
				{
					*timer = 120.0;
				}
				else
				{
					*timer += a_delta; //a_delta is negative
				}
			}
		}

		static void InstallHook()
		{
			g_branchTrampoline.Write5Call(Offsets::DialogueMenu_UpdateAutoCloseTimer_Hook.GetUIntPtr(), (uintptr_t)UpdateAutoCloseTimer);
		}
	};

	class FavoritesMenuEx
	{
	public:
		static void InstallHook()
		{
			//Fix for hotkeys not working
			//FavoritesHandler
			SafeWrite16(Offsets::FavoritesMenu_Hook.GetUIntPtr(), 0x9090);
		}
	};

	class BookMenuEx
	{
	public:

		static RE::TESObjectREFR* GetBookReference()
		{
			RE::TESObjectREFRPtr* refptr = reinterpret_cast<RE::TESObjectREFRPtr*>(Offsets::BookMenu_Target.GetUIntPtr());
			return refptr->get();
		}

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

		static void UpdateEquipment_Hook(RE::ItemList* a_this, RE::PlayerCharacter* a_player)
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
						Tasks::UpdateInventoryDelegate::RegisterTask(a_this, containerOwner);
						return;
					}
				}
			}

			return a_this->Update(a_player);
		}

		static void InstallHook()
		{
			//Fix for trading with followers
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

		//Fix mouse controls in sleep/wait menu
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

		static void StartSleepWait_Hook(const RE::FxDelegateArgs& a_args) {
			RE::MenuManager* mm = RE::MenuManager::GetSingleton();
			RE::UIStringHolder* strHolder = RE::UIStringHolder::GetSingleton();
			SkyrimSoulsRE::SettingStore* settings = SkyrimSoulsRE::SettingStore::GetSingleton();

			mm->GetMenu(strHolder->sleepWaitMenu)->flags |= RE::IMenu::Flag::kPauseGame;
			mm->numPauseGame++;

			static bool enableSlowMotion = settings->GetSetting(settings->slowMotion, "bEnableSlowMotion")->GetBool();
			if (SkyrimSoulsRE::isInSlowMotion)
			{
				static UInt32 multiplierPercent = settings->GetSetting(settings->slowMotion, "uSlowMotionPercent")->GetInt();
				float* globalTimescale = reinterpret_cast<float*>(Offsets::GlobalTimescaleMultipler.GetUIntPtr());
				float* globalTimescaleHavok = reinterpret_cast<float*>(Offsets::GlobalTimescaleMultipler_Havok.GetUIntPtr());

				float multiplier;
				if (multiplierPercent >= 10 && 100 >= multiplierPercent)
				{
					multiplier = (float)multiplierPercent / 100.0;
				}
				else {
					multiplier = 1.0;
				}

				SkyrimSoulsRE::isInSlowMotion = false;
				*globalTimescale = (1.0 / multiplier) * (*globalTimescale);
				*globalTimescaleHavok = (1.0 / multiplier) * (*globalTimescaleHavok);
			}

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

		//FxDelegate
		static void ButtonPress_Hook(const RE::FxDelegateArgs& a_args)
		{
			Tasks::MessageBoxButtonPressDelegate::RegisterTask(a_args);
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

		static void StartRemapMode_Hook(const RE::FxDelegateArgs& a_args)
		{
			RE::MenuManager* mm = RE::MenuManager::GetSingleton();
			RE::UIStringHolder* strHolder = RE::UIStringHolder::GetSingleton();

			RE::IMenu* journalMenu = mm->GetMenu(strHolder->journalMenu).get();
			if (journalMenu && !journalMenu->PausesGame())
			{
				journalMenu->flags |= RE::IMenu::Flag::kPauseGame;
				mm->numPauseGame++;
			}

			void(*StartRemapMode_Original)(const RE::FxDelegateArgs&);
			StartRemapMode_Original = reinterpret_cast<void(*)(const RE::FxDelegateArgs&)>(Offsets::JournalMenu_StartRemapMode_Original.GetUIntPtr());
			return StartRemapMode_Original(a_args);
		}

		static void FinishRemapMode_Hook(RE::GFxMovieView* a_movieView, const char* a_methodName, RE::FxResponseArgsBase& a_args)
		{
			RE::FxDelegate::Invoke(a_movieView, a_methodName, a_args);

			RE::MenuManager* mm = RE::MenuManager::GetSingleton();
			RE::UIStringHolder* strHolder = RE::UIStringHolder::GetSingleton();
			SkyrimSoulsRE::SettingStore* settings = SkyrimSoulsRE::SettingStore::GetSingleton();

			RE::IMenu* journalMenu = mm->GetMenu(strHolder->journalMenu).get();
			if (journalMenu && journalMenu->PausesGame() && settings->GetSetting(settings->unpausedMenus, "journalMenu"))
			{
				journalMenu->flags &= ~RE::IMenu::Flag::kPauseGame;
				mm->numPauseGame--;
			}
		}

		static void InstallHook()
		{
			g_branchTrampoline.Write5Branch(Offsets::JournalMenu_SaveGame_Hook.GetUIntPtr(), (uintptr_t)SaveGame_Hook);
			g_branchTrampoline.Write5Call(Offsets::JournalMenu_SaveGame_Overwrite_Hook.GetUIntPtr(), (uintptr_t)SaveGame_Hook);
			g_branchTrampoline.Write5Call(Offsets::JournalMenu_SaveGame_Overwrite_Hook.GetUIntPtr() + 0x22, (uintptr_t)SaveGame_Hook);

			g_branchTrampoline.Write5Call(Offsets::JournalMenu_FinishRemapMode_Hook.GetUIntPtr(), (uintptr_t)FinishRemapMode_Hook);
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
				RE::MenuManager* mm = RE::MenuManager::GetSingleton();
				RE::UIStringHolder* strHolder = RE::UIStringHolder::GetSingleton();
				RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();

				mm->numPauseGame++;
				mm->GetMenu(strHolder->console)->flags |= RE::IMenu::Flag::kPauseGame;
				
				player->Unk_BA();
				return;
			}

			void(*ExecuteCommand_Original)(const RE::FxDelegateArgs&);
			ExecuteCommand_Original = reinterpret_cast<void(*)(const RE::FxDelegateArgs&)>(Offsets::Console_ExecuteCommand_Original.GetUIntPtr());
			return ExecuteCommand_Original(a_args);
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

	class MenuControlsEx
	{
	public:

		static bool IsMappedToSameButton(UInt32 a_keyMask, RE::DeviceType a_deviceType, RE::BSFixedString a_controlName, RE::InputMappingManager::Context a_context = RE::InputMappingManager::Context::kGameplay)
		{
			RE::InputMappingManager* imm = RE::InputMappingManager::GetSingleton();

			if (a_deviceType == RE::DeviceType::kKeyboard)
			{
				UInt32 keyMask = imm->GetMappedKey(a_controlName, RE::DeviceType::kKeyboard, a_context);
				return a_keyMask == keyMask;
			}
			else if (a_deviceType == RE::DeviceType::kMouse)
			{
				UInt32 keyMask = imm->GetMappedKey(a_controlName, RE::DeviceType::kMouse, a_context);
				return a_keyMask == keyMask;
			}
			return false;
		}

		static RE::EventResult ReceiveEvent_Hook(RE::MenuControls* a_this, RE::InputEvent** a_event, RE::BSTEventSource<RE::InputEvent*>* a_source)
		{
			RE::MenuManager* mm = RE::MenuManager::GetSingleton();
			RE::UIStringHolder* strHolder = RE::UIStringHolder::GetSingleton();
			RE::InputStringHolder* inStr = RE::InputStringHolder::GetSingleton();
			SkyrimSoulsRE::SettingStore* settings = SkyrimSoulsRE::SettingStore::GetSingleton();

			static bool enableMovementInMenus = settings->GetSetting(settings->controls, "bEnableMovementInMenus")->GetBool();
			static bool enableGamepadCameraMove = settings->GetSetting(settings->controls, "bEnableGamepadCameraMove")->GetBool();

			if (a_event && *a_event && SkyrimSoulsRE::unpausedMenuCount && !a_this->remapMode && !(mm->GameIsPaused()) && !(mm->IsMenuOpen(strHolder->dialogueMenu)))
			{
				for (RE::InputEvent* evn = *a_event; evn; evn = evn->next)
				{
					if (evn && evn->IsIDEvent())
					{
						RE::ButtonEvent* buttonEvn = reinterpret_cast<RE::ButtonEvent*>(evn);
						RE::ThumbstickEvent* thumbEvn = reinterpret_cast<RE::ThumbstickEvent*>(evn);

						if (enableMovementInMenus)
						{
							//Forward
							if (buttonEvn->controlID == inStr->up && IsMappedToSameButton(buttonEvn->keyMask, buttonEvn->deviceType, inStr->forward))
							{
								buttonEvn->controlID = inStr->forward;
							}
							//Back
							if (buttonEvn->controlID == inStr->down && IsMappedToSameButton(buttonEvn->keyMask, buttonEvn->deviceType, inStr->back))
							{
								buttonEvn->controlID = inStr->back;
							}
							//Left
							if (buttonEvn->controlID == inStr->left && IsMappedToSameButton(buttonEvn->keyMask, buttonEvn->deviceType, inStr->strafeLeft))
							{
								buttonEvn->controlID = inStr->strafeLeft;
							}
							//Right
							if (buttonEvn->controlID == inStr->right && IsMappedToSameButton(buttonEvn->keyMask, buttonEvn->deviceType, inStr->strafeRight))
							{
								buttonEvn->controlID = inStr->strafeRight;
							}
							//SkyUI Favorites menu fix
							if (mm->IsMenuOpen(strHolder->favoritesMenu))
							{
								//Prevent SkyUI from detecting the key mask
								//Left
								if (buttonEvn->controlID == inStr->strafeLeft && IsMappedToSameButton(buttonEvn->keyMask, buttonEvn->deviceType, inStr->strafeLeft))
								{
									buttonEvn->keyMask = 0;
								}
								//Left
								if (buttonEvn->controlID == inStr->strafeRight && IsMappedToSameButton(buttonEvn->keyMask, buttonEvn->deviceType, inStr->strafeRight))
								{
									buttonEvn->keyMask = 0;
								}
								//Allow category change with LB and RB when using controllers
								if (buttonEvn->deviceType == RE::DeviceType::kGamepad)
								{
									if (buttonEvn->keyMask == 0x100) //LB
									{
										buttonEvn->controlID = inStr->left;
									}
									if (buttonEvn->keyMask == 0x200) //RB
									{
										buttonEvn->controlID = inStr->right;
									}
								}
							}
							//Book menu fix
							if (mm->IsMenuOpen(strHolder->bookMenu))
							{
								//Left
								if (buttonEvn->controlID == inStr->prevPage && IsMappedToSameButton(buttonEvn->keyMask, buttonEvn->deviceType, inStr->strafeLeft))
								{
									buttonEvn->controlID = inStr->strafeLeft;
								}
								//Right
								if (buttonEvn->controlID == inStr->nextPage && IsMappedToSameButton(buttonEvn->keyMask, buttonEvn->deviceType, inStr->strafeRight))
								{
									buttonEvn->controlID = inStr->strafeRight;
								}
							}

							//Controllers
							if (thumbEvn->controlID == inStr->leftStick)
							{
								thumbEvn->controlID = inStr->move;
							}
						}

						if (enableGamepadCameraMove)
						{
							//Look controls for controllers - do not allow when an item preview is maximized, so it is still possible to rotate it somehow
							if (thumbEvn->controlID == inStr->rotate && !(*(reinterpret_cast<float*>(Offsets::ItemMenu_MaximizeStatus.GetUIntPtr())) == 1.0))
							{
								thumbEvn->controlID = inStr->look;
							}
						}
					}
				}
			}


			//Fix for SkyUI favorites menu
			

			RE::EventResult(*ReceiveEvent_Original)(RE::MenuControls*, RE::InputEvent**, RE::BSTEventSource<RE::InputEvent*>*);
			ReceiveEvent_Original = reinterpret_cast<RE::EventResult(*)(RE::MenuControls*, RE::InputEvent**, RE::BSTEventSource<RE::InputEvent*>*)>(Offsets::MenuControls_ReceiveEvent_Original.GetUIntPtr());;
			return ReceiveEvent_Original(a_this, a_event, a_source);
		}

		static void InstallHook()
		{
			SafeWrite64(Offsets::MenuControls_ReceiveEvent_Hook.GetUIntPtr(), (uintptr_t)ReceiveEvent_Hook);
		}
	};

	class CameraMoveEx
	{
	public:
		static bool CameraMove_Hook()
		{
			RE::MenuManager* mm = RE::MenuManager::GetSingleton();
			RE::UIStringHolder* strHolder = RE::UIStringHolder::GetSingleton();
			RE::PlayerControls* pc = RE::PlayerControls::GetSingleton();
			RE::MenuControls* mc = RE::MenuControls::GetSingleton();
			SkyrimSoulsRE::SettingStore* settings = SkyrimSoulsRE::SettingStore::GetSingleton();

			if (SkyrimSoulsRE::unpausedMenuCount && !mc->remapMode && !(mm->GameIsPaused()) && !(mm->IsMenuOpen(strHolder->dialogueMenu)))
			{
				RE::NiPoint2* cursorPosition = reinterpret_cast<RE::NiPoint2*>(Offsets::CursorPosition.GetUIntPtr());
				RE::INIPrefSettingCollection* pref = RE::INIPrefSettingCollection::GetSingleton();

				UInt32 resX = pref->GetSetting("iSize W:DISPLAY")->GetUInt();
				UInt32 resY = pref->GetSetting("iSize H:DISPLAY")->GetUInt();

				float speedX = settings->GetSetting(settings->controls, "fCursorCameraHorizontalSpeed")->GetFloat();
				float speedY = settings->GetSetting(settings->controls, "fCursorCameraVerticalSpeed")->GetFloat();

				if (cursorPosition->x == 0)
				{
					pc->movementData.unk08.x = -speedX;
				}
				else if (cursorPosition->x == resX)
				{
					pc->movementData.unk08.x = speedX;
				}

				if (cursorPosition->y == 0)
				{
					pc->movementData.unk08.y = speedY;
				}
				else if (cursorPosition->y == resY)
				{
					pc->movementData.unk08.y = -speedY;
				}
			}
			return false;
		}

		static void InstallHook()
		{
			struct CameraMove_Code : Xbyak::CodeGenerator
			{
				CameraMove_Code(void* buf, UInt64 a_hookAddr) : Xbyak::CodeGenerator(4096, buf)
				{
					Xbyak::Label hookAddress;

					add(rsp, 0x30);
					pop(rdi);
					jmp(ptr[rip + hookAddress]);

					L(hookAddress);
					dq(a_hookAddr);
				}
			};

			void* codeBuf = g_localTrampoline.StartAlloc();
			CameraMove_Code code(codeBuf, uintptr_t(CameraMove_Hook));
			g_localTrampoline.EndAlloc(code.getCurr());

			g_branchTrampoline.Write5Branch(Offsets::ScreenEdgeCameraMove_Hook.GetUIntPtr(), uintptr_t(code.getCode()));
		}
	};

	//MenuControls
	//Prevent menus from being opened from other menus
	class MenuOpenHandlerEx
	{
	public:
		static bool CanProcess_Hook(RE::MenuOpenHandler* a_this, RE::InputEvent* a_event)
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
			SafeWrite64(Offsets::MenuOpenHandler_CanProcess_Hook.GetUIntPtr(), (uintptr_t)CanProcess_Hook);
		}
	};

	class DrawNextFrameEx
	{
	public:
		//Initial position of player and the container
		static bool containerTooFarWhenOpened;
		static bool lockpickingTooFarWhenOpened;
		static bool bookTooFarWhenOpened;

		static float containerInitialDistance;
		static float lockpickingInitialDistance;
		static float bookInitialDistance;

		static bool containerInitiallyDisabled;
		static bool lockpickingInitiallyDisabled;
		static bool bookInitiallyDisabled;

		inline static float GetDistance(RE::NiPoint3 a_playerPos, float a_playerHeight, RE::NiPoint3 a_refPos)
		{
			//Get distance from feet and head, return the smaller
			float distanceHead = sqrt(pow(a_playerPos.x - a_refPos.x, 2) + pow(a_playerPos.y - a_refPos.y, 2) + pow((a_playerPos.z + a_playerHeight) - a_refPos.z, 2));
			float distanceFeet = sqrt(pow(a_playerPos.x - a_refPos.x, 2) + pow(a_playerPos.y - a_refPos.y, 2) + pow(a_playerPos.z - a_refPos.z, 2));
			if (distanceHead < distanceFeet) {
				return distanceHead;
			}
			return distanceFeet;
		}

		static void UpdateClock()
		{
			RE::BSTimeManager* tm = RE::BSTimeManager::GetSingleton();
			RE::MenuManager* mm = RE::MenuManager::GetSingleton();
			RE::UIStringHolder* strHolder = RE::UIStringHolder::GetSingleton();

			void(*GetTimeString)(RE::BSTimeManager * a_timeManager, char* a_str, UInt64 a_bufferSize, bool a_showYear);
			GetTimeString = reinterpret_cast<void(*)(RE::BSTimeManager*, char*, UInt64, bool)>(Offsets::GetFormattedTime_Original.GetUIntPtr());;

			//Tween menu clock
			if (mm->IsMenuOpen(strHolder->tweenMenu) && !(mm->GameIsPaused()))
			{
				RE::IMenu* tweenMenu = mm->GetMenu(strHolder->tweenMenu).get();
				if (tweenMenu)
				{
					char buf[200];
					GetTimeString(tm, buf, 200, true);

					RE::GFxValue dateText;
					tweenMenu->view->GetVariable(&dateText, "_root.TweenMenu_mc.BottomBarTweener_mc.BottomBar_mc.DateText");
					RE::GFxValue newDate(buf);
					dateText.SetMember("htmlText", newDate);
				}
			}

			//Journal Menu clock
			if (mm->IsMenuOpen(strHolder->journalMenu) && !(mm->GameIsPaused()))
			{
				RE::IMenu* journalMenu = mm->GetMenu(strHolder->journalMenu).get();
				if (journalMenu)
				{
					char buf[200];
					GetTimeString(tm, buf, 200, true);

					RE::GFxValue dateText;
					journalMenu->view->GetVariable(&dateText, "_root.QuestJournalFader.Menu_mc.BottomBar_mc.DateText");
					RE::GFxValue newDate(buf);
					dateText.SetMember("htmlText", newDate);
				}
			}

			//Sleep/Wait menu clock
			if (mm->IsMenuOpen(strHolder->sleepWaitMenu) && !(mm->GameIsPaused()))
			{
				RE::IMenu* sleepWaitMenu = mm->GetMenu(strHolder->sleepWaitMenu).get();
				if (sleepWaitMenu)
				{
					char buf[200];
					GetTimeString(tm, buf, 200, false);

					RE::GFxValue dateText;
					sleepWaitMenu->view->GetVariable(&dateText, "_root.SleepWaitMenu_mc.CurrentTime");
					RE::GFxValue newDate(buf);
					dateText.SetMember("htmlText", newDate);
				}
			}
		}

		static void UpdateBottomBar()
		{
			RE::MenuManager* mm = RE::MenuManager::GetSingleton();
			RE::UIStringHolder* strHolder = RE::UIStringHolder::GetSingleton();

			//Inventory Menu
			if (mm->IsMenuOpen(strHolder->inventoryMenu) && !(mm->GameIsPaused()))
			{
				RE::IMenu* inventoryMenu = mm->GetMenu(strHolder->inventoryMenu).get();
				if (inventoryMenu)
				{
					void(*UpdateBottomBar)(RE::IMenu*);
					UpdateBottomBar = reinterpret_cast<void(*)(RE::IMenu*)>(Offsets::InventoryMenu_UpdateBottomBarInfo.GetUIntPtr());;
					UpdateBottomBar(inventoryMenu);
				}
			}

			//Magic Menu
			if (mm->IsMenuOpen(strHolder->magicMenu) && !(mm->GameIsPaused()))
			{
				RE::IMenu* magicMenu = mm->GetMenu(strHolder->magicMenu).get();
				if (magicMenu)
				{
					void(*UpdateBottomBar)(RE::IMenu*);
					UpdateBottomBar = reinterpret_cast<void(*)(RE::IMenu*)>(Offsets::MagicMenu_UpdateBottomBarInfo.GetUIntPtr());;
					UpdateBottomBar(magicMenu);
				}
			}

			//Container Menu
			if (mm->IsMenuOpen(strHolder->containerMenu) && !(mm->GameIsPaused()))
			{
				RE::IMenu* containerMenu = mm->GetMenu(strHolder->containerMenu).get();
				if (containerMenu)
				{
					void(*UpdateBottomBar)(RE::IMenu*);
					UpdateBottomBar = reinterpret_cast<void(*)(RE::IMenu*)>(Offsets::ContainerMenu_UpdateBottomBarInfo.GetUIntPtr());;
					UpdateBottomBar(containerMenu);
				}
			}
		}

		static void CheckShouldClose()
		{
			RE::MenuManager * mm = RE::MenuManager::GetSingleton();
			RE::UIStringHolder * strHolder = RE::UIStringHolder::GetSingleton();
			RE::PlayerCharacter * player = RE::PlayerCharacter::GetSingleton();
			RE::UIManager * uiManager = RE::UIManager::GetSingleton();
			SkyrimSoulsRE::SettingStore * settings = SkyrimSoulsRE::SettingStore::GetSingleton();

			static bool autoClose = settings->GetSetting(settings->autoClose, "bAutoClose");
			static float maxDistance = static_cast<float>(settings->GetSetting(settings->autoClose, "uAutoCloseDistance")->GetInt());
			static bool containerMenuUnpaused = settings->GetSetting(settings->unpausedMenus, "containerMenu")->GetBool();
			static bool lockpickingMenuUnpaused = settings->GetSetting(settings->unpausedMenus, "lockpickingMenu")->GetBool();
			static bool bookMenuUnpaused = settings->GetSetting(settings->unpausedMenus, "bookMenu")->GetBool();

			//Auto-close Container menu
			if (mm->IsMenuOpen(strHolder->containerMenu) && containerMenuUnpaused)
			{
				RE::TESObjectREFR * ref = ContainerMenuEx::GetContainerRef();

				if (ref) {
					float currentDistance = GetDistance(player->pos, player->GetHeight(), ref->pos);

					if (SkyrimSoulsRE::justOpenedContainer)
					{
						containerInitialDistance = currentDistance;
						containerInitiallyDisabled = ref->IsDisabled();
						SkyrimSoulsRE::justOpenedContainer = false;

						containerTooFarWhenOpened = (containerInitialDistance > maxDistance) ? true : false;
					}

					if ((ref->IsDisabled() && !containerInitiallyDisabled) || ref->IsMarkedForDeletion())
					{
						uiManager->AddMessage(strHolder->containerMenu, RE::UIMessage::Message::kClose, 0);
					}

					if (ref->Is(RE::FormType::ActorCharacter))
					{
						if (ContainerMenuEx::GetContainerMode() == ContainerMenuEx::ContainerMode::kMode_Pickpocket && ref->IsDead(true))
						{
							uiManager->AddMessage(strHolder->containerMenu, RE::UIMessage::Message::kClose, 0);
						}
					}
					
					if (autoClose) {

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

			//Auto-close Lockpicking menu
			if (mm->IsMenuOpen(strHolder->lockpickingMenu) && lockpickingMenuUnpaused)
			{
				RE::TESObjectREFR * ref = LockpickingMenuEx::GetLockpickingTarget();
				if (ref)
				{
					float currentDistance = GetDistance(player->pos, player->GetHeight(), ref->pos);

					if (SkyrimSoulsRE::justOpenedLockpicking)
					{
						lockpickingInitialDistance = currentDistance;
						lockpickingInitiallyDisabled = ref->IsDisabled();
						SkyrimSoulsRE::justOpenedLockpicking = false;

						lockpickingTooFarWhenOpened = (lockpickingInitialDistance > maxDistance) ? true : false;
					}

					if ((ref->IsDisabled() && !lockpickingInitiallyDisabled) || ref->IsMarkedForDeletion())
					{
						uiManager->AddMessage(strHolder->lockpickingMenu, RE::UIMessage::Message::kClose, 0);
					}

					if (autoClose) {

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

			//Auto-close Book menu
			if (mm->IsMenuOpen(strHolder->bookMenu) && bookMenuUnpaused)
			{
				RE::TESObjectREFR* ref = BookMenuEx::GetBookReference();
				if (ref)
				{
					float currentDistance = GetDistance(player->pos, player->GetHeight(), ref->pos);

					if (SkyrimSoulsRE::justOpenedBook)
					{
						bookInitialDistance = currentDistance;
						bookInitiallyDisabled = ref->IsDisabled();
						SkyrimSoulsRE::justOpenedBook = false;

						bookTooFarWhenOpened = (bookInitialDistance > maxDistance) ? true : false;
					}

					if ((ref->IsDisabled() && !bookInitiallyDisabled) || ref->IsMarkedForDeletion())
					{
						uiManager->AddMessage(strHolder->lockpickingMenu, RE::UIMessage::Message::kClose, 0);
					}

					if (autoClose) {

						if (bookTooFarWhenOpened)
						{
							//Check if the distance is increasing
							if (currentDistance > (bookInitialDistance + 50))
							{
								uiManager->AddMessage(strHolder->bookMenu, RE::UIMessage::Message::kClose, 0);
							}
							else if ((bookInitialDistance - 50) > currentDistance) {
								//Check if it's already in range
								if (currentDistance < maxDistance)
								{
									bookTooFarWhenOpened = false;
								}
							}
						}
						else
						{
							if (currentDistance > maxDistance)
							{
								uiManager->AddMessage(strHolder->bookMenu, RE::UIMessage::Message::kClose, 0);
							}
						}
					}
				}
			}
		}

		static void DrawNextFrame_Hook()
		{
			//Slow motion
			SkyrimSoulsRE::SettingStore* settings = SkyrimSoulsRE::SettingStore::GetSingleton();
			static bool slowMotionEnabled = settings->GetSetting(settings->slowMotion, "bEnableSlowMotion")->GetBool();

			if (slowMotionEnabled && SkyrimSoulsRE::isInSlowMotion)
			{
				static UInt32 slowMotionPercent = settings->GetSetting(settings->slowMotion, "uSlowMotionPercent")->GetBool();

				float multiplier;
				if (slowMotionPercent >= 10 && 100 >= slowMotionPercent)
				{
					multiplier = (float)slowMotionPercent / 100.0;
				}
				else {
					multiplier = 1.0;
				}

				float* globalTimescale = reinterpret_cast<float*>(Offsets::GlobalTimescaleMultipler.GetUIntPtr());

				if (multiplier < *globalTimescale)
				{
					//fix for slow motion setting back the value to 1.0 after it ends
					*globalTimescale = multiplier;
				}

			}

			UpdateClock();
			UpdateBottomBar();
			CheckShouldClose();
		}

		static void InstallHook()
		{

			struct DrawNextFrame_Code : Xbyak::CodeGenerator
			{
				DrawNextFrame_Code(void * buf, UInt64 a_hookAddr) : Xbyak::CodeGenerator(4096, buf)
				{
					Xbyak::Label hookAddress;

					call(ptr[rip + hookAddress]);
					add(rsp, 0x50);
					pop(rbx);
					ret();

					L(hookAddress);
					dq(a_hookAddr);
				}
			};

			void * codeBuf = g_localTrampoline.StartAlloc();
			DrawNextFrame_Code code(codeBuf, uintptr_t(DrawNextFrame_Hook));
			g_localTrampoline.EndAlloc(code.getCurr());

			g_branchTrampoline.Write5Branch(Offsets::DrawNextFrame_Hook.GetUIntPtr(), uintptr_t(code.getCode()));
		}
	};
	bool DrawNextFrameEx::containerTooFarWhenOpened = false;
	bool DrawNextFrameEx::lockpickingTooFarWhenOpened = false;
	bool DrawNextFrameEx::bookTooFarWhenOpened = false;

	float DrawNextFrameEx::containerInitialDistance = 0.0;
	float DrawNextFrameEx::lockpickingInitialDistance = 0.0;
	float DrawNextFrameEx::bookInitialDistance = 0.0;

	bool DrawNextFrameEx::containerInitiallyDisabled = false;
	bool DrawNextFrameEx::lockpickingInitiallyDisabled = false;
	bool DrawNextFrameEx::bookInitiallyDisabled = false;

	void Register_Func(RE::FxDelegate* a_delegate, HookType a_type)
	{
		switch (a_type)
		{
		case kSleepWaitMenu:
			a_delegate->callbacks.GetAlt("OK")->callback = SleepWaitMenuEx::StartSleepWait_Hook;
			break;
		case kConsole:
			a_delegate->callbacks.GetAlt("ExecuteCommand")->callback = ConsoleEx::ExecuteCommand_Hook;
			break;
		case kMessageBoxMenu:
			a_delegate->callbacks.GetAlt("buttonPress")->callback = MessageBoxMenuEx::ButtonPress_Hook; //method name lowercase
			break;
		case kJournalMenu:
			a_delegate->callbacks.GetAlt("StartRemapMode")->callback = JournalMenuEx::StartRemapMode_Hook;
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

	void InstallHooks(HookShare::RegisterForCanProcess_t* a_register)
	{
		using HookShare::Hook;

		SkyrimSoulsRE::SettingStore* settings = SkyrimSoulsRE::SettingStore::GetSingleton();

		a_register(Hook::kFirstPersonState, _PlayerInputHandler_CanProcess);
		a_register(Hook::kThirdPersonState, _PlayerInputHandler_CanProcess);
		a_register(Hook::kFavorites, _PlayerInputHandler_CanProcess);
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

		if (settings->GetSetting(settings->controls, "bEnableMovementInMenus")->GetBool() || settings->GetSetting(settings->controls, "bEnableGamepadCameraMove")->GetBool())
		{
			MenuControlsEx::InstallHook();
		}

		if (!settings->GetSetting(settings->controls, "bEnableMovementInMenus")->GetBool())
		{
			a_register(Hook::kMovement, _PlayerInputHandler_CanProcess);
		}

		if (!settings->GetSetting(settings->controls, "bEnableGamepadCameraMove")->GetBool())
		{
			a_register(Hook::kLook, _PlayerInputHandler_CanProcess);
		}

		if (settings->GetSetting(settings->controls, "bEnableCursorCameraMove")->GetBool())
		{
			CameraMoveEx::InstallHook();
		}


		DialogueMenuEx::InstallHook();

		if (settings->GetSetting(settings->unpausedMenus, "tweenMenu")->GetBool()) {
			TweenMenuEx::InstallHook();
		}
		if (settings->GetSetting(settings->unpausedMenus, "favoritesMenu")->GetBool()) {
			FavoritesMenuEx::InstallHook();
		}
		if (settings->GetSetting(settings->unpausedMenus, "bookMenu")->GetBool()) {
			BookMenuEx::InstallHook();
		}
		if (settings->GetSetting(settings->unpausedMenus, "lockpickingMenu")->GetBool()) {
			LockpickingMenuEx::InstallHook();
		}
		if (settings->GetSetting(settings->unpausedMenus, "sleepWaitMenu")->GetBool()) {
			SleepWaitMenuEx::InstallHook();
		}
		if (settings->GetSetting(settings->unpausedMenus, "containerMenu")->GetBool()) {
			ContainerMenuEx::InstallHook();
		}
		if (settings->GetSetting(settings->unpausedMenus, "inventoryMenu")->GetBool()) {
			InventoryMenuEx::InstallHook();
		}
		if (settings->GetSetting(settings->unpausedMenus, "journalMenu")->GetBool()) {
			JournalMenuEx::InstallHook();
		}

		DrawNextFrameEx::InstallHook();
		PapyrusEx::InstallHook();
		MenuOpenHandlerEx::InstallHook();
	}
}
