#include "aimbot.h"

bool Settings::Aimbot::enabled = true;
bool Settings::Aimbot::silent = true;
bool Settings::Aimbot::rcs = true;
bool Settings::Aimbot::friendly = false;
bool Settings::Aimbot::autowall = true;
								   //head, neck, pelvis, belly, thorax, lower chest, upper chest, left thigh, right thigh, left upper arm, right upper arm
bool Settings::Aimbot::hitboxes[] = {true, true, true,   true,  true,   true,		 true,		  true,		  true,		   true,		   true};
float Settings::Aimbot::mindmg = 15.f;
bool Settings::Aimbot::autoRevolver = true;
bool Settings::Aimbot::autoPistol = true;
bool Settings::Aimbot::autoShoot = true;
bool Settings::Aimbot::autoScope = true;

bool Settings::Aimbot::forwardtrack = true;

bool Settings::Aimbot::HitChance::enabled = true;
int Settings::Aimbot::HitChance::hitRays = 100;
float Settings::Aimbot::HitChance::value::autoSniper = 0.35f;
float Settings::Aimbot::HitChance::value::awp = 0.8f;
float Settings::Aimbot::HitChance::value::scout = 0.8f;
float Settings::Aimbot::HitChance::value::r8 = 0.65f;
float Settings::Aimbot::HitChance::value::other = 0.3f;

bool Settings::Aimbot::multipoint = true;
float Settings::Aimbot::pointscale = 0.7f;
float Settings::Aimbot::pointscaleBaim = 0.55f;

int AimBot::target = -1;
float AimBot::targetDmg = 0.f;
Vector AimBot::aimSpot = Vector(0, 0, 0);
bool AimBot::forceBaim[64];
bool AimBot::dontShoot[64];
bool AimBot::shootingRevolver = false;
int AimBot::cmdNum;
bool AimBot::forceTick[64];
int AimBot::tickCount[64];

Vector VelocityExtrapolate(C_BasePlayer* pTarget, Vector aimPos, int amountOfTicks = 1) {
	return aimPos + pTarget->GetVelocity() * (pGlobalVars->interval_per_tick * (float)amountOfTicks);
}

bool AimBot::HitChance(const Vector& point, C_BasePlayer* pTarget, bool bt, CTickRecord btRecord) {
	if (!Settings::Aimbot::HitChance::enabled)
		return true;

	//C_BasePlayer* pLocal = (C_BasePlayer*)pEntityList->GetClientEntity(pEngine->GetLocalPlayer());
	C_BaseCombatWeapon* pActiveWeapon = (C_BaseCombatWeapon*)pEntityList->GetClientEntityFromHandle(Globals::pLocal->GetActiveWeapon());

	// ye something is missing here i wonder what

	int hitCount = 0;
	for (int i = 0; i < Settings::Aimbot::HitChance::hitRays; i++) {
		Vector dst = point;

		if (!pActiveWeapon)
			return false;

		// if you think this is wrong enable console spam and test your new math before pushing an updated -.-
		float range = pActiveWeapon->GetCSWpnData()->GetRange();
		float a = (float)M_PI * 2.0f * ((float)(rand() % 1000) / 1000.0f);
		float b = pActiveWeapon->GetSpread() * ((float)(rand() % 1000) / 1000.0f) * 90.0f;
		float c = (float)M_PI * 2.0f * ((float)(rand() % 1000) / 1000.0f);
		float d = pActiveWeapon->GetInaccuracy() * ((float)(rand() % 1000) / 1000.0f) * 90.0f;

		Vector dir, src, dest;
		trace_t tr;
		Ray_t ray;
		CTraceFilterEntitiesOnly filter;

		src = Globals::pLocal->GetEyePosition();
		Vector angles = Math::CalcAngle(src, dst);
		angles.x += (cos(a) * b) + (cos(c) * d);
		angles.y += (sin(a) * b) + (sin(c) * d);
		Math::AngleVector(angles, dir);

		dest = src + (dir * range); // not sure what this magic 8192 was replaced it with range :eyo:

		ray.Init(src, dest);
		filter.pSkip = Globals::pLocal;
		pTrace->TraceRay(ray, MASK_SHOT, &filter, &tr);

		C_BasePlayer* player = (C_BasePlayer*)tr.m_pEnt;
		if (player && player->GetClientClass()->m_ClassID == EClassIds::CCSPlayer && player != Globals::pLocal &&
			!player->GetDormant() && player->GetAlive() && !player->GetImmune() &&
			(player->GetTeam() != Globals::pLocal->GetTeam() || Settings::Aimbot::friendly))
			hitCount++;
	}

	//cvar->ConsoleDPrintf("HitCount: %d/%d - %f\n", hitCount, Settings::Aimbot::HitChance::hitRays, Settings::Aimbot::HitChance::value);

	if (bt) {
		pTarget->InvalidateBoneCache();

		CTickRecord latest = BackTrack::backtrackRecords[pTarget->GetIndex()].back();

		pTarget->GetCollideable()->OBBMins() = latest.mins;
		pTarget->GetCollideable()->OBBMaxs() = latest.maxs;

		pTarget->setAbsAngle(Vector(0, latest.m_angEyeAngles.y, 0));
		pTarget->setAbsOriginal(latest.m_vecOrigin);

		*pTarget->GetFlagsPointer() = latest.flags;

		pTarget->GetPoseParameter() = latest.m_flPoseParameter;
	}

	float value = 0.0f;

	switch (*pActiveWeapon->GetItemDefinitionIndex()) {
	case ItemDefinitionIndex::WEAPON_SSG08:
		value = Settings::Aimbot::HitChance::value::scout;
		break;
	case ItemDefinitionIndex::WEAPON_G3SG1:
	case ItemDefinitionIndex::WEAPON_SCAR20:
		value = Settings::Aimbot::HitChance::value::autoSniper;
		break;
	case ItemDefinitionIndex::WEAPON_AWP:
		value = Settings::Aimbot::HitChance::value::awp;
		break;
	case ItemDefinitionIndex::WEAPON_REVOLVER:
	case ItemDefinitionIndex::WEAPON_DEAGLE:
		value = Settings::Aimbot::HitChance::value::r8;
		break;
	default:
		value = Settings::Aimbot::HitChance::value::other;
		break;
	}

	return ((float)hitCount / (float)Settings::Aimbot::HitChance::hitRays > value);
}

