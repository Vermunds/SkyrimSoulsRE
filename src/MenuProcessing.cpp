#include "MenuProcessing.h"
#include "Offsets.h"

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
			// Call original
			using func_t = void (*)(RE::ScrapHeap*);
			REL::Relocation<func_t> func(Offsets::ScrapHeap::HookedFunc.address());
			func(a_this);

			// Add
			RE::UI* ui = RE::UI::GetSingleton();

			if (!ui->GameIsPaused())
			{
				_ProcessMessages(ui);
				_AdvanceMenus(ui);

				void* executeConsoleCommandsSingleton = _GetExecuteConsoleCommandsSingleton();
				_ExecuteConsoleCommands(executeConsoleCommandsSingleton);
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
		REL::safe_write(Offsets::Job::UI.address() + 0xB, std::uint8_t(0xEB));

		// Hook UI processing
		SKSE::GetTrampoline().write_call<5>(Offsets::Main::Update.address() + 0xADF, (uintptr_t)MainThread_Hook);
	}
}
