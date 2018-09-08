#pragma once 

#include "../settings.h"
#include "../SDK/SDK.h"
#include "../interfaces.h"
#include "../math.h"

namespace Autowall
{
	struct FireBulletData
	{
		Vector src;
		trace_t enter_trace;
		Vector direction;
		CTraceFilter filter;
		float trace_length;
		float trace_length_remaining;
		float current_damage;
		int penetrate_count;
	};

	float GetDamageVec(const Vector &vecPoint, C_BasePlayer *player, CSGOHitbox hitbox, bool bt = false);
}