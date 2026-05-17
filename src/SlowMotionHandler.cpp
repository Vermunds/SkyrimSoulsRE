#include "SkyrimSoulsRE.h"

#include "SlowMotionHandler.h"

namespace SkyrimSoulsRE
{

	void SlowMotionHandler::EnableSlowMotion()
	{
		Settings* settings = Settings::GetSingleton();

		static float* timescaleMult1 = reinterpret_cast<float*>(Offsets::GlobalTimescaleMultiplier::Value1.address());
		static float* timescaleMult2 = reinterpret_cast<float*>(Offsets::GlobalTimescaleMultiplier::Value2.address());

		float multiplier = std::clamp(settings->slowMotionMultiplier, 0.01f, 1.0f);

		isInSlowMotion = true;
		currentSlowMotionMultiplier = multiplier;
		*timescaleMult1 = multiplier * (*timescaleMult1);
		*timescaleMult2 = *timescaleMult1;
	}

	void SlowMotionHandler::DisableSlowMotion()
	{
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

	void SlowMotionHandler::Update()
	{
		std::uint32_t slowMotionCount = GetSlowMotionCount();

		bool shouldBeActive = slowMotionCount > 0;
		if (shouldBeActive && Settings::GetSingleton()->slowMotionCombatOnly)
		{
			RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
			shouldBeActive = player && player->IsInCombat();
		}

		if (!isInSlowMotion && shouldBeActive)
		{
			EnableSlowMotion();
		}
		else if (isInSlowMotion && !shouldBeActive)
		{
			DisableSlowMotion();
		}
	}

	SlowMotionHandler* SlowMotionHandler::GetSingleton()
	{
		static SlowMotionHandler slowMotionHandler;
		return &slowMotionHandler;
	}
}