C_BasePlayer* GetZeusTarget(float &distance) {
	// Get pLocal pos
	Vector localPos = Globals::pLocal->GetEyePosition();
	
	float bestDistance = 8000.f;

	C_BaseCombatWeapon* pActiveWeapon = (C_BaseCombatWeapon*)pEntityList->GetClientEntityFromHandle(Globals::pLocal->GetActiveWeapon());

	C_BasePlayer* bestTarget = nullptr;
	for (int i = 1; i < pEngine->GetMaxClients(); i++) {
		C_BasePlayer* targ = (C_BasePlayer*)pEntityList->GetClientEntity(i);

		if (!targ
			|| targ->GetDormant()
			|| !targ->GetAlive()
			|| targ->GetTeam() == Globals::pLocal->GetTeam())
			continue;

		// get pos of targ
		Vector targPos = targ->GetBonePosition((int)Bone::BONE_PELVIS);

		float dist = targPos.DistTo(localPos);
		if (dist < bestDistance && dist < pActiveWeapon->GetCSWpnData()->GetRange() && Entity::IsVisible(targ, Bone::BONE_PELVIS)) {
			bestTarget = targ;
			bestDistance = dist;
			distance = dist;
		}
	}

	return bestTarget;
}

void AutoZeus(CUserCmd* pCmd) {	
	C_BaseCombatWeapon* pActiveWeapon = (C_BaseCombatWeapon*)pEntityList->GetClientEntityFromHandle(Globals::pLocal->GetActiveWeapon());

	if (pActiveWeapon->GetNextPrimaryAttack() > pGlobalVars->curtime)
		return;

	// find closest target
	float distance = -1;
	C_BasePlayer* targ = GetZeusTarget(distance);

	Vector oldAngle;
	pEngine->GetViewAngles(oldAngle);
	float oldForward = pCmd->forwardmove;
	float oldSideMove = pCmd->sidemove;

	if (targ) {
		Vector angle;
		pEngine->GetViewAngles(angle);

		Vector eVecTarget = targ->GetBonePosition((int)Bone::BONE_HIP);;
		Vector pVecTarget = Globals::pLocal->GetEyePosition();
		AimBot::aimSpot = eVecTarget;

		angle = Math::CalcAngle(pVecTarget, eVecTarget);

		CCSWeaponInfo* wpnData = pActiveWeapon->GetCSWpnData();

		//calculate dmg
		float dmg = wpnData->GetDamage() * (pow(wpnData->GetRangeModifier(), (distance/500)));

		//ghetto but works
		if (dmg >= targ->GetHealth()) {
			// Normalize + clamp angles for anti ut
			Math::NormalizeAngles(angle);
			Math::ClampAngles(angle);

			// dont set angle when its same
			if (angle != pCmd->viewangles)
				pCmd->viewangles = angle;

			Math::CorrectMovement(oldAngle, pCmd, oldForward, oldSideMove);

			pCmd->buttons |= IN_ATTACK;
		}
	}
}

