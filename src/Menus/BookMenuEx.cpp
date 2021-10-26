#include "Menus/BookMenuEx.h"

namespace SkyrimSoulsRE
{
	void BookMenuEx::AdvanceMovie_Hook(float a_interval, std::uint32_t a_currentTime)
	{
		class AdvanceBookMenuTask : public UnpausedTask
		{
		public:
			float interval;
			std::uint32_t currentTime;

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

		std::shared_ptr<AdvanceBookMenuTask> task = std::make_shared<AdvanceBookMenuTask>();
		task->interval = a_interval;
		task->currentTime = a_currentTime;

		UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
		queue->AddTask(task);

		AutoCloseManager::GetSingleton()->CheckAutoClose(RE::BookMenu::MENU_NAME);
	}

	RE::UI_MESSAGE_RESULTS BookMenuEx::ProcessMessage_Hook(RE::UIMessage& a_message)
	{
		class ProcessMessageTask : public UnpausedTask
		{
		public:
			RE::UIMessage message;

			void Run() override
			{
				RE::UI* ui = RE::UI::GetSingleton();

				if (ui->IsMenuOpen(RE::BookMenu::MENU_NAME))
				{
					BookMenuEx* menu = static_cast<BookMenuEx*>(ui->GetMenu(RE::BookMenu::MENU_NAME).get());
					menu->_ProcessMessage(menu, message);
				}
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
					RE::InterfaceStrings* interfaceStrings = RE::InterfaceStrings::GetSingleton();

					std::shared_ptr<ProcessMessageTask> task = std::make_shared<ProcessMessageTask>();
					task->message = RE::UIMessage{ a_message };

					RE::MessageDataFactoryManager* msgFactory = RE::MessageDataFactoryManager::GetSingleton();
					auto creator = msgFactory->GetCreator(interfaceStrings->bsUIMessageData);
					RE::BSUIMessageData* data = static_cast<RE::BSUIMessageData*>(creator->Create());

					data->data = oldData->data;
					data->str = oldData->str;
					data->fixedStr = oldData->fixedStr;

					task->message.data = data;

					queue->AddTask(task);
					return RE::UI_MESSAGE_RESULTS::kHandled;
				}
				return RE::UI_MESSAGE_RESULTS::kIgnore;
			}
		case RE::UI_MESSAGE_TYPE::kUpdate:
		case RE::UI_MESSAGE_TYPE::kShow:
			{
				std::shared_ptr<ProcessMessageTask> task = std::make_shared<ProcessMessageTask>();

				task->message = RE::UIMessage{ a_message };
				task->message.data = nullptr;

				queue->AddTask(task);
				return RE::UI_MESSAGE_RESULTS::kHandled;
			}
		case RE::UI_MESSAGE_TYPE::kHide:
			{
				std::shared_ptr<ProcessMessageTask> task = std::make_shared<ProcessMessageTask>();

				task->message = RE::UIMessage{ a_message };
				task->message.data = nullptr;

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
	}
}
