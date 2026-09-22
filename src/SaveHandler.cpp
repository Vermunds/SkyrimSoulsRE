#include "SaveHandler.h"

#include "HookUtils.h"
#include "Offsets.h"

#include <xbyak/xbyak.h>

namespace SkyrimSoulsRE
{
	SaveHandler* SaveHandler::GetSingleton()
	{
		static SaveHandler singleton;
		return &singleton;
	}

	bool SaveHandler::SaveGame_Hook(RE::BGSSaveLoadManager* a_this, std::uint32_t, std::uint32_t, const char* a_fileName)
	{
		SaveHandler* saveHandler = SaveHandler::GetSingleton();

		if (saveHandler->m_isSaving)
		{
			return false;
		}

		saveHandler->m_currentFileName = a_fileName ? a_fileName : "";
		saveHandler->m_isSaving = true;

		using func_t = void (*)(RE::BGSSaveLoadManager*, std::uint32_t);
		REL::Relocation<func_t> RequestSave(Offsets::BGSSaveLoadManager::RequestSave);
		RequestSave(a_this, 0xF0004000);

		return true;
	}

	void SaveHandler::BGSSaveLoadManager_ProcessEvents_Hook()
	{
		SaveHandler* saveHandler = SaveHandler::GetSingleton();
		if (!saveHandler->m_isSaving)
		{
			return;
		}

		const char* fileName = saveHandler->m_currentFileName.empty() ? nullptr : saveHandler->m_currentFileName.c_str();
		_SaveGame(RE::BGSSaveLoadManager::GetSingleton(), 2, 0, fileName);

		saveHandler->m_isSaving = false;
	}

	void SaveHandler::InstallHook()
	{
		// Every user initiated save ends up in BGSSaveLoadManager::Save, so the call sites are hooked directly
		_SaveGame = HookUtils::WriteCall<5>(Offsets::Menus::Console::SaveGameHandler.address() + 0xC4, (std::uintptr_t)SaveGame_Hook);  // Save from console
		HookUtils::WriteBranch<5>(Offsets::UISaveLoadManager::SaveGame.address() + 0x2B, (std::uintptr_t)SaveGame_Hook);                // New save
		HookUtils::WriteCall<5>(Offsets::BGSSaveLoadFileEntry::Save.address() + 0x3E, (std::uintptr_t)SaveGame_Hook);                   // Overwriting an existing save
		HookUtils::WriteCall<5>(Offsets::BGSSaveLoadFileEntry::Save.address() + 0x60, (std::uintptr_t)SaveGame_Hook);                   // Overwriting an autosave/quicksave

		struct ProcessEvents_Code : Xbyak::CodeGenerator
		{
			ProcessEvents_Code(uintptr_t a_hookAddr, uintptr_t a_returnAddr, uintptr_t a_f0000800case)
			{
				Xbyak::Label hookAddress;
				Xbyak::Label f0000800case;
				Xbyak::Label returnAddr;
				Xbyak::Label notF0000800;

				cmp(ecx, 0xF0000800);
				jne(notF0000800);
				jmp(ptr[rip + f0000800case]);

				L(notF0000800);
				cmp(ecx, 0xF0004000);
				call(ptr[rip + hookAddress]);
				jmp(ptr[rip + returnAddr]);

				L(hookAddress);
				dq(a_hookAddr);

				L(f0000800case);
				dq(a_f0000800case);

				L(returnAddr);
				dq(a_returnAddr);
			}
		};

		ProcessEvents_Code code{
			std::uintptr_t(BGSSaveLoadManager_ProcessEvents_Hook),
			Offsets::BGSSaveLoadManager::ProcessEvents.address() + 0x72A,
			Offsets::BGSSaveLoadManager::ProcessEvents.address() + 0x5D1
		};
		void* codeLoc = SKSE::GetTrampoline().allocate(code);

		HookUtils::WriteBranch<6>(Offsets::BGSSaveLoadManager::ProcessEvents.address() + 0x5D5, codeLoc);
	}
}