std::vector<Vector> GetPointsToScan(C_BasePlayer* pTarget, CSGOHitbox hitbox) {
	std::vector<Vector> pointsToScan;

	matrix3x4_t matrix[128];

	if (!pTarget->SetupBones(matrix, 128, 0x100, 0.f))
		return pointsToScan;
	model_t* pModel = pTarget->GetModel();
	if (!pModel)
		return pointsToScan;

	studiohdr_t* hdr = pModelInfo->GetStudioModel(pModel);
	if (!hdr)
		return pointsToScan;
	mstudiobbox_t* bbox = hdr->pHitbox((int)hitbox, 0);
	if (!bbox)
		return pointsToScan;

	float mod = bbox->radius != -1.f ? bbox->radius : 0.f;

	Vector max;
	Vector min;

	Vector in1 = bbox->bbmax + mod;
	Vector in2 = bbox->bbmin - mod;

	Math::VectorTransform(in1, matrix[bbox->bone], max);
	Math::VectorTransform(in2, matrix[bbox->bone], min);

	auto center = (min + max) * 0.5f;

	Vector curAngles = Math::CalcAngle(center, Globals::pLocal->GetEyePosition());

	Vector forward;
	Math::AngleVector(curAngles, forward);

	Vector right = forward.Cross(Vector(0, 0, 1));
	Vector left = Vector(-right.x, -right.y, right.z);

	Vector top = Vector(0, 0, 1);
	Vector bot = Vector(0, 0, -1);

	float POINT_SCALE = AimBot::forceBaim[pTarget->GetIndex()] ? Settings::Aimbot::pointscaleBaim : Settings::Aimbot::pointscale;
	if (Settings::Aimbot::multipoint/* && pTarget->GetVelocity().Length2D() == 0*/)
	{
		switch (hitbox)
		{
		case CSGOHitbox::HITBOX_HEAD:
			for (auto i = 0; i < 4; ++i)
			{
				pointsToScan.emplace_back(center);
			}
			pointsToScan[1] += top * (bbox->radius * POINT_SCALE);
			pointsToScan[2] += right * (bbox->radius * POINT_SCALE);
			pointsToScan[3] += left * (bbox->radius * POINT_SCALE);
			break;

		default:

			for (auto i = 0; i < 2; ++i)
			{
				pointsToScan.emplace_back(center);
			}
			pointsToScan[0] += right * (bbox->radius * POINT_SCALE);
			pointsToScan[1] += left * (bbox->radius * POINT_SCALE);
			break;
		}
	}
	else
		pointsToScan.emplace_back(center);

	return pointsToScan;
}

