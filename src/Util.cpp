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
		return std::wstring();
	}
}
