#pragma once

namespace SkyrimSoulsRE
{
	class SlowMotionHandler
	{
	public:
		void Update();

		static SlowMotionHandler* GetSingleton();

	private:
		bool isInSlowMotion = false;
		float currentSlowMotionMultiplier = 1.0f;

		void EnableSlowMotion();
		void DisableSlowMotion();

		SlowMotionHandler() {};
		~SlowMotionHandler() {};
		SlowMotionHandler(const SlowMotionHandler&) = delete;
		SlowMotionHandler& operator=(const SlowMotionHandler&) = delete;
	};
}
