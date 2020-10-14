#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	class CustomMenuEx : RE::IMenu
	{
	public:
		static RE::IMenu* Creator();
	};
}