#include "Settings.h"
#include "Utility.h"

namespace SkyrimSoulsRE
{

	Setting::Setting(std::string a_name, SInt32 a_value)
	{
		this->name = a_name;
		this->value.INT = a_value;
	}

	Setting::Setting(std::string a_name, float a_value)
	{
		this->name = a_name;
		this->value.FLOAT = a_value;
	}

	Setting::Setting(std::string a_name, bool a_value)
	{
		this->name = a_name;
		this->value.BOOL = a_value;
	}

	std::string Setting::GetName()
	{
		return this->name;
	}

	SInt32 Setting::GetInt()
	{
		return this->value.INT;
	}

	float Setting::GetFloat()
	{
		return this->value.FLOAT;
	}

	bool Setting::GetBool()
	{
		return this->value.BOOL;
	}

	void Setting::SetValue(SInt32 a_value)
	{
		this->value.INT = a_value;
	}

	void Setting::SetValue(float a_value)
	{
		this->value.FLOAT = a_value;
	}

	void Setting::SetValue(bool a_value)
	{
		this->value.BOOL = a_value;
	}

	Setting* SettingStore::GetSetting(std::vector<Setting*> &a_category, std::string a_setting)
	{
		for (auto &setting : a_category)
		{
			if (setting->GetName() == a_setting)
			{
				return setting;
			}
		}
		return nullptr;
	}

	void SettingStore::AddSetting(std::vector<Setting*> &a_category, Setting* a_setting)
	{
		if (this->GetSetting(a_category, a_setting->GetName()))
		{
			_ERROR("Setting already exists, overwriting: %s", a_setting->GetName());
		}
		a_category.emplace_back(a_setting);
	}

	SettingStore* SettingStore::singleton = nullptr;

	SettingStore::SettingStore()
	{

	}

	SettingStore* SettingStore::GetSingleton()
	{
		if (singleton)
		{
			return singleton;
		}
		singleton = new SettingStore();
		return singleton;
	}

	SInt32 GetIniInt(const char* a_section, const char* a_key, SInt32 a_defaultValue)
	{
		const char* iniFile = ".\\Data\\SKSE\\Plugins\\SkyrimSoulsRE.ini";
		char buffer[200];
		SInt32 result;

		GetPrivateProfileString(a_section, a_key, std::to_string(a_defaultValue).c_str(), buffer, 200, iniFile);
		if (GetLastError() == 0x2 || !(StrToInt(result, buffer)))
		{
			WritePrivateProfileStringA(a_section, a_key, std::to_string(a_defaultValue).c_str(), iniFile);
			return a_defaultValue;
		}
		return result;
	}

	bool GetIniBool(const char* a_section, const char* a_key, bool a_defaultValue)
	{
		return static_cast<bool>(GetIniInt(a_section, a_key, static_cast<SInt32>(a_defaultValue)));
	}

	float GetIniFloat(const char* a_section, const char* a_key, float a_defaultValue)
	{
		const char* iniFile = ".\\Data\\SKSE\\Plugins\\SkyrimSoulsRE.ini";
		char buffer[200];
		float result;

		GetPrivateProfileString(a_section, a_key, std::to_string(a_defaultValue).c_str(), buffer, 200, iniFile);
		if (GetLastError() == 0x2)
		{
			WritePrivateProfileStringA(a_section, a_key, std::to_string(a_defaultValue).c_str(), iniFile);
			return a_defaultValue;
		}
		try
		{
			result = std::atof(buffer);
		}
		catch (std::invalid_argument)
		{
			WritePrivateProfileStringA(a_section, a_key, std::to_string(a_defaultValue).c_str(), iniFile);
			return a_defaultValue;
		}
		catch (std::out_of_range)
		{
			WritePrivateProfileStringA(a_section, a_key, std::to_string(a_defaultValue).c_str(), iniFile);
			return a_defaultValue;
		}
		return result;
	}


