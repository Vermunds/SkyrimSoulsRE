#pragma once

namespace SkyrimSoulsRE
{
	class MenuFlagHandler
	{
	public:
		void Update();
		void UpdateMenu(RE::IMenu* a_menu, std::string_view a_menuName, bool a_isOpen);

		static MenuFlagHandler* GetSingleton();

	private:
		MenuFlagHandler() {};
		~MenuFlagHandler() {};
		MenuFlagHandler(const MenuFlagHandler&) = delete;
		MenuFlagHandler& operator=(const MenuFlagHandler&) = delete;
	};
}
