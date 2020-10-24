#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	class SlowMotionHandler : public RE::BSTEventSink<RE::MenuOpenCloseEvent>
	{
	public:
		static bool isInSlowMotion;
		static float currentSlowMotionMultiplier;

		static void EnableSlowMotion();
		static void DisableSlowMotion();

		RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_dispatcher) override;
	};

	extern SlowMotionHandler g_slowMotionHandler;
}