std::vector<Vector> GetPointsToScanBT(C_BasePlayer* pTarget, CSGOHitbox hitbox, CTickRecord record) {
	std::vector<Vector> pointsToScan;

	if (!record.matrixBuilt)
		return pointsToScan;
	model_t* pModel = pTarget->GetModel();
	if (!pModel)
		return pointsToScan;

	studiohdr_t* hdr = pModelInfo->GetStudioModel(pModel);
	if (!hdr)
		return pointsToScan;

	mstudiobbox_t* bbox = hdr->pHitbox((int)hitbox, 0);
	if (!bbox)
		return pointsToScan;

	float mod = bbox->radius != -1.f ? bbox->radius : 0.f;

	Vector max;
	Vector min;

	Vector in1 = bbox->bbmax + mod;
	Vector in2 = bbox->bbmin - mod;

	Math::VectorTransform(in1, record.matrix[bbox->bone], max);
	Math::VectorTransform(in2, record.matrix[bbox->bone], min);

	auto center = (min + max) * 0.5f;

	Vector curAngles = Math::CalcAngle(center, Globals::pLocal->GetEyePosition());

	Vector forward;
	Math::AngleVector(curAngles, forward);

	Vector right = forward.Cross(Vector(0, 0, 1));
	Vector left = Vector(-right.x, -right.y, right.z);

	Vector top = Vector(0, 0, 1);
	Vector bot = Vector(0, 0, -1);

	float POINT_SCALE = AimBot::forceBaim[pTarget->GetIndex()] ? Settings::Aimbot::pointscaleBaim : Settings::Aimbot::pointscale;
	if (Settings::Aimbot::multipoint/* && pTarget->GetVelocity().Length2D() == 0*/)
	{
		switch (hitbox)
		{
		case CSGOHitbox::HITBOX_HEAD:
			for (auto i = 0; i < 4; ++i)
			{
				pointsToScan.emplace_back(center);
			}
			pointsToScan[1] += top * (bbox->radius * POINT_SCALE);
			pointsToScan[2] += right * (bbox->radius * POINT_SCALE);
			pointsToScan[3] += left * (bbox->radius * POINT_SCALE);
			break;

		default:

			for (auto i = 0; i < 2; ++i)
			{
				pointsToScan.emplace_back(center);
			}
			pointsToScan[0] += right * (bbox->radius * POINT_SCALE);
			pointsToScan[1] += left * (bbox->radius * POINT_SCALE);
			break;
		}
	}
	else
		pointsToScan.emplace_back(center);

	return pointsToScan;
}

std::vector<CSGOHitbox> HitboxesToScan(C_BasePlayer* pTarget) {
	std::vector<CSGOHitbox> hitboxesToScan = {};
	
	if (!pTarget
		|| !pTarget->GetAlive()
		|| pTarget->GetDormant())
		return hitboxesToScan;

	int id = pTarget->GetIndex();

	if (AimBot::dontShoot[id])
		return hitboxesToScan;

	bool onGround = pTarget->GetFlags() & FL_ONGROUND;

	if (/*!onGround || */AimBot::forceBaim[id]) {
		hitboxesToScan.push_back(CSGOHitbox::HITBOX_MAX);
		hitboxesToScan.push_back(CSGOHitbox::HITBOX_MAX);
		hitboxesToScan.push_back(CSGOHitbox::HITBOX_PELVIS);
		hitboxesToScan.push_back(CSGOHitbox::HITBOX_BELLY);
		hitboxesToScan.push_back(CSGOHitbox::HITBOX_THORAX);
		hitboxesToScan.push_back(CSGOHitbox::HITBOX_LOWER_CHEST);
		hitboxesToScan.push_back(CSGOHitbox::HITBOX_UPPER_CHEST);
		hitboxesToScan.push_back(CSGOHitbox::HITBOX_LEFT_THIGH);
		hitboxesToScan.push_back(CSGOHitbox::HITBOX_RIGHT_THIGH);
		hitboxesToScan.push_back(CSGOHitbox::HITBOX_LEFT_UPPER_ARM);
		hitboxesToScan.push_back(CSGOHitbox::HITBOX_RIGHT_UPPER_ARM);
	}
	else {
		hitboxesToScan.push_back(CSGOHitbox::HITBOX_HEAD);
		hitboxesToScan.push_back(CSGOHitbox::HITBOX_NECK);
		hitboxesToScan.push_back(CSGOHitbox::HITBOX_PELVIS);
		hitboxesToScan.push_back(CSGOHitbox::HITBOX_BELLY);
		hitboxesToScan.push_back(CSGOHitbox::HITBOX_THORAX);
		hitboxesToScan.push_back(CSGOHitbox::HITBOX_LOWER_CHEST);
		hitboxesToScan.push_back(CSGOHitbox::HITBOX_UPPER_CHEST);
		hitboxesToScan.push_back(CSGOHitbox::HITBOX_LEFT_THIGH);
		hitboxesToScan.push_back(CSGOHitbox::HITBOX_RIGHT_THIGH);
		hitboxesToScan.push_back(CSGOHitbox::HITBOX_LEFT_UPPER_ARM);
		hitboxesToScan.push_back(CSGOHitbox::HITBOX_RIGHT_UPPER_ARM);
	}
	
	return hitboxesToScan;
}