	void LoadSettings()
	{
		SettingStore* settings = SettingStore::GetSingleton();

		//Unpaused menus
		settings->AddSetting(settings->unpausedMenus, new Setting("tweenMenu", GetIniBool("UNPAUSED_MENUS", "bTweenMenu", true)));
		settings->AddSetting(settings->unpausedMenus, new Setting("inventoryMenu", GetIniBool("UNPAUSED_MENUS", "bInventoryMenu", true)));
		settings->AddSetting(settings->unpausedMenus, new Setting("magicMenu", GetIniBool("UNPAUSED_MENUS", "bMagicMenu", true)));
		settings->AddSetting(settings->unpausedMenus, new Setting("barterMenu", GetIniBool("UNPAUSED_MENUS", "bBarterMenu", true)));
		settings->AddSetting(settings->unpausedMenus, new Setting("containerMenu", GetIniBool("UNPAUSED_MENUS", "bContainerMenu", true)));
		settings->AddSetting(settings->unpausedMenus, new Setting("giftMenu", GetIniBool("UNPAUSED_MENUS", "bGiftMenu", true)));
		settings->AddSetting(settings->unpausedMenus, new Setting("favoritesMenu", GetIniBool("UNPAUSED_MENUS", "bFavoritesMenu", true)));
		settings->AddSetting(settings->unpausedMenus, new Setting("tutorialMenu", GetIniBool("UNPAUSED_MENUS", "bTutorialMenu", true)));
		settings->AddSetting(settings->unpausedMenus, new Setting("lockpickingMenu", GetIniBool("UNPAUSED_MENUS", "bLockpickingMenu", true)));
		settings->AddSetting(settings->unpausedMenus, new Setting("bookMenu", GetIniBool("UNPAUSED_MENUS", "bBookMenu", true)));
		settings->AddSetting(settings->unpausedMenus, new Setting("messageBoxMenu", GetIniBool("UNPAUSED_MENUS", "bMessageBoxMenu", true)));
		settings->AddSetting(settings->unpausedMenus, new Setting("trainingMenu", GetIniBool("UNPAUSED_MENUS", "bTrainingMenu", true)));
		settings->AddSetting(settings->unpausedMenus, new Setting("journalMenu", GetIniBool("UNPAUSED_MENUS", "bJournalMenu", true)));
		settings->AddSetting(settings->unpausedMenus, new Setting("sleepWaitMenu", GetIniBool("UNPAUSED_MENUS", "bSleepWaitMenu", true)));
		//settings->AddSetting(settings->unpausedMenus, new Setting("sleepWaitMenu", GetIniValue("UNPAUSED_MENUS", "bMapMenu", "1")));
		//settings->AddSetting(settings->unpausedMenus, new Setting("sleepWaitMenu", GetIniValue("UNPAUSED_MENUS", "bStatsMenu", "1")));
		settings->AddSetting(settings->unpausedMenus, new Setting("console", GetIniBool("UNPAUSED_MENUS", "bConsole", true)));
		settings->AddSetting(settings->unpausedMenus, new Setting("bethesdaModMenu", GetIniBool("UNPAUSED_MENUS", "bBethesdaModMenu", true)));
		settings->AddSetting(settings->unpausedMenus, new Setting("customMenu", GetIniBool("UNPAUSED_MENUS", "bCustomMenu", true)));

		//Auto-close
		settings->AddSetting(settings->autoClose, new Setting("bAutoClose", GetIniBool("AUTOCLOSE", "bAutoCloseMenus", true)));
		settings->AddSetting(settings->autoClose, new Setting("uAutoCloseDistance", GetIniInt("AUTOCLOSE", "uAutoCloseDistance", 350)));

		//Controls
		settings->AddSetting(settings->controls, new Setting("bAutoCenterCursor", GetIniBool("CONTROLS", "bAutoCenterCursor", true)));
		settings->AddSetting(settings->controls, new Setting("bEnableMovementInMenus", GetIniBool("CONTROLS", "bEnableMovementInMenus", false)));
		settings->AddSetting(settings->controls, new Setting("bEnableGamepadCameraMove", GetIniBool("CONTROLS", "bEnableGamepadCameraMove", false)));
		settings->AddSetting(settings->controls, new Setting("bEnableCursorCameraMove", GetIniBool("CONTROLS", "bEnableCursorCameraMove", false)));
		settings->AddSetting(settings->controls, new Setting("fCursorCameraVerticalSpeed", GetIniFloat("CONTROLS", "fCursorCameraVerticalSpeed", 0.15)));
		settings->AddSetting(settings->controls, new Setting("fCursorCameraHorizontalSpeed", GetIniFloat("CONTROLS", "fCursorCameraHorizontalSpeed", 0.20)));

		//Slowmotion
		settings->AddSetting(settings->slowMotion, new Setting("bEnableSlowMotion", GetIniBool("SLOWMOTION", "bEnableSlowMotion", false)));
		settings->AddSetting(settings->slowMotion, new Setting("uSlowMotionPercent", GetIniInt("SLOWMOTION", "uSlowMotionPercent", 100)));
	}
}
