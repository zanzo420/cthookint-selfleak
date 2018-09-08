#include "Menu.h"

bool Menu::render = false;;

void Menu::Draw()
{
	static bool mouse_enabled = false;

	bool is_renderer_active = renderer.IsActive();
	render = is_renderer_active;

	/*static ConVar* mouseEnable = pCvar->FindVar("cl_mouseenable");

	if (is_renderer_active) {
		if (mouse_enabled) {
			//pEngine->ClientCmd_Unrestricted("cl_mouseenable 0");
			//pInputSystem->EnableInput(false);
			mouseEnable->SetValue(0);
			mouse_enabled = false;
		}
	}
	else {
		if (!mouse_enabled) {
			//pEngine->ClientCmd_Unrestricted("cl_mouseenable 1");
			//pInputSystem->EnableInput(true);
			mouseEnable->SetValue(1);
			mouse_enabled = true;
		}
	}*/
	//ImGui::GetIO().MouseDrawCursor = is_renderer_active;
	//ImGui::GetIO().IniFilename = "imgui.ini";
	//ImGui::LogToFile();
	ImGui_ImplDX9_NewFrame();

	if (is_renderer_active)
	{
		gui::RenderWindow();
		pList::RenderWindow();
	}

	ImGui::Render();
}
