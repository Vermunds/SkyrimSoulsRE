#include "MenuProcessing.h"
#include "HookUtils.h"
#include "MenuFlagHandler.h"
#include "Offsets.h"
#include "SlowMotionHandler.h"

namespace SkyrimSoulsRE::MenuProcessing
{
	namespace
	{
		using ProcessMessages_t = void (RE::UI::*)();
		REL::Relocation<ProcessMessages_t> _ProcessMessages;

		using AdvanceMovies_t = void (RE::UI::*)();
		REL::Relocation<AdvanceMovies_t> _AdvanceMenus;

		using GetExecuteConsoleCommandsSingleton_t = void* (*)();
		REL::Relocation<GetExecuteConsoleCommandsSingleton_t> _GetExecuteConsoleCommandsSingleton;

		using ExecuteConsoleCommands_t = void (*)(void*);
		REL::Relocation<ExecuteConsoleCommands_t> _ExecuteConsoleCommands;

		// Main thread after rendering - job tasks should not be running during this
		// Ideally ProcessMessages should be called before rendering, but that is very difficult to properly synchronize with the still running job threads
		void MainThread_Hook(RE::ScrapHeap* a_this)
		{
			// Original
			++a_this->keepPagesRequest;

			// Add
			RE::UI* ui = RE::UI::GetSingleton();

			MenuFlagHandler::GetSingleton()->Update();

			if (!ui->GameIsPaused())
			{
				_ProcessMessages(ui);
				_AdvanceMenus(ui);

				void* executeConsoleCommandsSingleton = _GetExecuteConsoleCommandsSingleton();
				_ExecuteConsoleCommands(executeConsoleCommandsSingleton);

				SlowMotionHandler::GetSingleton()->Update();
			}
		}
	}

	void InstallHook()
	{
		_ProcessMessages = Offsets::UI::ProcessMessages.address();
		_AdvanceMenus = Offsets::UI::AdvanceMovies.address();
		_GetExecuteConsoleCommandsSingleton = Offsets::Misc::GetExecuteConsoleCommandsSingleton.address();
		_ExecuteConsoleCommands = Offsets::Misc::ExecuteConsoleCommands.address();

		// Disable UI job
		HookUtils::SafeWrite(Offsets::Job::UI.address() + 0xB, std::uint8_t(0xEB));

		// Hook UI processing
		HookUtils::WriteCall<5>(Offsets::Main::Update.address() + 0xADF, (uintptr_t)MainThread_Hook);
	}
}
