#include "Util.h"

namespace SkyrimSoulsRE::Util
{
	std::string GetRefDebugString(const RE::TESObjectREFR* a_form)
	{
		if (!a_form)
		{
			return "NULL";
		}

		const char* name = a_form->GetName();
		const char* editorID = a_form->GetFormEditorID();
		RE::FormID formID = a_form->formID;
		RE::FormType formType = a_form->formType.get();

		return std::format("{} \"{}\" [{}:{:08X}]", editorID, name, RE::FormTypeToString(formType), formID);
	}

	std::wstring TranslateUIString(RE::GFxMovieView* a_view, const std::wstring& a_key)
	{
		auto translator = a_view->GetState<RE::GFxTranslator>(RE::GFxState::StateType::kTranslator);
		RE::GPtr bsTranslator{ skyrim_cast<RE::BSScaleformTranslator*>(translator.get()) };
		if (bsTranslator)
		{
			auto& map = bsTranslator->translator.translationMap;
			auto it = map.find(a_key);
			if (it != map.end())
			{
				return std::wstring(it->second);
			}
		}
		return a_key;
	}

	std::string TranslateSkyUIString(RE::GFxMovieView* a_view, const std::string& a_key)
	{
		RE::GFxValue result;
		RE::GFxValue args[1];
		args[0].SetString(a_key.c_str());

		if (a_view->Invoke("skyui.util.Translator.translate", &result, args, 1))
		{
			if (result.IsString())
			{
				return std::string(result.GetString());
			}
		}

		return a_key;
	}

	std::string FormatTimeRemaining(float a_seconds, const std::string& a_dayLabel, const std::string& a_hourLabel, const std::string& a_minuteLabel, const std::string& a_secondsLabel)
	{
		if (a_seconds == 0.0f)
		{
			return "-";
		}

		int s = int(std::round(a_seconds));
		int m = s / 60;
		s %= 60;
		int h = m / 60;
		m %= 60;
		int d = h / 24;
		h %= 24;

		std::ostringstream ss;

		if (d > 0)
		{
			ss << d << a_dayLabel << " ";
		}
		if (h > 0 || d > 0)
		{
			ss << h << a_hourLabel << " ";
		}
		if (m > 0 || h > 0 || d > 0)
		{
			ss << m << a_minuteLabel << " ";
		}
		ss << s << a_secondsLabel;

		return ss.str();
	}

	bool IsActorValueMeterUpdateNeeded(float a_oldValue, float a_oldValueMax, float a_newValue, float a_newValueMax, std::int32_t a_numSteps)
	{
		if (a_numSteps == 0 || a_oldValueMax <= 0.0f || a_newValueMax <= 0.0f)
		{
			return a_oldValue != a_newValue || a_oldValueMax != a_newValueMax;
		}

		std::int32_t oldStep = static_cast<std::int32_t>(a_oldValue * a_numSteps / a_oldValueMax);
		std::int32_t newStep = static_cast<std::int32_t>(a_newValue * a_numSteps / a_newValueMax);

		if (a_oldValue >= a_oldValueMax)
		{
			oldStep = a_numSteps;
		}

		if (a_newValue >= a_newValueMax)
		{
			newStep = a_numSteps;
		}

		return oldStep != newStep;
	}

	RE::TESObjectREFR* GetReferenceFromHandle(RE::RefHandle a_handle)
	{
		RE::TESObjectREFRPtr refptr = nullptr;
		RE::TESObjectREFR::LookupByHandle(a_handle, refptr);
		return refptr.get();
	}
}
