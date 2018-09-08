#pragma once 

#include "../settings.h"
#include "../SDK/SDK.h"
#include "../interfaces.h"
#include "../math.h"
#include "aimbot.h"
#include "resolver.h"
#include "fakelag.h"

namespace AntiAim {
	void CreateMove(CUserCmd* pCmd);
	float DetectsThemWalls(float range);

	extern float lastReal;
	extern float lastFake;
	extern bool doingAA;
	extern bool lbyUpdated;
	extern bool lbyUpdatedd;
	extern float nextLBYUpdate;
}