Vector GetBestDamagePoint(C_BasePlayer* pTarget, float& dmg) {
	// would use range for loop but hard to check against settings for (const CSGOHitbox& hitbox : scanHitboxes)

	float bestDmg = 0.f;
	Vector bestAimSpot = Vector(0, 0, 0);

	auto scanHitboxes = HitboxesToScan(pTarget);

	for (std::vector<CSGOHitbox>::size_type i = 0; i != scanHitboxes.size(); i++) {
		if (!Settings::Aimbot::hitboxes[i]) // skip not selected hitboxes
			continue;

		if (scanHitboxes[i] == CSGOHitbox::HITBOX_MAX)
			continue;

		auto pointsToScan = GetPointsToScan(pTarget, scanHitboxes[i]);

		for (int j = 0; j < pointsToScan.size(); j++) {
			// calc dmg here
			int ticksToCorrect = TIME_TO_TICKS(1 - 0); // b1g fake lag fix antipasta HINT: animtime - simtime but animtime isnt networked you need to use some other time hinthint not depending on the entity

			float damage = Autowall::GetDamageVec(VelocityExtrapolate(pTarget, pointsToScan[j], ticksToCorrect), pTarget, scanHitboxes[i]);

			if ((damage > bestDmg && damage >= Settings::Aimbot::mindmg) || damage > pTarget->GetHealth()) {
				bestDmg = damage;
				bestAimSpot = pointsToScan[j];
				if (damage > pTarget->GetHealth()) {
					// lets break we have found the best one, it can 1 him
					dmg = damage;
					return pointsToScan[j];
				}
			}
		}
	}

	breakloop:

	dmg = bestDmg;
	return bestAimSpot;
}

CTickRecord RageBacktrack(C_BasePlayer* pTarget, int i, float &dmg, Vector &aimSpot) {
	float bestDmg = 0.f;
	Vector bestAimSpot = Vector(0, 0, 0);
	CTickRecord bestTick;

	for (auto record = BackTrack::backtrackRecords[i].begin(); record != BackTrack::backtrackRecords[i].end(); record++) {
		if (record->calcPos != Globals::pLocal->GetEyePosition()) {
			//float simTime = pTarget->GetSimulationTime();
			//float curTime = pGlobalVars->curtime;
			//float frameTime = pGlobalVars->frametime;
			
			pTarget->InvalidateBoneCache();

			pTarget->GetCollideable()->OBBMins() = record->mins;
			pTarget->GetCollideable()->OBBMaxs() = record->maxs;

			//pGlobalVars->curtime = simTime;
			//pGlobalVars->frametime = pGlobalVars->interval_per_tick;

			*pTarget->GetFlagsPointer() = record->flags;

			pTarget->GetPoseParameter() = record->m_flPoseParameter;

			pTarget->setAbsAngle(Vector(0, record->m_angEyeAngles.y, 0));
			pTarget->setAbsOriginal(record->m_vecOrigin);

			/*AnimationLayer backup_layers[15];
			std::memcpy(backup_layers, pTarget->GetAnimOverlays(), (sizeof(AnimationLayer) * pTarget->GetNumAnimOverlays()));

			// invalidates prior animations so the entity gets animated on our client 100% via UpdateClientSideAnimation
			CBasePlayerAnimState *state = pTarget->GetPlayerAnimState();
			if (state)
				state->m_iLastClientSideAnimationUpdateFramecount() = pGlobalVars->framecount - 1;

			*pTarget->ClientSideAnimation() = true;

			pTarget->updateClientSideAnimation();

			*pTarget->ClientSideAnimation() = false;

			std::memcpy(pTarget->GetAnimOverlays(), backup_layers, (sizeof(AnimationLayer) * pTarget->GetNumAnimOverlays()));
			pGlobalVars->curtime = curTime;
			pGlobalVars->frametime = frameTime;*/

			//AnimFix(pTarget, *record);

			if (!record->matrixBuilt)
			{
				if (!pTarget->SetupBones(record->matrix, 128, 256, record->m_flSimulationTime))
					continue;

				record->matrixBuilt = true;
			}

			float damage;

			Vector aimSpotB = GetBestDamagePoint(pTarget, damage);

			if (damage > bestDmg && damage > Settings::Aimbot::mindmg) {
				bestDmg = damage;
				bestAimSpot = aimSpotB;
				bestTick = *record;

				if (damage > pTarget->GetHealth())
					goto foundbest;
			}
			record->calcPos = Globals::pLocal->GetEyePosition();
		}
	}

	foundbest:

	pTarget->InvalidateBoneCache();

	CTickRecord latest = BackTrack::backtrackRecords[i].back();

	pTarget->GetCollideable()->OBBMins() = latest.mins;
	pTarget->GetCollideable()->OBBMaxs() = latest.maxs;

	pTarget->setAbsAngle(Vector(0, latest.m_angEyeAngles.y, 0));
	pTarget->setAbsOriginal(latest.m_vecOrigin);

	*pTarget->GetFlagsPointer() = latest.flags;

	pTarget->GetPoseParameter() = latest.m_flPoseParameter;

	dmg = bestDmg;
	aimSpot = bestAimSpot;
	return bestTick;
}

