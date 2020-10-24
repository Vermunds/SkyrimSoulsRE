#include "Menus/Hooks_MapMenu.h"

namespace SkyrimSoulsRE
{
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
			}

			void Run() override
			{
				RE::UI* ui = RE::UI::GetSingleton();

				if (ui->IsMenuOpen(RE::MapMenu::MENU_NAME))
				{
					RE::MapMenu* menu = static_cast<RE::MapMenu*>(ui->GetMenu(RE::MapMenu::MENU_NAME).get());

					static_cast<MapMenuEx*>(menu)->_AdvanceMovie(menu, interval, currentTime);
				}
			}
		};

		UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
		queue->AddTask(new AdvanceMapMenuTask(a_interval, a_currentTime));
	}

	RE::IMenu* MapMenuEx::Creator()
	{
		return CreateMenu(RE::MapMenu::MENU_NAME);
	}

	void MapMenuEx::InstallHook()
	{
		REL::Relocation<std::uintptr_t> vTable(Offsets::Menus::MapMenu::Vtbl);
		_AdvanceMovie = vTable.write_vfunc(0x5, &MapMenuEx::AdvanceMovie_Hook);

		// Prevent setting kFreezeFrameBackground flag when opening local map
		REL::safe_write(Offsets::Menus::MapMenu::LocalMapUpdaterFunc.address() + 0x53, std::uint32_t(0x90909090));
		REL::safe_write(Offsets::Menus::MapMenu::LocalMapUpdaterFunc.address() + 0x9D, std::uint16_t(0x9090));
		REL::safe_write(Offsets::Menus::MapMenu::LocalMapUpdaterFunc.address() + 0x9F, std::uint8_t(0x90));
	}

}
