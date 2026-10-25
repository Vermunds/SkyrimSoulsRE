#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	// Enderal's Hero Menu is not a menu of its own, it is a CustomMenu with this movie loaded.
	class HeroMenu
	{
	public:
		static constexpr std::string_view MENU_NAME = "CustomMenu";
		static constexpr std::string_view MOVIE_NAME = "_heromenu.swf";

		static bool IsOpen();
		static void ProcessInput(RE::InputEvent* const* a_event);
	};
}
