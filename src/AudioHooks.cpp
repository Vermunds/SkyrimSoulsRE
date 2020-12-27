#include "AudioHooks.h"

namespace SkyrimSoulsRE::Audio
{
	void QuaternionToMatrix(RE::NiPoint3& a_v1, RE::NiPoint3& a_v2, RE::NiQuaternion a_quaternion)
	{
		// https://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToMatrix/index.htm
		// ( - | v1.x | v2.x )
		// ( - | v1.y | v2.y )
		// ( - | v1.z | v2.z )

		a_v1.x = 2 * a_quaternion.x * a_quaternion.y - 2 * a_quaternion.z * a_quaternion.w;
		a_v1.y = 1 - 2 * a_quaternion.x * a_quaternion.x - 2 * a_quaternion.z * a_quaternion.z;
		a_v1.z = 2 * a_quaternion.y * a_quaternion.z + 2 * a_quaternion.x * a_quaternion.w;

		a_v2.x = 2 * a_quaternion.x * a_quaternion.z + 2 * a_quaternion.y * a_quaternion.w;
		a_v2.y = 2 * a_quaternion.y * a_quaternion.z - 2 * a_quaternion.x * a_quaternion.w;
		a_v2.z = 1 - 2 * a_quaternion.x * a_quaternion.x - 2 * a_quaternion.y * a_quaternion.y;
	}

	void SetListenerPosition_Hook(RE::BSAudioManager* a_audioManager, RE::NiPoint3* a_pos)
	{
		RE::UI* ui = RE::UI::GetSingleton();
		if (ui->IsMenuOpen(RE::MapMenu::MENU_NAME))
		{
			RE::PlayerCamera* camera = RE::PlayerCamera::GetSingleton();
			RE::TESCameraState* currentState = camera->currentState.get();
			currentState->GetTranslation(*a_pos);
		}

		using func_t = decltype(&SetListenerPosition_Hook);
		REL::Relocation<func_t> func{ Offsets::BSAudioManager::SetListenerPosition };
		return func(a_audioManager, a_pos);
	}

	void SetListenerRotation_Hook(RE::BSAudioManager* a_audioManager, RE::NiPoint3* a_unk1, RE::NiPoint3* a_unk2)
	{
		RE::UI* ui = RE::UI::GetSingleton();
		if (ui->IsMenuOpen(RE::MapMenu::MENU_NAME))
		{
			RE::PlayerCamera* camera = RE::PlayerCamera::GetSingleton();
			RE::TESCameraState* currentState = camera->currentState.get();

			RE::NiQuaternion rot;
			currentState->GetRotation(rot);
			QuaternionToMatrix(*a_unk1, *a_unk2, rot);
		}

		using func_t = decltype(&SetListenerRotation_Hook);
		REL::Relocation<func_t> func{ Offsets::BSAudioManager::SetListenerRotation };
		return func(a_audioManager, a_unk1, a_unk2);
	}

	void InstallHook()
	{
		SKSE::GetTrampoline().write_call<5>(Offsets::BSAudioManager::Hook.address() + 0xBC, (std::uintptr_t)SetListenerPosition_Hook);
		SKSE::GetTrampoline().write_call<5>(Offsets::BSAudioManager::Hook.address() + 0x124, (std::uintptr_t)SetListenerRotation_Hook);
	}
}
