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
		// Journal Menu Remap Handler
		class RemapHandler : RE::BSTEventSink<RE::InputEvent*>
		{
		public:
			//todo - find a better way to do this
			class FakeButtonEvent
			{
			public:
				virtual ~FakeButtonEvent() = default;  // 00

				virtual bool HasIDCode() const { return true; };                            // 01
				virtual const RE::BSFixedString& QUserEvent() const { return userEvent; };  // 02

				// members
				RE::INPUT_DEVICE device;         // 08
				RE::INPUT_EVENT_TYPE eventType;  // 0C
				RE::InputEvent* next;            // 10
				RE::BSFixedString userEvent;     // 18
				std::uint32_t idCode;            // 20
				std::uint32_t pad24;             // 24
				float value;                     // 28
				float heldDownSecs;              // 2C
			};
			static_assert(sizeof(FakeButtonEvent) == sizeof(RE::ButtonEvent));

			RE::BSEventNotifyControl ProcessEvent_Hook(RE::InputEvent** a_event, RE::BSTEventSource<RE::InputEvent**>* a_eventSource);

			using ProcessEvent_t = decltype(&ProcessEvent_Hook);
			static inline REL::Relocation<ProcessEvent_t> _ProcessEvent;
		};

		static inline bool isSaving = false;

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
