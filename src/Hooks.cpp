#include "Hooks.h"

#include "skse64_common/BranchTrampoline.h"  // g_trampoline
#include "skse64_common/Relocation.h"  // RelocAddr, RelocPtr
#include "skse64_common/SafeWrite.h"  // SafeWrite

#include "skse64/InputMap.h" //InputMap::GamepadMaskToKeycode

#include "xbyak/xbyak.h"

#include "RE/Skyrim.h"

#include "Events.h"
#include "Offsets.h"
#include "Settings.h"
#include "Tasks.h"
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
		static void DropItem_Hook(RE::PlayerCharacter* a_player, RE::ObjectRefHandle& a_droppedItemHandle, RE::TESBoundObject* a_item, RE::ExtraDataList* a_extraList, UInt32 a_count, void* a_arg5, void* a_arg6)
		{
			//Looks like some other thread moves it before the calculation is complete, resulting in the item being moved to the coc marker
			//This is an ugly workaround, but it should work good enought

			RE::UI* ui = RE::UI::GetSingleton();
			ui->numPausesGame++;
			std::this_thread::sleep_for(std::chrono::milliseconds(10));

			a_player->DropObject(a_droppedItemHandle, a_item,  a_extraList, a_count, a_arg5, a_arg6);

			ui->numPausesGame--;
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
			RE::UI* ui = RE::UI::GetSingleton();
			RE::InterfaceStrings* interfaceStrings = RE::InterfaceStrings::GetSingleton();
			if (ui->IsMenuOpen(interfaceStrings->containerMenu))
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
			RE::UI* ui = RE::UI::GetSingleton();
			RE::InterfaceStrings* interfaceStrings = RE::InterfaceStrings::GetSingleton();
			SkyrimSoulsRE::Settings* settings = SkyrimSoulsRE::Settings::GetSingleton();

			ui->GetMenu(interfaceStrings->sleepWaitMenu)->flags |= RE::IMenu::Flag::kPausesGame;
			ui->numPausesGame++;

			if (SkyrimSoulsRE::isInSlowMotion)
			{
				float slowMotionMultiplier = settings->slowMotionMultiplier;
				float* globalTimescale = reinterpret_cast<float*>(Offsets::GlobalTimescaleMultipler.GetUIntPtr());
				float* globalTimescaleHavok = reinterpret_cast<float*>(Offsets::GlobalTimescaleMultipler_Havok.GetUIntPtr());

				float multiplier;
				if (slowMotionMultiplier >= 0.1 && 1.0 >= slowMotionMultiplier)
				{
					multiplier = slowMotionMultiplier;
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

		static void SetJournalPaused(bool a_paused)
		{
			RE::UI* ui = RE::UI::GetSingleton();
			RE::InterfaceStrings* interfaceStrings = RE::InterfaceStrings::GetSingleton();
			SkyrimSoulsRE::Settings* settings = SkyrimSoulsRE::Settings::GetSingleton();

			RE::IMenu* journalMenu = ui->GetMenu(interfaceStrings->journalMenu).get();

			if (journalMenu && settings->unpausedMenus["Journal Menu"])
			{
				if (a_paused)
				{
					journalMenu->flags |= RE::IMenu::Flag::kPausesGame;
					ui->numPausesGame++;
				}
				else
				{
					journalMenu->flags &= ~RE::IMenu::Flag::kPausesGame;
					ui->numPausesGame--;
				}
			}
		}

		//Copied from SKSE64
		class SKSEScaleform_StartRemapMode : public RE::GFxFunctionHandler
		{
			class RemapHandler : public RE::BSTEventSink<RE::InputEvent*>
			{
			public:

				//End remap mode - controls menu
				virtual RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const * a_event, RE::BSTEventSource<RE::InputEvent*>* a_eventSource) override
				{
					RE::ButtonEvent* evn = (RE::ButtonEvent*) * a_event;

					// Make sure this is really a button event
					if (!evn || evn->eventType != RE::INPUT_EVENT_TYPE::kButton)
						return RE::BSEventNotifyControl::kContinue;

					RE::INPUT_DEVICE deviceType = evn->device;

					RE::BSInputDeviceManager* idm = static_cast<RE::BSInputDeviceManager*>(a_eventSource);

					if ((idm->IsGamepadEnabled() ^ (deviceType == RE::INPUT_DEVICE::kGamepad)) || evn->value == 0 || evn->heldDownSecs != 0.0)
					{
						return RE::BSEventNotifyControl::kContinue;
					}
						

					UInt32 keyMask = evn->idCode;
					UInt32 keyCode;

					// Mouse
					if (deviceType == RE::INPUT_DEVICE::kMouse)
						keyCode = InputMap::kMacro_MouseButtonOffset + keyMask;
					// Gamepad
					else if (deviceType == RE::INPUT_DEVICE::kGamepad)
						keyCode = InputMap::GamepadMaskToKeycode(keyMask);
					// Keyboard
					else
						keyCode = keyMask;

					// Valid scancode?
					if (keyCode >= InputMap::kMaxMacros)
						keyCode = -1;

					RE::GFxValue arg;
					arg.SetNumber(keyCode);
					scope.Invoke("EndRemapMode", NULL, &arg, 1);
					SetJournalPaused(false);

					RE::MenuControls::GetSingleton()->remapMode = false;
					RE::PlayerControls::GetSingleton()->data.remapMode = false;

					a_eventSource->RemoveEventSink(this);
					return RE::BSEventNotifyControl::kContinue;
				}

			RE::GFxValue scope;
		};

		RemapHandler remapHandler;

		public:

			//Start remap mode - MCM menu
			virtual void Call(RE::GFxFunctionHandler::Params& a_args) override
			{
				_ASSERT(a_args->numArgs >= 1);

				SetJournalPaused(true);

				remapHandler.scope = a_args.args[0];

				RE::PlayerControls* playerControls = RE::PlayerControls::GetSingleton();
				if (!playerControls)
					return;

				RE::MenuControls* menuControls = RE::MenuControls::GetSingleton();
				if (!menuControls)
					return;
				RE::BSInputDeviceManager* pInputEventDispatcher = RE::BSInputDeviceManager::GetSingleton();
				if (!(pInputEventDispatcher))
					return;

				pInputEventDispatcher->AddEventSink(&remapHandler);
				menuControls->remapMode = true;
				playerControls->data.remapMode = true;
			}
		};

		//Start remap mode - controls menu
		static void StartRemapMode_Hook(const RE::FxDelegateArgs& a_args)
		{
			SetJournalPaused(true);

			void(*StartRemapMode_Original)(const RE::FxDelegateArgs&);
			StartRemapMode_Original = reinterpret_cast<void(*)(const RE::FxDelegateArgs&)>(Offsets::JournalMenu_StartRemapMode_Original.GetUIntPtr());
			return StartRemapMode_Original(a_args);
		}

		//End remap mode - controls menu
		static void FinishRemapMode_Hook(RE::GFxMovieView* a_movieView, const char* a_methodName, RE::FxResponseArgsBase& a_args)
		{

			RE::FxDelegate::Invoke(a_movieView, a_methodName, a_args);

			SetJournalPaused(false);
		}

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
				RE::UI* ui = RE::UI::GetSingleton();
				RE::InterfaceStrings* interfaceStrings = RE::InterfaceStrings::GetSingleton();
				RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();

				ui->numPausesGame++;
				ui->GetMenu(interfaceStrings->console)->flags |= RE::IMenu::Flag::kPausesGame;
				
				player->ServePrisonTime();
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

		static bool IsMappedToSameButton(UInt32 a_keyMask, RE::INPUT_DEVICE a_deviceType, RE::BSFixedString a_controlName, RE::UserEvents::INPUT_CONTEXT_ID a_context = RE::UserEvents::INPUT_CONTEXT_ID::kGameplay)
		{
			RE::ControlMap* controlMap = RE::ControlMap::GetSingleton();

			if (a_deviceType == RE::INPUT_DEVICE::kKeyboard)
			{
				UInt32 keyMask = controlMap->GetMappedKey(a_controlName, RE::INPUT_DEVICE::kKeyboard, a_context);
				return a_keyMask == keyMask;
			}
			else if (a_deviceType == RE::INPUT_DEVICE::kMouse)
			{
				UInt32 keyMask = controlMap->GetMappedKey(a_controlName, RE::INPUT_DEVICE::kMouse, a_context);
				return a_keyMask == keyMask;
			}
			return false;
		}

		static RE::BSEventNotifyControl ReceiveEvent_Hook(RE::MenuControls* a_this, RE::InputEvent** a_event, RE::BSTEventSource<RE::InputEvent*>* a_source)
		{
			RE::UI* ui = RE::UI::GetSingleton();
			RE::InterfaceStrings* interfaceStrings = RE::InterfaceStrings::GetSingleton();
			RE::UserEvents* inputStrings = RE::UserEvents::GetSingleton();
			SkyrimSoulsRE::Settings* settings = SkyrimSoulsRE::Settings::GetSingleton();

			if (a_event && *a_event && !a_this->remapMode && !(ui->GameIsPaused()) && ((SkyrimSoulsRE::unpausedMenuCount) || (ui->IsMenuOpen(interfaceStrings->dialogueMenu) && settings->enableMovementDialogueMenu)))
			{
				for (RE::InputEvent* evn = *a_event; evn; evn = evn->next)
				{
					if (evn && evn->HasIDCode())
					{
						RE::ButtonEvent* buttonEvn = reinterpret_cast<RE::ButtonEvent*>(evn);
						RE::ThumbstickEvent* thumbEvn = reinterpret_cast<RE::ThumbstickEvent*>(evn);

						if (settings->enableMovementInMenus)
						{
							//Forward
							if (buttonEvn->userEvent == inputStrings->up && IsMappedToSameButton(buttonEvn->idCode, buttonEvn->device, inputStrings->forward))
							{
								buttonEvn->userEvent = inputStrings->forward;
							}
							//Back
							if (buttonEvn->userEvent == inputStrings->down && IsMappedToSameButton(buttonEvn->idCode, buttonEvn->device, inputStrings->back))
							{
								buttonEvn->userEvent = inputStrings->back;
							}
							//Left
							if (buttonEvn->userEvent == inputStrings->left && IsMappedToSameButton(buttonEvn->idCode, buttonEvn->device, inputStrings->strafeLeft))
							{
								buttonEvn->userEvent = inputStrings->strafeLeft;
							}
							//Right
							if (buttonEvn->userEvent == inputStrings->right && IsMappedToSameButton(buttonEvn->idCode, buttonEvn->device, inputStrings->strafeRight))
							{
								buttonEvn->userEvent = inputStrings->strafeRight;
							}
							//SkyUI Favorites menu fix
							if (ui->IsMenuOpen(interfaceStrings->favoritesMenu))
							{
								//Prevent SkyUI from detecting the key mask
								//Left
								if (buttonEvn->userEvent == inputStrings->strafeLeft && IsMappedToSameButton(buttonEvn->idCode, buttonEvn->device, inputStrings->strafeLeft))
								{
									buttonEvn->idCode = 0;
								}
								//Left
								if (buttonEvn->userEvent == inputStrings->strafeRight && IsMappedToSameButton(buttonEvn->idCode, buttonEvn->device, inputStrings->strafeRight))
								{
									buttonEvn->idCode = 0;
								}
								//Allow category change with LB and RB when using controllers
								if (buttonEvn->device == RE::INPUT_DEVICE::kGamepad)
								{
									if (buttonEvn->idCode == 0x100) //LB
									{
										buttonEvn->userEvent = inputStrings->left;
									}
									if (buttonEvn->idCode == 0x200) //RB
									{
										buttonEvn->userEvent = inputStrings->right;
									}
								}
							}
							//Book menu fix
							if (ui->IsMenuOpen(interfaceStrings->bookMenu))
							{
								//Left
								if (buttonEvn->userEvent == inputStrings->prevPage && IsMappedToSameButton(buttonEvn->idCode, buttonEvn->device, inputStrings->strafeLeft))
								{
									buttonEvn->userEvent = inputStrings->strafeLeft;
								}
								//Right
								if (buttonEvn->userEvent == inputStrings->nextPage && IsMappedToSameButton(buttonEvn->idCode, buttonEvn->device, inputStrings->strafeRight))
								{
									buttonEvn->userEvent = inputStrings->strafeRight;
								}
							}

							//Controllers
							if (thumbEvn->userEvent == inputStrings->leftStick)
							{
								thumbEvn->userEvent = inputStrings->move;
							}
						}

						if (settings->enableGamepadCameraMove)
						{
							//Look controls for controllers - do not allow when an item preview is maximized, so it is still possible to rotate it somehow
							if (thumbEvn->userEvent == inputStrings->rotate && !(*(reinterpret_cast<float*>(Offsets::ItemMenu_MaximizeStatus.GetUIntPtr())) == 1.0))
							{
								thumbEvn->userEvent = inputStrings->look;
							}
						}

					}
				}
			}		

			RE::BSEventNotifyControl(*ReceiveEvent_Original)(RE::MenuControls*, RE::InputEvent**, RE::BSTEventSource<RE::InputEvent*>*);
			ReceiveEvent_Original = reinterpret_cast<RE::BSEventNotifyControl(*)(RE::MenuControls*, RE::InputEvent**, RE::BSTEventSource<RE::InputEvent*>*)>(Offsets::MenuControls_ReceiveEvent_Original.GetUIntPtr());;
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
		static bool CameraMove_Hook(bool a_retVal)
		{
			RE::UI* ui = RE::UI::GetSingleton();
			RE::InterfaceStrings* interfaceStrings = RE::InterfaceStrings::GetSingleton();
			RE::PlayerControls* pc = RE::PlayerControls::GetSingleton();
			RE::MenuControls* mc = RE::MenuControls::GetSingleton();
			SkyrimSoulsRE::Settings* settings = SkyrimSoulsRE::Settings::GetSingleton();

			if (SkyrimSoulsRE::unpausedMenuCount && !mc->remapMode && !(ui->GameIsPaused()) && !(ui->IsMenuOpen(interfaceStrings->dialogueMenu)))
			{
				RE::NiPoint2* cursorPosition = reinterpret_cast<RE::NiPoint2*>(Offsets::CursorPosition.GetUIntPtr());
				RE::INIPrefSettingCollection* pref = RE::INIPrefSettingCollection::GetSingleton();

				UInt32 resX = pref->GetSetting("iSize W:DISPLAY")->GetUInt();
				UInt32 resY = pref->GetSetting("iSize H:DISPLAY")->GetUInt();

				float speedX = settings->cursorCameraHorizontalSpeed;
				float speedY = settings->cursorCameraVerticalSpeed;

				if (cursorPosition->x == 0)
				{
					pc->data.lookInputVec.x = -speedX;
				}
				else if (cursorPosition->x == resX)
				{
					pc->data.lookInputVec.x = speedX;
				}

				if (cursorPosition->y == 0)
				{
					pc->data.lookInputVec.y = speedY;
				}
				else if (cursorPosition->y == resY)
				{
					pc->data.lookInputVec.y = -speedY;
				}
			}
			return a_retVal;
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
					mov(rcx, rax);
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
			RE::Calendar* calendar = RE::Calendar::GetSingleton();
			RE::UI* ui = RE::UI::GetSingleton();
			RE::InterfaceStrings* interfaceStrings = RE::InterfaceStrings::GetSingleton();

			void(*GetTimeString)(RE::Calendar* a_this, char* a_str, UInt64 a_bufferSize, bool a_showYear);
			GetTimeString = reinterpret_cast<void(*)(RE::Calendar*, char*, UInt64, bool)>(Offsets::GetFormattedTime_Original.GetUIntPtr());;

			//Tween menu clock
			if (ui->IsMenuOpen(interfaceStrings->tweenMenu) && !(ui->GameIsPaused()))
			{
				RE::IMenu* tweenMenu = ui->GetMenu(interfaceStrings->tweenMenu).get();
				if (tweenMenu)
				{
					char buf[200];
					GetTimeString(calendar, buf, 200, true);

					RE::GFxValue dateText;
					tweenMenu->view->GetVariable(&dateText, "_root.TweenMenu_mc.BottomBarTweener_mc.BottomBar_mc.DateText");
					RE::GFxValue newDate(buf);
					dateText.SetMember("htmlText", newDate);
				}
			}

			//Journal Menu clock
			if (ui->IsMenuOpen(interfaceStrings->journalMenu) && !(ui->GameIsPaused()))
			{
				RE::IMenu* journalMenu = ui->GetMenu(interfaceStrings->journalMenu).get();
				if (journalMenu)
				{
					char buf[200];
					GetTimeString(calendar, buf, 200, true);

					RE::GFxValue dateText;
					journalMenu->view->GetVariable(&dateText, "_root.QuestJournalFader.Menu_mc.BottomBar_mc.DateText");
					RE::GFxValue newDate(buf);
					dateText.SetMember("htmlText", newDate);
				}
			}

			//Sleep/Wait menu clock
			if (ui->IsMenuOpen(interfaceStrings->sleepWaitMenu) && !(ui->GameIsPaused()))
			{
				RE::IMenu* sleepWaitMenu = ui->GetMenu(interfaceStrings->sleepWaitMenu).get();
				if (sleepWaitMenu)
				{
					char buf[200];
					GetTimeString(calendar, buf, 200, false);

					RE::GFxValue dateText;
					sleepWaitMenu->view->GetVariable(&dateText, "_root.SleepWaitMenu_mc.CurrentTime");
					RE::GFxValue newDate(buf);
					dateText.SetMember("htmlText", newDate);
				}
			}
		}

		static void UpdateBottomBar()
		{
			RE::UI* ui = RE::UI::GetSingleton();
			RE::InterfaceStrings* interfaceStrings = RE::InterfaceStrings::GetSingleton();

			//Inventory Menu
			if (ui->IsMenuOpen(interfaceStrings->inventoryMenu) && !(ui->GameIsPaused()))
			{
				RE::IMenu* inventoryMenu = ui->GetMenu(interfaceStrings->inventoryMenu).get();
				if (inventoryMenu)
				{
					void(*UpdateBottomBar)(RE::IMenu*);
					UpdateBottomBar = reinterpret_cast<void(*)(RE::IMenu*)>(Offsets::InventoryMenu_UpdateBottomBarInfo.GetUIntPtr());;
					UpdateBottomBar(inventoryMenu);
				}
			}

			//Magic Menu
			if (ui->IsMenuOpen(interfaceStrings->magicMenu) && !(ui->GameIsPaused()))
			{
				RE::IMenu* magicMenu = ui->GetMenu(interfaceStrings->magicMenu).get();
				if (magicMenu)
				{
					void(*UpdateBottomBar)(RE::IMenu*);
					UpdateBottomBar = reinterpret_cast<void(*)(RE::IMenu*)>(Offsets::MagicMenu_UpdateBottomBarInfo.GetUIntPtr());;
					UpdateBottomBar(magicMenu);
				}
			}

			//Container Menu
			if (ui->IsMenuOpen(interfaceStrings->containerMenu) && !(ui->GameIsPaused()))
			{
				RE::IMenu* containerMenu = ui->GetMenu(interfaceStrings->containerMenu).get();
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
			RE::UI* ui = RE::UI::GetSingleton();
			RE::InterfaceStrings* interfaceStrings = RE::InterfaceStrings::GetSingleton();
			RE::PlayerCharacter * player = RE::PlayerCharacter::GetSingleton();
			RE::UIMessageQueue* uiMessageQueue = RE::UIMessageQueue::GetSingleton();
			SkyrimSoulsRE::Settings * settings = SkyrimSoulsRE::Settings::GetSingleton();

			float autoCloseDistance = settings->autoCloseDistance;

			//Auto-close Container menu
			if (ui->IsMenuOpen(interfaceStrings->containerMenu) && settings->unpausedMenus["ContainerMenu"])
			{
				RE::TESObjectREFR * ref = ContainerMenuEx::GetContainerRef();

				ContainerMenuEx::ContainerMode mode = ContainerMenuEx::GetContainerMode();

				if (ref) {
					float currentDistance = GetDistance(player->GetPosition(), player->GetHeight(), ref->GetPosition());

					if (SkyrimSoulsRE::justOpenedContainer)
					{
						containerInitialDistance = currentDistance;
						containerInitiallyDisabled = ref->IsDisabled();
						SkyrimSoulsRE::justOpenedContainer = false;

						containerTooFarWhenOpened = (containerInitialDistance > autoCloseDistance) ? true : false;
					}

					if ((ref->IsDisabled() && !containerInitiallyDisabled) || ref->IsMarkedForDeletion())
					{
						uiMessageQueue->AddMessage(interfaceStrings->containerMenu, RE::UIMessage::Message::kClose, 0);
					}

					if (ref->Is(RE::FormType::ActorCharacter))
					{
						if (ContainerMenuEx::GetContainerMode() == ContainerMenuEx::ContainerMode::kMode_Pickpocket && ref->IsDead(true))
						{
							uiMessageQueue->AddMessage(interfaceStrings->containerMenu, RE::UIMessage::Message::kClose, 0);
						}
					}
					
					if (settings->autoCloseMenus) {

						if (containerTooFarWhenOpened)
						{
							//Check if the distance is increasing
							if (currentDistance > (containerInitialDistance + 50))
							{
								uiMessageQueue->AddMessage(interfaceStrings->containerMenu, RE::UIMessage::Message::kClose, 0);
							}
							else if ((containerInitialDistance - 50) > currentDistance) {
								//Check if it's already in range
								if (currentDistance < autoCloseDistance)
								{
									containerTooFarWhenOpened = false;
								}
							}
						}
						else
						{
							if (currentDistance > autoCloseDistance)
							{
								uiMessageQueue->AddMessage(interfaceStrings->containerMenu, RE::UIMessage::Message::kClose, 0);
							}
						}
					}
				}
			}

			//Auto-close Lockpicking menu
			if (ui->IsMenuOpen(interfaceStrings->lockpickingMenu) && settings->unpausedMenus["Lockpicking Menu"])
			{
				RE::TESObjectREFR * ref = LockpickingMenuEx::GetLockpickingTarget();
				if (ref)
				{
					float currentDistance = GetDistance(player->GetPosition(), player->GetHeight(), ref->GetPosition());

					if (SkyrimSoulsRE::justOpenedLockpicking)
					{
						lockpickingInitialDistance = currentDistance;
						lockpickingInitiallyDisabled = ref->IsDisabled();
						SkyrimSoulsRE::justOpenedLockpicking = false;

						lockpickingTooFarWhenOpened = (lockpickingInitialDistance > autoCloseDistance) ? true : false;
					}

					if ((ref->IsDisabled() && !lockpickingInitiallyDisabled) || ref->IsMarkedForDeletion())
					{
						uiMessageQueue->AddMessage(interfaceStrings->lockpickingMenu, RE::UIMessage::Message::kClose, 0);
					}

					if (settings->autoCloseMenus) {

						if (lockpickingTooFarWhenOpened)
						{
							//Check if the distance is increasing
							if (currentDistance > (lockpickingInitialDistance + 50))
							{
								uiMessageQueue->AddMessage(interfaceStrings->lockpickingMenu, RE::UIMessage::Message::kClose, 0);
							}
							else if ((lockpickingInitialDistance - 50) > currentDistance) {
								//Check if it's already in range
								if (currentDistance < autoCloseDistance)
								{
									lockpickingTooFarWhenOpened = false;
								}
							}
						}
						else
						{
							if (currentDistance > autoCloseDistance)
							{
								uiMessageQueue->AddMessage(interfaceStrings->lockpickingMenu, RE::UIMessage::Message::kClose, 0);
							}
						}
					}
				}
			}

			//Auto-close Book menu
			if (ui->IsMenuOpen(interfaceStrings->bookMenu) && settings->unpausedMenus["Book Menu"])
			{
				RE::TESObjectREFR* ref = BookMenuEx::GetBookReference();
				if (ref)
				{
					float currentDistance = GetDistance(player->GetPosition(), player->GetHeight(), ref->GetPosition());

					if (SkyrimSoulsRE::justOpenedBook)
					{
						bookInitialDistance = currentDistance;
						bookInitiallyDisabled = ref->IsDisabled();
						SkyrimSoulsRE::justOpenedBook = false;

						bookTooFarWhenOpened = (bookInitialDistance > autoCloseDistance) ? true : false;
					}

					if ((ref->IsDisabled() && !bookInitiallyDisabled) || ref->IsMarkedForDeletion())
					{
						uiMessageQueue->AddMessage(interfaceStrings->lockpickingMenu, RE::UIMessage::Message::kClose, 0);
					}

					if (settings->autoCloseMenus) {

						if (bookTooFarWhenOpened)
						{
							//Check if the distance is increasing
							if (currentDistance > (bookInitialDistance + 50))
							{
								uiMessageQueue->AddMessage(interfaceStrings->bookMenu, RE::UIMessage::Message::kClose, 0);
							}
							else if ((bookInitialDistance - 50) > currentDistance) {
								//Check if it's already in range
								if (currentDistance < autoCloseDistance)
								{
									bookTooFarWhenOpened = false;
								}
							}
						}
						else
						{
							if (currentDistance > autoCloseDistance)
							{
								uiMessageQueue->AddMessage(interfaceStrings->bookMenu, RE::UIMessage::Message::kClose, 0);
							}
						}
					}
				}
			}
		}

		static void DrawNextFrame_Hook()
		{
			//Slow motion
			SkyrimSoulsRE::Settings* settings = SkyrimSoulsRE::Settings::GetSingleton();

			if (settings->enableSlowMotion && SkyrimSoulsRE::isInSlowMotion)
			{
				float slowMotionMultiplier = settings->slowMotionMultiplier;

				float multiplier;
				if (slowMotionMultiplier >= 0.1 && 1.0 >= slowMotionMultiplier)
				{
					multiplier = slowMotionMultiplier;
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

	void Register_Func(RE::IMenu* a_menu, HookType a_type)
	{
		RE::FxDelegate* dlg = a_menu->fxDelegate.get();

		switch (a_type)
		{
		case kSleepWaitMenu:
			dlg->callbacks.GetAlt("OK")->callback = SleepWaitMenuEx::StartSleepWait_Hook;
			break;
		case kConsole:
			dlg->callbacks.GetAlt("ExecuteCommand")->callback = ConsoleEx::ExecuteCommand_Hook;
			break;
		case kMessageBoxMenu:
			dlg->callbacks.GetAlt("buttonPress")->callback = MessageBoxMenuEx::ButtonPress_Hook; //method name lowercase
			break;
		case kJournalMenu:

			//Temporary fix for remapping from controls menu
			dlg->callbacks.GetAlt("StartRemapMode")->callback = JournalMenuEx::StartRemapMode_Hook;

			//Temporary fix for remapping from MCM menu
			RE::GFxValue globals, skse;

			bool result = a_menu->view->GetVariable(&globals, "_global");
			if (!result)
			{
				_ERROR("Couldn't get _global");
				return;
			}

			result = globals.GetMember("skse", &skse);
			if (!result)
			{
				_ERROR("Couldn't get skse");
				return;
			}

			RE::GFxFunctionHandler* fn = nullptr;

			fn = new JournalMenuEx::SKSEScaleform_StartRemapMode();
			RE::GFxValue fnValue;
			a_menu->view.get()->CreateFunction(&fnValue, fn);
			skse.SetMember("StartRemapMode", fnValue);
		}
	}

	template <std::uintptr_t offset>
	class PlayerInputHandler : public RE::PlayerInputHandler
	{
	public:
		using func_t = function_type_t<decltype(&RE::PlayerInputHandler::CanProcess)>;
		static inline func_t* func;

		bool CanProcess_Hook(RE::InputEvent* a_event)
		{
			if (SkyrimSoulsRE::MenuOpenCloseEventHandler::BlockInput())
			{
				return false;
			}
			return func(this, a_event);
		}

		static void InstallHook()
		{
			REL::Offset<func_t**> vFunc(offset);
			func = *vFunc;
			SafeWrite64(vFunc.GetAddress(), GetFnAddr(&CanProcess_Hook));
		}
	};

	void InstallHooks()
	{
		SkyrimSoulsRE::Settings* settings = SkyrimSoulsRE::Settings::GetSingleton();

		using FirstPersonStateHandlerEx = PlayerInputHandler<RE::Offset::FirstPersonState::Vtbl + (0x8 * 0x8) + 0x10 + (0x1 * 0x8)>;
		using ThirdPersonStateHandlerEx = PlayerInputHandler<RE::Offset::ThirdPersonState::Vtbl + (0xF * 0x8) + 0x10 + (0x1 * 0x8)>;
		using FavoritesHandlerEx = PlayerInputHandler<RE::Offset::FavoritesHandler::Vtbl + (0x1 * 0x8)>;
		using MovementHandlerEx = PlayerInputHandler<RE::Offset::MovementHandler::Vtbl + (0x1 * 0x8)>;
		using LookHandlerEx = PlayerInputHandler<RE::Offset::LookHandler::Vtbl + (0x1 * 0x8)>;
		using SprintHandlerEx = PlayerInputHandler<RE::Offset::SprintHandler::Vtbl + (0x1 * 0x8)>;
		using ReadyWeaponHandlerEx = PlayerInputHandler<RE::Offset::ReadyWeaponHandler::Vtbl + (0x1 * 0x8)>;
		using AutoMoveHandlerEx = PlayerInputHandler<RE::Offset::AutoMoveHandler::Vtbl + (0x1 * 0x8)>;
		using ToggleRunHandlerEx = PlayerInputHandler<RE::Offset::ToggleRunHandler::Vtbl + (0x1 * 0x8)>;
		using ActivateHandlerEx = PlayerInputHandler<RE::Offset::ActivateHandler::Vtbl + (0x1 * 0x8)>;
		using JumpHandlerEx = PlayerInputHandler<RE::Offset::JumpHandler::Vtbl + (0x1 * 0x8)>;
		using ShoutHandlerEx = PlayerInputHandler<RE::Offset::ShoutHandler::Vtbl + (0x1 * 0x8)>;
		using AttackBlockHandlerEx = PlayerInputHandler<RE::Offset::AttackBlockHandler::Vtbl + (0x1 * 0x8)>;
		using RunHandlerEx = PlayerInputHandler<RE::Offset::RunHandler::Vtbl + (0x1 * 0x8)>;
		using SneakHandlerEx = PlayerInputHandler<RE::Offset::SneakHandler::Vtbl + (0x1 * 0x8)>;

		FirstPersonStateHandlerEx::InstallHook();
		ThirdPersonStateHandlerEx::InstallHook();
		FavoritesHandlerEx::InstallHook();
		SprintHandlerEx::InstallHook();
		ReadyWeaponHandlerEx::InstallHook();
		AutoMoveHandlerEx::InstallHook();
		ToggleRunHandlerEx::InstallHook();
		ActivateHandlerEx::InstallHook();
		JumpHandlerEx::InstallHook();
		ShoutHandlerEx::InstallHook();
		AttackBlockHandlerEx::InstallHook();
		RunHandlerEx::InstallHook();
		SneakHandlerEx::InstallHook();

		if (settings->enableMovementInMenus || settings->enableGamepadCameraMove)
		{
			MenuControlsEx::InstallHook();
		}

		if (!settings->enableMovementInMenus)
		{
			MovementHandlerEx::InstallHook();
		}

		if (!settings->enableGamepadCameraMove)
		{
			LookHandlerEx::InstallHook();
		}

		if (settings->enableCursorCameraMove)
		{
			CameraMoveEx::InstallHook();
		}


		DialogueMenuEx::InstallHook();

		if (settings->unpausedMenus["TweenMenu"]) {
			TweenMenuEx::InstallHook();
		}
		if (settings->unpausedMenus["FavoritesMenu"]) {
			FavoritesMenuEx::InstallHook();
		}
		if (settings->unpausedMenus["Book Menu"]) {
			BookMenuEx::InstallHook();
		}
		if (settings->unpausedMenus["Lockpicking Menu"]) {
			LockpickingMenuEx::InstallHook();
		}
		if (settings->unpausedMenus["Sleep/Wait Menu"]) {
			SleepWaitMenuEx::InstallHook();
		}
		if (settings->unpausedMenus["ContainerMenu"]) {
			ContainerMenuEx::InstallHook();
		}
		if (settings->unpausedMenus["InventoryMenu"]) {
			InventoryMenuEx::InstallHook();
		}
		if (settings->unpausedMenus["Journal Menu"]) {
			JournalMenuEx::InstallHook();
		}

		DrawNextFrameEx::InstallHook();
		PapyrusEx::InstallHook();
		MenuOpenHandlerEx::InstallHook();
	}
}
