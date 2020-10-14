#include "SlowMotionHandler.h"

namespace SkyrimSoulsRE
{
	bool SlowMotionHandler::isInSlowMotion = false;
	float SlowMotionHandler::currentSlowMotionMultiplier = 1.0f;

	void SlowMotionHandler::EnableSlowMotion()
	{
		Settings* settings = Settings::GetSingleton();

		float slowMotionMultiplier = settings->slowMotionMultiplier;
		float* timescaleMult1 = reinterpret_cast<float*>(Offsets::GlobalTimescaleMultiplier::Value1.address());
		float* timescaleMult2 = reinterpret_cast<float*>(Offsets::GlobalTimescaleMultiplier::Value2.address());

		if (settings->enableSlowMotion)
		{

			float multiplier;
			if (slowMotionMultiplier >= 0.1f && 1.0f >= slowMotionMultiplier)
			{
				multiplier = slowMotionMultiplier;
			}
			else {
				multiplier = 1.0f;
			}

			isInSlowMotion = true;
			currentSlowMotionMultiplier = settings->slowMotionMultiplier;
			*timescaleMult1 = multiplier * (*timescaleMult1);
			*timescaleMult2 = *timescaleMult1;
		}
	}

	void SlowMotionHandler::DisableSlowMotion()
	{
		float slowMotionMultiplier = currentSlowMotionMultiplier;
		float* timescaleMult1 = reinterpret_cast<float*>(Offsets::GlobalTimescaleMultiplier::Value1.address());
		float* timescaleMult2 = reinterpret_cast<float*>(Offsets::GlobalTimescaleMultiplier::Value2.address());

		if (isInSlowMotion)
		{
			*timescaleMult1 = (1.0f / currentSlowMotionMultiplier) * (*timescaleMult1);

			//Safety check
			if (*timescaleMult1 > 1.0f)
			{
				*timescaleMult1 = 1.0f;
			}

			*timescaleMult2 = *timescaleMult1;

			isInSlowMotion = false;
		}
	}

	RE::BSEventNotifyControl SlowMotionHandler::ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_dispatcher)
	{
		std::uint32_t unpausedMenuCount = GetUnpausedMenuCount();
		if (!isInSlowMotion && unpausedMenuCount)
		{
			EnableSlowMotion();
		}
		else if (isInSlowMotion && !unpausedMenuCount)
		{
			DisableSlowMotion();
		}

		return RE::BSEventNotifyControl::kContinue;
	}

	SlowMotionHandler g_slowMotionHandler;
}