C_BasePlayer* GetTarget(float &dmg, Vector &bestAimSpot, bool &bt, CTickRecord &backtrackRecord) {
	C_BasePlayer* bestTarget = nullptr;

	for (int i = 0; i <= pEngine->GetMaxClients(); i++) {
		C_BasePlayer* pTarget = (C_BasePlayer*)pEntityList->GetClientEntity(i);

		if (!pTarget
			|| !pTarget->GetAlive()
			|| pTarget->GetHealth() <= 0
			|| pTarget->GetClientClass()->m_ClassID != EClassIds::CCSPlayer
			|| pTarget->GetImmune()
			|| pTarget->GetDormant()
			|| pTarget->GetTeam() == Globals::pLocal->GetTeam())
			continue;

		float damage = 0.f;
		bool backtrack = false;

		Vector aimSpot;

		if (Settings::Aimbot::backtrack && BackTrack::backtrackRecords[i].size() > 1 && pTarget->GetVelocity().Length2D() > 1.f && (BackTrack::backtrackRecords[i].end()[-2].absOrigin - BackTrack::backtrackRecords[i].back().absOrigin).Length2DSqr() < 4096.f) {
			backtrack = true;
			backtrackRecord = RageBacktrack(pTarget, i, damage, aimSpot);
		}
		else
			aimSpot = GetBestDamagePoint(pTarget, damage);

														// really weird shit idk
		if (aimSpot == Vector(0, 0, 0) || aimSpot.DistTo(Vector(-107374176, -107374176, -107374176)) < 1.f)
			continue;

		if (damage > Settings::Aimbot::mindmg) {
			dmg = damage;
			bestAimSpot = aimSpot;
			bt = backtrack;
			bestTarget = pTarget;
			// kinda shitty version of baim if lethal
			if (dmg > pTarget->GetHealth())
				return bestTarget;
		}
	}

	return bestTarget;
}

void AutoShoot(CUserCmd* pCmd, C_BasePlayer* pTarget, C_BaseCombatWeapon* pActiveWeapon, Vector aimSpot, bool bt, CTickRecord btRecord) {
	if (!Settings::Aimbot::autoShoot)
		return;

	if (!pTarget || pActiveWeapon->GetAmmo() == 0)
		return;

	CSWeaponType weaponType = pActiveWeapon->GetCSWpnData()->GetWeaponType();
	if (weaponType == CSWeaponType::WEAPONTYPE_KNIFE || weaponType == CSWeaponType::WEAPONTYPE_C4 ||
		weaponType == CSWeaponType::WEAPONTYPE_GRENADE)
		return;

	if (pCmd->buttons & IN_USE)
		return;

	if (Settings::Aimbot::autoScope && pActiveWeapon->HasScope() &&
		!Globals::pLocal->IsScoped())
		pCmd->buttons |= IN_ATTACK2;

	if (Settings::Aimbot::HitChance::enabled && !AimBot::HitChance(aimSpot, pTarget, bt, btRecord))
		return;

	float nextPrimaryAttack = pActiveWeapon->GetNextPrimaryAttack();

	if (nextPrimaryAttack > pGlobalVars->curtime) {
		if (*pActiveWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER)
			pCmd->buttons &= ~IN_ATTACK2;
		else
			pCmd->buttons &= ~IN_ATTACK;
	}
	else {
		if (Settings::Aimbot::autoScope && pActiveWeapon->HasScope() &&	!Globals::pLocal->IsScoped())
			pCmd->buttons |= IN_ATTACK2;
		else if (*pActiveWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER)
			pCmd->buttons |= IN_ATTACK2;
		else
			pCmd->buttons |= IN_ATTACK;
	}
}

