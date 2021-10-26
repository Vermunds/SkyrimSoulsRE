#include "Menus/StatsMenuEx.h"
#include <xbyak/xbyak.h>

namespace SkyrimSoulsRE
{
	RE::UI_MESSAGE_RESULTS StatsMenuEx::ProcessMessage_Hook(RE::UIMessage& a_message)
	{
		if (a_message.type == RE::UI_MESSAGE_TYPE::kHide)
		{
			isSleeping = false;
		}

		return _ProcessMessage(this, a_message);
	}

	void StatsMenuEx::AdvanceMovie_Hook(float a_interval, std::uint32_t a_currentTime)
	{
		HUDMenuEx* hudMenu = static_cast<HUDMenuEx*>(RE::UI::GetSingleton()->GetMenu(RE::HUDMenu::MENU_NAME).get());
		if (hudMenu)
		{
			hudMenu->UpdateHUD();
		}

		return _AdvanceMovie(this, a_interval, a_currentTime);
	}

	RE::IMenu* StatsMenuEx::Creator()
	{
		StatsMenuEx* menu = static_cast<StatsMenuEx*>(CreateMenu(RE::StatsMenu::MENU_NAME));
		return menu;
	}

	void OpenStatsMenuAfterSleep_Hook(RE::UIMessageQueue* a_queue, const RE::BSFixedString& a_menuName, RE::UI_MESSAGE_TYPE a_type, RE::IUIMessageData* a_data)
	{
		using FlagBD9 = RE::PlayerCharacter::FlagBD9;

		RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
		if (player->unkBD9.all(FlagBD9::kIsSleeping))
		{
			StatsMenuEx::isSleeping = true;
		}

		return a_queue->AddMessage(a_menuName, a_type, a_data);
	}

	void StatsMenuEx::InstallHook()
	{
		SKSE::Trampoline& trampoline = SKSE::GetTrampoline();

		// Fix level up when sleeping using survival mode
		// We replace a tail-call jump with a normal call, so the rest of the function can execute as well (this will remove the sleeping flag).
		REL::safe_write(Offsets::Menus::StatsMenu::OpenStatsMenuAfterSleep_Hook.address() + 0x65, std::uint32_t(0x90909090));
		REL::safe_write(Offsets::Menus::StatsMenu::OpenStatsMenuAfterSleep_Hook.address() + 0x69, std::uint8_t(0x90));
		trampoline.write_call<5>(Offsets::Menus::StatsMenu::OpenStatsMenuAfterSleep_Hook.address() + 0x6A, OpenStatsMenuAfterSleep_Hook);

		// Make StatsMenu check our sleeping variable
		struct SleepCheck_Code : Xbyak::CodeGenerator
		{
			SleepCheck_Code(uintptr_t a_trueAddress, uintptr_t a_falseAddress)
			{
				Xbyak::Label trueAddress;
				Xbyak::Label falseAddress;
				Xbyak::Label falseLabel;

				mov(rcx, std::uintptr_t(&StatsMenuEx::isSleeping));
				mov(cl, byte[rcx]);
				cmp(cl, 1);
				jne(falseLabel);
				jmp(ptr[rip + trueAddress]);

				L(falseLabel);
				jmp(ptr[rip + falseAddress]);

				L(trueAddress);
				dq(a_trueAddress);

				L(falseAddress);
				dq(a_falseAddress);
			}
		};

		SleepCheck_Code code{ std::uintptr_t(Offsets::Menus::StatsMenu::ProcessMessage.address() + 0xF45), std::uintptr_t(Offsets::Menus::StatsMenu::ProcessMessage.address() + 0xFA9) };
		void* codeLoc = SKSE::GetTrampoline().allocate(code);
		trampoline.write_branch<5>(Offsets::Menus::StatsMenu::ProcessMessage.address() + 0xF3C, codeLoc);

		// Fix for menu not appearing
		REL::safe_write(Offsets::Menus::StatsMenu::ProcessMessage.address() + 0x929, std::uint32_t(0x90909090));
		REL::safe_write(Offsets::Menus::StatsMenu::ProcessMessage.address() + 0x92D, std::uint16_t(0x9090));

		// Prevent setting kFreezeFrameBackground flag
		REL::safe_write(Offsets::Menus::StatsMenu::ProcessMessage.address() + 0xAEC, std::uint32_t(0x90909090));

		// Keep the menu updated
		REL::safe_write(Offsets::Menus::StatsMenu::ProcessMessage.address() + 0xFBC, std::uint16_t(0x9090));

		// Fix for controls not working
		REL::safe_write(Offsets::Menus::StatsMenu::CanProcess.address() + 0x46, std::uint32_t(0x90909090));
		REL::safe_write(Offsets::Menus::StatsMenu::CanProcess.address() + 0x4A, std::uint16_t(0x9090));

		//Hook ProcessMessage and AdvanceMovie
		REL::Relocation<std::uintptr_t> vTable(Offsets::Menus::StatsMenu::Vtbl);
		_ProcessMessage = vTable.write_vfunc(0x4, &StatsMenuEx::ProcessMessage_Hook);
		_AdvanceMovie = vTable.write_vfunc(0x5, &StatsMenuEx::AdvanceMovie_Hook);
	}
}
