#pragma once

#include <string>
#include <vector>

namespace SkyrimSoulsRE
{
	class Setting
	{
	protected:
		const char * name;
		UInt32 value = 0;
	public:
		Setting(const char * a_name, UInt32 a_value);
		const char * GetName();
		UInt32 GetValue();
		void SetValue(UInt32 a_value);
	};

	class SettingStore
	{
	private:
		static SettingStore * singleton;
		SettingStore();
	public:
		std::vector<Setting*> settings;

		UInt32 GetSetting(std::string a_menuName);

		void AddSetting(Setting* a_setting);

		static SettingStore* GetSingleton();

	};

	extern void LoadSettings();
}
