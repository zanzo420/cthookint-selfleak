#pragma once

#include "../settings.h"
#include "../SDK/SDK.h"
#include "../interfaces.h"
#include "../math.h"
#include "autowall.h"
#include "../util.h"
#include "../entity.h"
#include "backtrack.h"

static const CSGOHitbox scanHitboxes[7] = {
	CSGOHitbox::HITBOX_HEAD,
	CSGOHitbox::HITBOX_NECK,
	CSGOHitbox::HITBOX_PELVIS,
	CSGOHitbox::HITBOX_BELLY,
	CSGOHitbox::HITBOX_THORAX,
	CSGOHitbox::HITBOX_LOWER_CHEST,
	CSGOHitbox::HITBOX_UPPER_CHEST
};

class HitboxVector {
public:
	HitboxVector(Vector Vector, CSGOHitbox Hitbox) {
		vector = Vector;
		hitbox = Hitbox;
	}

	Vector vector;
	CSGOHitbox hitbox;
};

namespace AimBot
{
	void CreateMove(CUserCmd* pCmd);
	bool HitChance(const Vector& point, C_BasePlayer* pTarget, bool bt, CTickRecord btRecord);

	extern int target;
	extern float targetDmg;
	extern Vector aimSpot;
	extern bool forceBaim[64];
	extern bool dontShoot[64];
	extern bool shootingRevolver;
	extern int cmdNum;
	extern bool forceTick[64];
	extern int tickCount[64];
}
