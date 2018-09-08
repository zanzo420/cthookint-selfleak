#pragma once

#include "../settings.h"
#include "../SDK/SDK.h"
#include "../interfaces.h"
#include "resolver.h"
#include "esp.h"

struct impactInfo {
	impactInfo(Vector pos, Vector startPos, Color color, float time) {
		this->pos = pos;
		this->startPos = startPos;
		this->color = color;
		this->time = time;
	}

	int dmg;
	Vector pos;
	Vector startPos;
	Color color;
	float time;
};

static std::deque<impactInfo> impacts = {};

namespace HitMarker
{
	void FrameStageNotify();
	void FireGameEvent(IGameEvent* pEvent);
}