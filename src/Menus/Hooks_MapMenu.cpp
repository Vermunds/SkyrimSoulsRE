#include "Menus/Hooks_MapMenu.h"

namespace SkyrimSoulsRE
{
	RE::UI_MESSAGE_RESULTS MapMenuEx::ProcessMessage_Hook(RE::UIMessage& a_message)
	{
		class ProcessMessageTask : public UnpausedTask
		{
			RE::UIMessage* message;
		public:
			ProcessMessageTask(RE::UIMessage* a_message)
			{
				this->message = a_message;
				this->usesDelay = false;
			}

			void Run() override
			{
				RE::UI* ui = RE::UI::GetSingleton();

				if (ui->IsMenuOpen(RE::MapMenu::MENU_NAME))
				{
					MapMenuEx* menu = static_cast<MapMenuEx*>(ui->GetMenu(RE::MapMenu::MENU_NAME).get());
					menu->_ProcessMessage(menu, *message);

					RE::UIMessageQueue* msgQueue = RE::UIMessageQueue::GetSingleton();
					msgQueue->AddMessage(RE::MapMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kHide, nullptr);
				}
			}

			void Dispose() override
			{
				delete this->message;
				delete this;
			}
		};

		// Fix for disappearing first person model after closing menu
		if (a_message.type == RE::UI_MESSAGE_TYPE::kHide)
		{
			if (closeMenu)
			{
				closeMenu = false;
				RE::PlayerControls* pc = RE::PlayerControls::GetSingleton();
				if (pc->autoMoveHandler->IsInputEventHandlingEnabled() && RE::ControlMap::GetSingleton()->IsMovementControlsEnabled())
				{
					pc->data.autoMove = restoreAutoMove;
				}
				restoreAutoMove = false;
				return RE::UI_MESSAGE_RESULTS::kHandled;
			}

			closeMenu = true;

			RE::UIMessage* message = new RE::UIMessage(a_message);
			if (message->data)
			{
				// Fast traveling
				restoreAutoMove = false;
				RE::BSUIMessageData* oldData = static_cast<RE::BSUIMessageData*>(message->data);

				RE::InterfaceStrings* interfaceStrings = RE::InterfaceStrings::GetSingleton();
				RE::MessageDataFactoryManager* msgFactory = RE::MessageDataFactoryManager::GetSingleton();

				auto creator = msgFactory->GetCreator(interfaceStrings->bsUIMessageData);
				RE::BSUIMessageData* newData = static_cast<RE::BSUIMessageData*>(creator->Create());
				newData->data = oldData->data;

				message->data = newData;
			}

			ProcessMessageTask* task = new ProcessMessageTask(message);

			UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
			queue->AddTask(task);

			return RE::UI_MESSAGE_RESULTS::kIgnore;
		}

		return _ProcessMessage(this, a_message);
	}

	void MapMenuEx::AdvanceMovie_Hook(float a_interval, std::uint32_t a_currentTime)
	{
		class AdvanceMapMenuTask : public UnpausedTask
		{
			float interval;
			std::uint32_t currentTime;

		public:

			AdvanceMapMenuTask(float a_interval, std::uint32_t a_currentTime)
			{
				this->interval = a_interval;
				this->currentTime = a_currentTime;
				this->usesDelay = false;
			}

			void Run() override
			{
				RE::UI* ui = RE::UI::GetSingleton();

				if (ui->IsMenuOpen(RE::MapMenu::MENU_NAME))
				{
					MapMenuEx* menu = static_cast<MapMenuEx*>(ui->GetMenu(RE::MapMenu::MENU_NAME).get());

					menu->UpdateClock();
					menu->_AdvanceMovie(menu, interval, currentTime);
				}
			}
		};

		UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
		queue->AddTask(new AdvanceMapMenuTask(a_interval, a_currentTime));	
	}

	void MapMenuEx::UpdateClock()
	{
		char timeDateString[200];
		RE::Calendar::GetSingleton()->GetTimeDateString(timeDateString, 200, true);

		RE::GFxValue dateText;
		if (this->uiMovie->GetVariable(&dateText, "_root.bottomBar.DateText")) //SkyUI
		{
			RE::GFxValue newDate(timeDateString);
			dateText.SetMember("htmlText", newDate);
		}
		else if (this->uiMovie->GetVariable(&dateText, "_root.Bottom.DateText")) // non-SkyUI
		{
			RE::GFxValue newDate(timeDateString);
			dateText.SetMember("htmlText", newDate);
		}
	}

	RE::IMenu* MapMenuEx::Creator()
	{
		RE::MapMenu* menu = static_cast<RE::MapMenu*>(CreateMenu(RE::MapMenu::MENU_NAME));	
		RE::PlayerControls* pc = RE::PlayerControls::GetSingleton();
		restoreAutoMove = pc->data.autoMove;
		pc->data.autoMove = false;
		return menu;
	}

	void MapMenuEx::InstallHook()
	{
		REL::Relocation<std::uintptr_t> vTable(Offsets::Menus::MapMenu::Vtbl);
		_ProcessMessage = vTable.write_vfunc(0x4, &MapMenuEx::ProcessMessage_Hook);
		_AdvanceMovie = vTable.write_vfunc(0x5, &MapMenuEx::AdvanceMovie_Hook);

		// Prevent setting kFreezeFrameBackground flag when opening local map
		REL::safe_write(Offsets::Menus::MapMenu::LocalMapUpdaterFunc.address() + 0x53, std::uint32_t(0x90909090));
		REL::safe_write(Offsets::Menus::MapMenu::LocalMapUpdaterFunc.address() + 0x9D, std::uint16_t(0x9090));
		REL::safe_write(Offsets::Menus::MapMenu::LocalMapUpdaterFunc.address() + 0x9F, std::uint8_t(0x90));

		// Disable map menu background sound
		REL::safe_write(Offsets::Menus::MapMenu::Ctor.address() + 0x538, std::uint8_t(0xEB));
	}
}
