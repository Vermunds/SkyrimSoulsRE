#pragma once

#include <vector>  // vector
#include <string>  // string

#include "json.hpp"  // json
#include "Utility.h"  // boolToString


namespace SkyrimSoulsRE
{
	class ISetting;
	static std::vector<ISetting*> settings;
	static std::vector<ISetting*> consoleSettings;

	class ISetting
	{
	private:
		typedef nlohmann::json json;

	public:
		ISetting(std::string a_key, bool a_consoleOK) : _key(a_key) { settings.push_back(this); if (a_consoleOK) consoleSettings.push_back(this); }
		virtual ~ISetting() {}

		virtual void				assign(bool a_val) {}
		virtual void				assign(int a_val) {}
		virtual void				assign(float a_val) {}
		virtual void				assign(const char* a_val) {}
		virtual void				assign(std::string a_val) {}
		virtual void				assign(json& a_val) {}
		virtual void				dump() = 0;
		virtual std::string			getValueAsString()			const = 0;
		inline const std::string&	key()						const { return _key; };

	protected:
		std::string	_key;
	};


	class bSetting : public ISetting
	{
	public:
		bSetting(std::string a_key, bool a_consoleOK, bool a_value) : ISetting(a_key, a_consoleOK), _value(a_value) {}
		virtual ~bSetting() {}

		virtual void		assign(bool a_val)			override { _value = a_val; }
		virtual void		assign(int a_val)			override { _value = a_val ? true : false; }
		virtual void		assign(float a_val)			override { _value = (int)a_val ? true : false; }
		virtual void		dump()						override { _DMESSAGE("%s: %s", _key.c_str(), boolToString(_value).c_str()); }
		virtual std::string	getValueAsString()	const	override { return _value ? "True" : "False"; }
		inline				operator bool()		const { return _value; }

	protected:
		bool _value;
	};


	class iSetting : public ISetting
	{
	public:
		iSetting(std::string a_key, bool a_consoleOK, SInt32 a_value) : ISetting(a_key, a_consoleOK), _value(a_value) {}
		virtual ~iSetting() {}

		virtual void		assign(int a_val)			override { _value = a_val; }
		virtual void		assign(float a_val)			override { _value = (int)a_val; }
		virtual void		dump()						override { _DMESSAGE("%s: %i", _key.c_str(), _value); }
		virtual std::string	getValueAsString()	const	override { return std::to_string(_value); }
		inline				operator SInt32()	const { return _value; }

	protected:
		SInt32 _value;
	};


	class fSetting : public ISetting
	{
	public:
		fSetting(std::string a_key, bool a_consoleOK, float a_value) : ISetting(a_key, a_consoleOK), _value(a_value) {}
		virtual ~fSetting() {}

		virtual void		assign(int a_val)			override { _value = (float)a_val; }
		virtual void		assign(float a_val)			override { _value = a_val; }
		virtual void		dump()						override { _DMESSAGE("%s: %f", _key.c_str(), _value); }
		virtual std::string	getValueAsString()	const	override { return std::to_string(_value); }
		inline				operator float()	const { return _value; }

	protected:
		float _value;
	};


	class sSetting : public ISetting
	{
	public:
		sSetting(std::string a_key, bool a_consoleOK, std::string a_value) : ISetting(a_key, a_consoleOK), _value(a_value) {}
		virtual ~sSetting() {}

		virtual void		assign(std::string a_val)											override { _value = a_val; }
		virtual void		assign(const char* a_val)											override { _value = a_val; }
		virtual void		dump()																override { _DMESSAGE("%s: %s", _key.c_str(), _value.c_str()); }
		virtual std::string	getValueAsString()											const	override { return _value; }
		inline const char*	c_str()														const { return _value.c_str(); }
		inline				operator std::string()										const { return _value; }
		inline				operator const char*()										const { return _value.c_str(); }
		inline friend bool	operator==(const sSetting& a_lhs, const sSetting& a_rhs) { return a_lhs._value == a_rhs._value; }
		inline friend bool	operator!=(const sSetting& a_lhs, const sSetting& a_rhs) { return !operator==(a_lhs, a_rhs); }
		inline friend bool	operator==(const sSetting& a_lhs, const std::string& a_rhs) { return a_lhs._value == a_rhs; }
		inline friend bool	operator!=(const sSetting& a_lhs, const std::string& a_rhs) { return !operator==(a_lhs, a_rhs); }
		inline friend bool	operator==(const std::string& a_lhs, const sSetting& a_rhs) { return  operator==(a_rhs, a_lhs); }
		inline friend bool	operator!=(const std::string& a_lhs, const sSetting& a_rhs) { return !operator==(a_lhs, a_rhs); }
		inline friend bool	operator==(const sSetting& a_lhs, const char* a_rhs) { return a_lhs._value.compare(a_rhs) == 0; }
		inline friend bool	operator!=(const sSetting& a_lhs, const char* a_rhs) { return !operator==(a_lhs, a_rhs); }
		inline friend bool	operator==(const char* a_lhs, const sSetting& a_rhs) { return  operator==(a_rhs, a_lhs); }
		inline friend bool	operator!=(const char* a_lhs, const sSetting& a_rhs) { return !operator==(a_rhs, a_lhs); }
		inline friend bool	operator< (const sSetting& a_lhs, const sSetting& a_rhs) { return a_lhs._value < a_rhs._value; }
		inline friend bool	operator> (const sSetting& a_lhs, const sSetting& a_rhs) { return  operator< (a_rhs, a_lhs); }
		inline friend bool	operator<=(const sSetting& a_lhs, const sSetting& a_rhs) { return !operator> (a_lhs, a_rhs); }
		inline friend bool	operator>=(const sSetting& a_lhs, const sSetting& a_rhs) { return !operator< (a_lhs, a_rhs); }

	protected:
		std::string _value;
	};


	template <typename T>
	class aSetting :
		public ISetting,
		public std::vector<T>
	{
		virtual ~aSetting() {}
	};


	template <>
	class aSetting<std::string> :
		public ISetting,
		public std::vector<std::string>
	{
	private:
		using json = nlohmann::json;

	public:
		aSetting(std::string a_key, bool a_consoleOK, std::initializer_list<std::string> a_list = {}) : ISetting(a_key, a_consoleOK), std::vector<std::string>(a_list) {}
		virtual ~aSetting() {}

		virtual void assign(json& a_val) override
		{
			clear();
			for (auto& val : a_val) {
				emplace_back(val.get<std::string>());
			}
		}

		virtual void dump() override
		{
			_DMESSAGE("%s:", _key.c_str());
			for (auto& it = begin(); it != end(); ++it) {
				_DMESSAGE("\t%s", it->c_str());
			}
		}

		virtual std::string	getValueAsString() const override
		{
			std::string str = _key + ":";
			for (auto& it = begin(); it != end(); ++it) {
				str += "\t" + *it + "\n";
			}
			return str;
		}
	};


	class Settings
	{
	public:
		static bool	loadSettings();
		static void	dump();


		static aSetting<std::string> unpausedMenus;

	private:
		Settings() {}
		~Settings() {}


		static const char* FILE_NAME;
	};
}
