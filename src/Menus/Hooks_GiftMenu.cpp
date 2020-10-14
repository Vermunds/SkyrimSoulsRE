#include "Menus/Hooks_GiftMenu.h"

namespace SkyrimSoulsRE
{
	void GiftMenuEx::AdvanceMovie_Hook(float a_interval, std::uint32_t a_currentTime)
	{
		this->UpdateBottomBar();
		return _AdvanceMovie(this, a_interval, a_currentTime);
	}

	void GiftMenuEx::ItemSelect_Hook(const RE::FxDelegateArgs& a_args)
	{
		class ItemSelectTask : public UnpausedTask
		{
			double	amount;
		public:
			ItemSelectTask(double a_amount)
			{
				this->amount = a_amount;
			}

			virtual void Run() override
			{
				RE::UI* ui = RE::UI::GetSingleton();

				if (ui->IsMenuOpen(RE::GiftMenu::MENU_NAME))
				{
					RE::IMenu* menu = ui->GetMenu(RE::GiftMenu::MENU_NAME).get();

					RE::GFxValue arg = this->amount;
					const RE::FxDelegateArgs args(0, menu, menu->uiMovie.get(), &arg, 1);
					_ItemSelect(args);
				}
			}
		};

		UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
		queue->AddTask(new ItemSelectTask(a_args[0].GetNumber()));
	}

	void GiftMenuEx::UpdateBottomBar()
	{
		using func_t = decltype(&GiftMenuEx::UpdateBottomBar);
		REL::Relocation<func_t> func(Offsets::Menus::GiftMenu::UpdateBottomBar);

		HUDMenuEx* hudMenu = static_cast<HUDMenuEx*>(RE::UI::GetSingleton()->GetMenu(RE::HUDMenu::MENU_NAME).get());
		if (hudMenu)
		{
			hudMenu->SetStealPickpocketHUDMode(true);
		}

		std::uint32_t* handle = reinterpret_cast<std::uint32_t*>(Offsets::Menus::GiftMenu::TargetRefHandle.address());
		RE::TESObjectREFRPtr refptr = nullptr;
		RE::TESObjectREFR* ref = nullptr;
		if (RE::TESObjectREFR::LookupByHandle(*handle, refptr))
		{
			ref = refptr.get();
		}
		else
		{
			SKSE::log::error("Failed to find Gift Menu target!");
		}

		AutoCloseManager* autoCloseManager = AutoCloseManager::GetSingleton();
		autoCloseManager->InitAutoClose(RE::ContainerMenu::MENU_NAME, ref, true);

		return func(this);
	}

	RE::IMenu* GiftMenuEx::Creator()
	{
		RE::IMenu* menu = static_cast<RE::GiftMenu*>(CreateMenu(RE::GiftMenu::MENU_NAME));

		RE::FxDelegate* dlg = menu->fxDelegate.get();
		_ItemSelect = dlg->callbacks.GetAlt("ItemSelect")->callback;
		dlg->callbacks.GetAlt("ItemSelect")->callback = ItemSelect_Hook;

		return menu;
	}

	void GiftMenuEx::InstallHook()
	{
		// Provides no difference right now - reserved for future use.

		//Hook AdvanceMovie
		//REL::Offset<std::uintptr_t> vTable(Offsets::IMenu::GiftMenu::Vtbl);
		//_AdvanceMovie = vTable.WriteVFunc(0x5, &GiftMenuEx::AdvanceMovie_Hook);
	}
}

