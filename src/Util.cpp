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
}
