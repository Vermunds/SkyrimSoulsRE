#include "Menus/Hooks_BookMenu.h"

namespace SkyrimSoulsRE
{
	void BookMenuEx::AdvanceMovie_Hook(float a_interval, std::uint32_t a_currentTime)
	{
		class AdvanceBookMenuTask : public UnpausedTask
		{
			float interval;
			std::uint32_t currentTime;

		public:

			AdvanceBookMenuTask(float a_interval, std::uint32_t a_currentTime)
			{
				this->interval = a_interval;
				this->currentTime = a_currentTime;
			}

			void Run() override
			{
				RE::UI* ui = RE::UI::GetSingleton();

				if (ui->IsMenuOpen(RE::BookMenu::MENU_NAME))
				{
					RE::BookMenu* menu = static_cast<RE::BookMenu*>(ui->GetMenu(RE::BookMenu::MENU_NAME).get());

					static_cast<BookMenuEx*>(menu)->_AdvanceMovie(menu, interval, currentTime);
				}
			}
		};

		UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
		queue->AddTask(new AdvanceBookMenuTask(a_interval, a_currentTime));

		AutoCloseManager* autoCloseManager = AutoCloseManager::GetSingleton();
		autoCloseManager->CheckAutoClose(RE::BookMenu::MENU_NAME);
	}

	RE::UI_MESSAGE_RESULTS BookMenuEx::ProcessMessage_Hook(RE::UIMessage& a_message)
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

				if (ui->IsMenuOpen(RE::BookMenu::MENU_NAME))
				{
					BookMenuEx* menu = static_cast<BookMenuEx*>(ui->GetMenu(RE::BookMenu::MENU_NAME).get());
					menu->_ProcessMessage(menu, *message);
				}
			}

			void Dispose() override
			{
				delete this->message;
				delete this;
			}
		};

		RE::UserEvents* userEvents = RE::UserEvents::GetSingleton();
		UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();

		switch (a_message.type.get())
		{
			case RE::UI_MESSAGE_TYPE::kUserEvent:
			{
				RE::BSUIMessageData* oldData = static_cast<RE::BSUIMessageData*>(a_message.data);

				if (oldData->fixedStr == userEvents->leftEquip || oldData->fixedStr == userEvents->rightEquip ||
					oldData->fixedStr == userEvents->prevPage || oldData->fixedStr == userEvents->nextPage ||
					oldData->fixedStr == userEvents->accept || oldData->fixedStr == userEvents->cancel)
				{
					RE::UIMessageQueue* uiMessageQueue = RE::UIMessageQueue::GetSingleton();
					RE::InterfaceStrings* interfaceStrings = RE::InterfaceStrings::GetSingleton();

					RE::UIMessage* message = new RE::UIMessage(a_message);
					RE::MessageDataFactoryManager* msgFactory = RE::MessageDataFactoryManager::GetSingleton();
					auto creator = msgFactory->GetCreator(interfaceStrings->bsUIMessageData);
					RE::BSUIMessageData* data = static_cast<RE::BSUIMessageData*>(creator->Create());

					data->data = oldData->data;
					data->str = oldData->str;
					data->fixedStr = oldData->fixedStr;

					message->data = data;

					ProcessMessageTask* task = new ProcessMessageTask(message);
					queue->AddTask(task);
					return RE::UI_MESSAGE_RESULTS::kHandled;
				}
				return RE::UI_MESSAGE_RESULTS::kIgnore;
			}
			case RE::UI_MESSAGE_TYPE::kUpdate:
			{
				RE::UIMessage* message = new RE::UIMessage(a_message);
				message->data = nullptr;

				ProcessMessageTask* task = new ProcessMessageTask(message);
				queue->AddTask(task);
				return RE::UI_MESSAGE_RESULTS::kHandled;
			}
			case RE::UI_MESSAGE_TYPE::kShow:
			{
				RE::UIMessage* message = new RE::UIMessage(a_message);
				message->data = nullptr;

				ProcessMessageTask* task = new ProcessMessageTask(message);
				queue->AddTask(task);
				return RE::UI_MESSAGE_RESULTS::kHandled;
			}
			case RE::UI_MESSAGE_TYPE::kHide:
			{
				RE::UIMessage* message = new RE::UIMessage(a_message);
				message->data = nullptr;

				ProcessMessageTask* task = new ProcessMessageTask(message);
				queue->AddTask(task);

				if (this->closeMenu)
				{
					// close animation done
					return RE::UI_MESSAGE_RESULTS::kHandled;
				}
				else
				{
					// close animation still playing
					return RE::UI_MESSAGE_RESULTS::kIgnore;
				}
			}
		}
		return _ProcessMessage(this, a_message);
	}

	RE::IMenu* BookMenuEx::Creator()
	{
		RE::BookMenu* menu = static_cast<RE::BookMenu*>(CreateMenu(RE::BookMenu::MENU_NAME));

		AutoCloseManager* autoCloseManager = AutoCloseManager::GetSingleton();
		autoCloseManager->InitAutoClose(RE::BookMenu::MENU_NAME, menu->GetTargetReference(), false);

		return menu;
	}

	void BookMenuEx::InstallHook()
	{
		//Hook AdvanceMovie
		REL::Relocation<std::uintptr_t> vTable(Offsets::Menus::BookMenu::Vtbl);
		_ProcessMessage = vTable.write_vfunc(0x4, &BookMenuEx::ProcessMessage_Hook);
		_AdvanceMovie = vTable.write_vfunc(0x5, &BookMenuEx::AdvanceMovie_Hook);

		//Fix for book menu not appearing
		//REL::safe_write(Offsets::BookMenu_Hook.address() + 0x5E, std::uint16_t(0x9090));
	}
}