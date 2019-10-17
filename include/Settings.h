#pragma once

#include <string>
#include <vector>

namespace SkyrimSoulsRE
{
	class Setting
	{
	protected:
		union Value
		{
			SInt32			INT;
			float			FLOAT;
			bool			BOOL;
		};

		std::string name;
		Value value;

	public:
		Setting(std::string a_name, SInt32 a_value);
		Setting(std::string a_name, float a_value);
		Setting(std::string a_name, bool a_value);

		std::string GetName();

		SInt32 GetInt();
		float GetFloat();
		bool GetBool();

		void SetValue(SInt32 a_value);
		void SetValue(float a_value);
		void SetValue(bool a_value);
	};

	class SettingStore
	{
	private:
		static SettingStore * singleton;
		SettingStore();
	public:

		std::vector<Setting*> unpausedMenus;
		std::vector<Setting*> autoClose;
		std::vector<Setting*> controls;
		std::vector<Setting*> slowMotion;

		Setting* GetSetting(std::vector<Setting*> &a_category, std::string a_setting);

		void AddSetting(std::vector<Setting*> &a_category, Setting* a_setting);

		static SettingStore* GetSingleton();

	};

	extern void LoadSettings();
}
