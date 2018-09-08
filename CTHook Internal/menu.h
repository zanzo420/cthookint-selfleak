#pragma once

#include "Hooks.h"
#include "ImGui\imgui.h"
#include "ImGui\imgui_internal.h"
#include "ImGui\imgui_impl_dx9.h"
#include <d3d9.h>
#include "Menu/gui.h"
#include "Menu/plist.h"

namespace Menu {
	void Draw();

	extern bool render;
}
