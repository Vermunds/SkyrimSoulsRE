#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	// menuDepth = 5
	// flags = kPausesGame | kUsesMenuContext | kFreezeFrameBackground | kRequiresUpdate | kTopmostRenderedMenu | kUpdateUsesCursor | kAllowSaving
	// kDisablePauseMenu if game load prevented
	// context = kJournal
	class JournalMenuEx : public RE::JournalMenu
	{
	public:
		// Journal Menu Remap Handler hooks
		class RemapHandlerEx : RE::BSTEventSink<RE::InputEvent*>
		{
		public:
			// override (RE::BSTEventSink<RE::InputEvent*>)
			RE::BSEventNotifyControl ProcessEvent_Hook(RE::InputEvent** a_event, RE::BSTEventSource<RE::InputEvent**>* a_eventSource);

			using ProcessEvent_t = decltype(&ProcessEvent_Hook);
			static inline REL::Relocation<ProcessEvent_t> _ProcessEvent;
		};

		// Custom class to handle remapping inside MCM
		class MCMRemapHandler : public RE::GFxFunctionHandler, public RE::BSTEventSink<RE::InputEvent*>
		{
		public:
			// override (RE::BSTEventSink<RE::InputEvent*>)
			RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>* a_eventSource) override;

			// override (RE::GFxFunctionHandler)
			void Call(RE::GFxFunctionHandler::Params& a_args) override;

		private:
			RE::GFxValue scope;
		};

		// Custom class to handle game saves
		class SaveGameHandler : public RE::GFxFunctionHandler
		{
		public:
			// override (RE::GFxFunctionHandler)
			void Call(RE::GFxFunctionHandler::Params& params) override;
		};

		static inline bool isSaving = false;
		static inline MCMRemapHandler* mcmRemapHandler;
		static inline SaveGameHandler* saveGameHandler;

		// override (RE::IMenu)
		RE::UI_MESSAGE_RESULTS ProcessMessage_Hook(RE::UIMessage& a_message);   // 04
		void AdvanceMovie_Hook(float a_interval, std::uint32_t a_currentTime);  // 05

		void UpdateClock();

		static RE::IMenu* Creator();
		static void InstallHook();

		using AdvanceMovie_t = decltype(&RE::JournalMenu::AdvanceMovie);
		static inline REL::Relocation<AdvanceMovie_t> _AdvanceMovie;

		using ProcessMessage_t = decltype(&RE::JournalMenu::ProcessMessage);
		static inline REL::Relocation<ProcessMessage_t> _ProcessMessage;
	};
}
