#pragma once

#include <ModConfigUI/ModConfigUI.h>

namespace SkyrimSoulsRE
{
	void InstallModConfigUI();

	void DrawUnpausedMenusPage(ModConfigUI::Renderer& a_renderer);
	void DrawAutoClosePage(ModConfigUI::Renderer& a_renderer);
	void DrawDataUpdatesPage(ModConfigUI::Renderer& a_renderer);
	void DrawControlsPage(ModConfigUI::Renderer& a_renderer);
	void DrawSlowMotionPage(ModConfigUI::Renderer& a_renderer);
	void DrawCombatAlertOverlayPage(ModConfigUI::Renderer& a_renderer);
	void DrawMiscellaneousPage(ModConfigUI::Renderer& a_renderer);
}
