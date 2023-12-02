#include "Menus/BookMenuEx.h"

namespace SkyrimSoulsRE
{
	void BookMenuEx::AdvanceMovie_Hook(float a_interval, std::uint32_t a_currentTime)
	{
		UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
		queue->AddTask([a_interval, a_currentTime]() {
			RE::UI* ui = RE::UI::GetSingleton();
			RE::BSSpinLockGuard lk(ui->processMessagesLock);

			if (ui->IsMenuOpen(RE::BookMenu::MENU_NAME))
			{
				BookMenuEx* menu = static_cast<BookMenuEx*>(ui->GetMenu(RE::BookMenu::MENU_NAME).get());
				menu->_AdvanceMovie(menu, a_interval, a_currentTime);
				AutoCloseManager::GetSingleton()->CheckAutoClose(RE::BookMenu::MENU_NAME);
			}
		});
	}

	RE::UI_MESSAGE_RESULTS BookMenuEx::ProcessMessage_Hook(RE::UIMessage& a_message)
	{
		std::shared_ptr<RE::UIMessage> modifiedMessage = std::make_shared<RE::UIMessage>(a_message);

		auto task = [modifiedMessage]() {
			RE::UI* ui = RE::UI::GetSingleton();
			RE::BSSpinLockGuard lk(ui->processMessagesLock);

			if (ui->IsMenuOpen(RE::BookMenu::MENU_NAME))
			{
				BookMenuEx* menu = static_cast<BookMenuEx*>(ui->GetMenu(RE::BookMenu::MENU_NAME).get());
				menu->_ProcessMessage(menu, *modifiedMessage);
			}

			if (modifiedMessage->data)
			{
				RE::MessageDataFactoryManager* msgFactory = RE::MessageDataFactoryManager::GetSingleton();
				auto creator = msgFactory->GetCreator(RE::InterfaceStrings::GetSingleton()->bsUIMessageData);
				creator->Destroy(modifiedMessage->data);
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

					RE::MessageDataFactoryManager* msgFactory = RE::MessageDataFactoryManager::GetSingleton();
					auto creator = msgFactory->GetCreator(interfaceStrings->bsUIMessageData);
					RE::BSUIMessageData* data = static_cast<RE::BSUIMessageData*>(creator->Create());

					data->data = oldData->data;
					data->str = oldData->str;
					data->fixedStr = oldData->fixedStr;

					modifiedMessage->data = data;
					queue->AddTask(task);
					return RE::UI_MESSAGE_RESULTS::kHandled;
				}
				return RE::UI_MESSAGE_RESULTS::kIgnore;
			}
		case RE::UI_MESSAGE_TYPE::kUpdate:
		case RE::UI_MESSAGE_TYPE::kShow:
			{
				modifiedMessage->data = nullptr;
				queue->AddTask(task);
				return RE::UI_MESSAGE_RESULTS::kHandled;
			}
		case RE::UI_MESSAGE_TYPE::kHide:
			{
				modifiedMessage->data = nullptr;
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
		REL::Relocation<std::uintptr_t> vTable(RE::VTABLE_BookMenu[0]);
		_ProcessMessage = vTable.write_vfunc(0x4, &BookMenuEx::ProcessMessage_Hook);
		_AdvanceMovie = vTable.write_vfunc(0x5, &BookMenuEx::AdvanceMovie_Hook);
	}
}
