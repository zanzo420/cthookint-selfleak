#pragma once 

#define NOMINMAX

#include "../settings.h"
#include "../SDK/SDK.h"
#include "../interfaces.h"
#include "../math.h"
#include "../draw.h"
#include "../utilItems.h"
#include "antiaim.h"
#include "resolver.h"
#include <iomanip>

namespace ESP {
	void Paint();
	bool PrePaintTraverse(VPANEL vgui_panel, bool force_repaint, bool allow_force);

	void DrawHitboxes(int player, int r, int g, int b, int a, float duration);
}