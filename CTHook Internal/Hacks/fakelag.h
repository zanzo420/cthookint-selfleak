#pragma once

#include "../settings.h"
#include "../SDK/SDK.h"
#include "../interfaces.h"
#include "../draw.h"
#include "antiaim.h"

namespace FakeLag
{
	void CreateMove(CUserCmd* pCmd);

	extern bool lcBroken;
	extern bool sendPacket;
	extern Vector lastUnchokedPos;
}