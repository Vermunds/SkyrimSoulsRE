#include "Settings.h"

namespace SkyrimSoulsRE
{

	Setting::Setting(const char * a_name, UInt32 a_value)
	{
		this->name = a_name;
		this->value = a_value;
	}

	const char * Setting::GetName()
	{
		return this->name;
	}

	UInt32 Setting::GetValue()
	{
		return this->value;
	}

	void Setting::SetValue(UInt32 a_value)
	{
		this->value = a_value;
	}

	UInt32 SettingStore::GetSetting(std::string a_name)
	{
		for (auto &setting : this->settings)
		{
			if (setting->GetName() == a_name)
			{
				return setting->GetValue();
			}
		}
		return 0;
	}

	void SettingStore::AddSetting(Setting* a_setting)
	{
		this->settings.emplace_back(a_setting);
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

	UInt32 GetIniValue(const char * a_section, const char * a_setting, UInt32 a_defaultValue, const char * a_inifile)
	{
		SInt32 value = GetPrivateProfileInt(a_section, a_setting, -1, a_inifile);
		if (value < 0)
		{
			std::string str = std::to_string(a_defaultValue);
			const char * cstr = str.c_str();
			WritePrivateProfileString(a_section, a_setting, cstr, a_inifile);
			return a_defaultValue;
		}
		return (UInt32)value;
	}

	void LoadSettings()
	{
		SettingStore * settingStore = SettingStore::GetSingleton();

		const char * iniFile = ".\\Data\\SKSE\\Plugins\\SkyrimSoulsRE.ini";

		//Unpaused menus
		settingStore->AddSetting(new Setting("tweenMenu", GetIniValue("UNPAUSED_MENUS", "bTweenMenu", 1, iniFile)));
		settingStore->AddSetting(new Setting("inventoryMenu", GetIniValue("UNPAUSED_MENUS", "bInventoryMenu", 1, iniFile)));
		settingStore->AddSetting(new Setting("magicMenu", GetIniValue("UNPAUSED_MENUS", "bMagicMenu", 1, iniFile)));
		settingStore->AddSetting(new Setting("barterMenu", GetIniValue("UNPAUSED_MENUS", "bBarterMenu", 1, iniFile)));
		settingStore->AddSetting(new Setting("containerMenu", GetIniValue("UNPAUSED_MENUS", "bContainerMenu", 1, iniFile)));
		settingStore->AddSetting(new Setting("giftMenu", GetIniValue("UNPAUSED_MENUS", "bGiftMenu", 1, iniFile)));
		settingStore->AddSetting(new Setting("favoritesMenu", GetIniValue("UNPAUSED_MENUS", "bFavoritesMenu", 1, iniFile)));
		settingStore->AddSetting(new Setting("tutorialMenu", GetIniValue("UNPAUSED_MENUS", "bTutorialMenu", 1, iniFile)));
		settingStore->AddSetting(new Setting("lockpickingMenu", GetIniValue("UNPAUSED_MENUS", "bLockpickingMenu", 1, iniFile)));
		settingStore->AddSetting(new Setting("bookMenu", GetIniValue("UNPAUSED_MENUS", "bBookMenu", 1, iniFile)));
		settingStore->AddSetting(new Setting("messageBoxMenu", GetIniValue("UNPAUSED_MENUS", "bMessageBoxMenu", 1, iniFile)));
		settingStore->AddSetting(new Setting("trainingMenu", GetIniValue("UNPAUSED_MENUS", "bTrainingMenu", 1, iniFile)));
		settingStore->AddSetting(new Setting("journalMenu", GetIniValue("UNPAUSED_MENUS", "bJournalMenu", 1, iniFile)));
		settingStore->AddSetting(new Setting("sleepWaitMenu", GetIniValue("UNPAUSED_MENUS", "bSleepWaitMenu", 1, iniFile)));
		//settingStore->AddSetting(new Setting("mapMenu", GetIniValue("UNPAUSED_MENUS", "bMapMenu", 1, iniFile)));
		//settingStore->AddSetting(new Setting("statsMenu", GetIniValue("UNPAUSED_MENUS", "bStatsMenu", 1, iniFile)));
		settingStore->AddSetting(new Setting("console", GetIniValue("UNPAUSED_MENUS", "bConsole", 1, iniFile)));
		settingStore->AddSetting(new Setting("bethesdaModMenu", GetIniValue("UNPAUSED_MENUS", "bBethesdaModMenu", 1, iniFile)));
		settingStore->AddSetting(new Setting("customMenu", GetIniValue("UNPAUSED_MENUS", "bCustomMenu", 1, iniFile)));

		//Auto-close
		settingStore->AddSetting(new Setting("bAutoClose", GetIniValue("AUTOCLOSE", "bAutoCloseMenus", 1, iniFile)));
		settingStore->AddSetting(new Setting("uAutoCloseDistance", GetIniValue("AUTOCLOSE", "uAutoCloseDistance", 350, iniFile)));

		//Controls
		settingStore->AddSetting(new Setting("bEnableMovementInMenus", GetIniValue("CONTROLS", "bEnableMovementInMenus", 0, iniFile)));

		//Slowmotion
		settingStore->AddSetting(new Setting("bEnableSlowMotion", GetIniValue("SLOWMOTION", "bEnableSlowMotion", 0, iniFile)));
		settingStore->AddSetting(new Setting("uSlowMotionPercent", GetIniValue("SLOWMOTION", "uSlowMotionPercent", 100, iniFile)));
	}
}