void AutoPistol(C_BaseCombatWeapon* pActiveWeapon, CUserCmd* pCmd) {
	if (!Settings::Aimbot::autoPistol)
		return;

	if (!pActiveWeapon || pActiveWeapon->GetCSWpnData()->GetWeaponType() != CSWeaponType::WEAPONTYPE_PISTOL)
		return;

	if (pActiveWeapon->GetNextPrimaryAttack() < pGlobalVars->curtime)
		return;

	if (*pActiveWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER)
		pCmd->buttons &= ~IN_ATTACK2;
	else
		pCmd->buttons &= ~IN_ATTACK;
}

void ShootCheck(C_BaseCombatWeapon* pActiveWeapon, CUserCmd* pCmd) {
	if (!Settings::AntiAim::Yaw::enabled && !Settings::AntiAim::Pitch::enabled)
		return;

	if (!Settings::Aimbot::silent)
		return;

	if (!(pCmd->buttons & IN_ATTACK))
		return;

	if (pActiveWeapon->GetNextPrimaryAttack() < pGlobalVars->curtime)
		return;

	if (*pActiveWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_C4)
		return;

	if (*pActiveWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER)
		pCmd->buttons &= ~IN_ATTACK2;
	else
		pCmd->buttons &= ~IN_ATTACK;
}

void AimBot::CreateMove(CUserCmd* pCmd) {
	if (!pEngine->IsInGame() || !pEngine->IsConnected())
		return;

	if (!Settings::Aimbot::enabled)
		return;

	if (!Globals::pLocal || !Globals::pLocal->GetAlive())
		return;

	C_BaseCombatWeapon* pActiveWeapon = (C_BaseCombatWeapon*)pEntityList->GetClientEntityFromHandle(Globals::pLocal->GetActiveWeapon());
	
	if (!pActiveWeapon || pActiveWeapon->GetInReload())
		return;

	CSWeaponType weaponType = pActiveWeapon->GetCSWpnData()->GetWeaponType();

	if (*pActiveWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_TASER) {
		AutoZeus(pCmd);
		return;
	}
	else if (weaponType == CSWeaponType::WEAPONTYPE_KNIFE) {
		// autoknife
		return;
	}
	else if (weaponType == CSWeaponType::WEAPONTYPE_C4 || weaponType == CSWeaponType::WEAPONTYPE_GRENADE)
		return;

	Vector oldAngle;
	pEngine->GetViewAngles(oldAngle);
	float oldForward = pCmd->forwardmove;
	float oldSideMove = pCmd->sidemove;

	float dmg = 0.f;
	bool bt = false;
	CTickRecord btRecord;

	C_BasePlayer* pTarget = GetTarget(dmg, aimSpot, bt, btRecord);

	Vector viewAngles;
	pEngine->GetViewAngles(viewAngles);

	if (pTarget) {
		target = pTarget->GetIndex();
		targetDmg = dmg;

		int ticksToCorrect = TIME_TO_TICKS(1 - 0); // b1g fake lag fix antipasta HINT: animtime - simtime but animtime isnt networked you need to use some other time hinthint not depending on the entity

		viewAngles = Math::CalcAngle(Globals::pLocal->GetEyePosition(), VelocityExtrapolate(pTarget, aimSpot, ticksToCorrect));

		if (Settings::Aimbot::rcs) {
			float recoilScale = pCvar->FindVar("weapon_recoil_scale")->GetFloat();
			viewAngles -= *Globals::pLocal->GetAimPunchAngle() * recoilScale;
		}

		if (forceTick[target]) {
			pCmd->tick_count = tickCount[target];
			forceTick[target] = false;
		}
		else if (bt)
			pCmd->tick_count = TIME_TO_TICKS(btRecord.m_flSimulationTime - BackTrack::GetLerpTime());
		else {
			//pCmd->tick_count = TIME_TO_TICKS(pTarget->GetSimulationTime() + BackTrack::GetLerpTime());
			pCmd->tick_count += ticksToCorrect;
		}
	}

	AutoPistol(pActiveWeapon, pCmd);
	AutoShoot(pCmd, pTarget, pActiveWeapon, aimSpot, bt, btRecord);
	ShootCheck(pActiveWeapon, pCmd);

	// normalize then clamp for anti ut
	Math::NormalizeAngles(viewAngles);
	Math::ClampAngles(viewAngles);

	if (viewAngles != pCmd->viewangles)
		pCmd->viewangles = viewAngles;

	Math::CorrectMovement(oldAngle, pCmd, oldForward, oldSideMove);

	if (!Settings::Aimbot::silent)
		pEngine->SetViewAngles(viewAngles);
}
