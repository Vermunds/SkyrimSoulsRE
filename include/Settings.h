#pragma once

#include <vector>  // vector
#include <string>  // string

#include "json.hpp"  // json
#include "Utility.h"  // boolToString


namespace SkyrimSoulsRE
{
	class ISetting
	{
	private:
		typedef nlohmann::json json;

	public:
		ISetting(std::string a_key) : _key(a_key) {}
		virtual ~ISetting() {}

		virtual void				Assign(bool a_val)					{}
		virtual void				Assign(int a_val)					{}
		virtual void				Assign(float a_val)					{}
		virtual void				Assign(const char* a_val)			{}
		virtual void				Assign(std::string a_val)			{}
		virtual void				Assign(json& a_val)					{}
		virtual void				Dump()								= 0;
		inline const std::string&	key()						const	{ return _key; };

	protected:
		std::string	_key;
	};


	static std::vector<ISetting*> settings;


	class bSetting : public ISetting
	{
	public:
		bSetting(std::string a_key, bool a_value) : ISetting(a_key), _value(a_value) { settings.push_back(this); }
		virtual ~bSetting() {}

		virtual void	Assign(bool a_val)			override	{ _value = a_val; }
		virtual void	Assign(int a_val)			override	{ _value = a_val ? true : false; }
		virtual void	Assign(float a_val)			override	{ _value = (int)a_val ? true : false; }
		virtual void	Dump()						override	{ _DMESSAGE("%s: %s", _key.c_str(), boolToString(_value).c_str()); }
		inline			operator bool()		const				{ return _value; }

	protected:
		bool _value;
	};


	class iSetting : public ISetting
	{
	public:
		iSetting(std::string a_key, SInt32 a_value) : ISetting(a_key), _value(a_value) { settings.push_back(this); }
		virtual ~iSetting() {}

		virtual void	Assign(int a_val)			override	{ _value = a_val; }
		virtual void	Assign(float a_val)			override	{ _value = (int)a_val; }
		virtual void	Dump()						override	{ _DMESSAGE("%s: %i", _key.c_str(), _value); }
		inline			operator SInt32()	const				{ return _value; }

	protected:
		SInt32 _value;
	};


	class fSetting : public ISetting
	{
	public:
		fSetting(std::string a_key, float a_value) : ISetting(a_key), _value(a_value) { settings.push_back(this); }
		virtual ~fSetting() {}

		virtual void	Assign(int a_val)			override	{ _value = (float)a_val; }
		virtual void	Assign(float a_val)			override	{ _value = a_val; }
		virtual void	Dump()						override	{ _DMESSAGE("%s: %f", _key.c_str(), _value); }
		inline			operator float()	const				{ return _value; }

	protected:
		float _value;
	};


	class sSetting : public ISetting
	{
	public:
		sSetting(std::string a_key, std::string a_value) : ISetting(a_key), _value(a_value) { settings.push_back(this); }
		virtual ~sSetting() {}

		virtual void		Assign(std::string a_val)											override	{ _value = a_val; }
		virtual void		Assign(const char* a_val)											override	{ _value = a_val; }
		virtual void		Dump()																override	{ _DMESSAGE("%s: %s", _key.c_str(), _value.c_str()); }
		inline const char*	c_str()														const				{ return _value.c_str(); }
		inline				operator std::string()										const				{ return _value; }
		inline				operator const char*()										const				{ return _value.c_str(); }
		inline friend bool	operator==(const sSetting& a_lhs, const sSetting& a_rhs)						{ return a_lhs._value == a_rhs._value; }
		inline friend bool	operator!=(const sSetting& a_lhs, const sSetting& a_rhs)						{ return !operator==(a_lhs, a_rhs); }
		inline friend bool	operator==(const sSetting& a_lhs, const std::string& a_rhs)						{ return a_lhs._value == a_rhs; }
		inline friend bool	operator!=(const sSetting& a_lhs, const std::string& a_rhs)						{ return !operator==(a_lhs, a_rhs); }
		inline friend bool	operator==(const std::string& a_lhs, const sSetting& a_rhs)						{ return  operator==(a_rhs, a_lhs); }
		inline friend bool	operator!=(const std::string& a_lhs, const sSetting& a_rhs)						{ return !operator==(a_lhs, a_rhs); }
		inline friend bool	operator==(const sSetting& a_lhs, const char* a_rhs)							{ return a_lhs._value.compare(a_rhs) == 0; }
		inline friend bool	operator!=(const sSetting& a_lhs, const char* a_rhs)							{ return !operator==(a_lhs, a_rhs); }
		inline friend bool	operator==(const char* a_lhs, const sSetting& a_rhs)							{ return  operator==(a_rhs, a_lhs); }
		inline friend bool	operator!=(const char* a_lhs, const sSetting& a_rhs)							{ return !operator==(a_rhs, a_lhs); }
		inline friend bool	operator< (const sSetting& a_lhs, const sSetting& a_rhs)						{ return a_lhs._value < a_rhs._value; }
		inline friend bool	operator> (const sSetting& a_lhs, const sSetting& a_rhs)						{ return  operator< (a_rhs, a_lhs); }
		inline friend bool	operator<=(const sSetting& a_lhs, const sSetting& a_rhs)						{ return !operator> (a_lhs, a_rhs); }
		inline friend bool	operator>=(const sSetting& a_lhs, const sSetting& a_rhs)						{ return !operator< (a_lhs, a_rhs); }

	protected:
		std::string _value;
	};


	class aSetting : public ISetting
	{
	private:
		typedef nlohmann::json json;

	public:
		aSetting(std::string a_key, std::initializer_list<std::string> a_list = {}) : ISetting(a_key), _values(a_list) { settings.push_back(this); }
		virtual ~aSetting() {}

		virtual void Assign(json& a_val) override
		{
			_values.clear();
			for (auto& val : a_val) {
				_values.emplace_back(val.get<std::string>());
			}
		}
		virtual void Dump() override
		{
			_DMESSAGE("%s:", _key.c_str());
			for (auto& val : _values) {
				_DMESSAGE("\t%s", val.c_str());
			}
		}
		inline std::vector<std::string>::iterator	begin()	noexcept	{ return _values.begin(); }
		inline std::vector<std::string>::iterator	end()	noexcept	{ return _values.end(); }

	protected:
		std::vector<std::string> _values;
	};


	class Settings
	{
	public:
		static bool		loadSettings();
		static void		dump();


		static aSetting	unpausedMenus;

	private:
		Settings() {}
		~Settings() {}


		static const char* FILE_NAME;
	};
}
