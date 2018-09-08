#include "autowall.h"

void traceIt(Vector &vecAbsStart, Vector &vecAbsEnd, unsigned int mask, C_BasePlayer *ign, CGameTrace *tr)
{
	Ray_t ray;

	CTraceFilter filter;
	filter.pSkip = ign;

	ray.Init(vecAbsStart, vecAbsEnd);

	pTrace->TraceRay(ray, mask, &filter, tr);
}

void ClipTraceToPlayers(const Vector &vecAbsStart, const Vector &vecAbsEnd, unsigned int mask, ITraceFilter *filter, CGameTrace *tr)
{
	trace_t playerTrace;
	Ray_t ray;
	float smallestFraction = tr->fraction;
	const float maxRange = 60.0f;

	ray.Init(vecAbsStart, vecAbsEnd);

	for (int i = 1; i <= pGlobalVars->maxClients; i++)
	{
		C_BasePlayer *player = (C_BasePlayer*)pEntityList->GetClientEntity(i);

		if (!player || !player->GetAlive() || player->GetDormant())
			continue;

		if (filter && filter->ShouldHitEntity(player, mask) == false)
			continue;

		float range = Math::DistanceToRay(player->WorldSpaceCenter(), vecAbsStart, vecAbsEnd);
		if (range < 0.0f || range > maxRange)
			continue;

		pTrace->ClipRayToEntity(ray, mask | CONTENTS_HITBOX, player, &playerTrace);
		if (playerTrace.fraction < smallestFraction)
		{
			*tr = playerTrace;
			smallestFraction = playerTrace.fraction;
		}
	}
}

bool IsArmored(C_BasePlayer *player, int armorVal, HitGroups hitgroup)
{
	bool res = false;

	if (armorVal > 0)
	{
		switch (hitgroup)
		{
		case HitGroups::HITGROUP_GENERIC:
		case HitGroups::HITGROUP_CHEST:
		case HitGroups::HITGROUP_STOMACH:
		case HitGroups::HITGROUP_LEFTARM:
		case HitGroups::HITGROUP_RIGHTARM:

			res = true;
			break;

		case HitGroups::HITGROUP_HEAD:

			res = player->HasHelmet();
			break;

		}
	}

	return res;
}

void ScaleDamage(HitGroups hitgroup, C_BasePlayer *player, float weapon_armor_ratio, float &current_damage)
{
	//bool heavArmor = player->HasHeavyArmor(); broken
	int armor = player->GetArmor();

	switch (hitgroup)
	{
	case HitGroups::HITGROUP_HEAD:
		//current_damage *= heavArmor ? 2.f : 4.f;
		current_damage *= 4.f;
		break;
	case HitGroups::HITGROUP_STOMACH:
		current_damage *= 1.25f;
		break;
	case HitGroups::HITGROUP_LEFTLEG:
	case HitGroups::HITGROUP_RIGHTLEG:
		current_damage *= 0.75f;
		break;
	}

	if (IsArmored(player, armor, hitgroup))
	{
		float bonusValue = 1.f, armorBonusRatio = 0.5f, armorRatio = weapon_armor_ratio / 2.f;

		//Damage gets modified for heavy armor users
		/*if (heavArmor)
		{
			armorBonusRatio = 0.33f;
			armorRatio *= 0.5f;
			bonusValue = 0.33f;
		}*/

		auto NewDamage = current_damage * armorRatio;

		//if (heavArmor)
		//	NewDamage *= 0.85f;

		if (((current_damage - (current_damage * armorRatio)) * (bonusValue * armorBonusRatio)) > armor)
			NewDamage = current_damage - (armor / armorBonusRatio);

		current_damage = NewDamage;
	}
}

bool IsBreakableEntity(C_BasePlayer *ent)
{
	typedef bool(__thiscall *isBreakbaleEntityFn)(C_BasePlayer*);
	static isBreakbaleEntityFn IsBreakableEntityFn = (isBreakbaleEntityFn)FindPattern("client_panorama.dll", "\x55\x8B\xEC\x51\x56\x8B\xF1\x85\xF6\x74\x68", "xxxxxxxxxxx");

	if (IsBreakableEntityFn)
	{
		// 0x27C = m_takedamage

		auto backupval = *reinterpret_cast<int*>((uint32_t)ent + 0x27C);
		auto className = ent->GetClientClass()->m_pNetworkName;

		if (ent != pEntityList->GetClientEntity(0))
		{
			// CFuncBrush:
			// (className[1] != 'F' || className[4] != 'c' || className[5] != 'B' || className[9] != 'h')
			if ((className[1] == 'B' && className[9] == 'e' && className[10] == 'S' && className[16] == 'e') // CBreakableSurface
				|| (className[1] != 'B' || className[5] != 'D')) // CBaseDoor because fuck doors
			{
				*reinterpret_cast<int*>((uint32_t)ent + 0x27C) = 2;
			}
		}

		bool retn = IsBreakableEntityFn(ent);

		*reinterpret_cast<int*>((uint32_t)ent + 0x27C) = backupval;

		return retn;
	}
	else
		return false;
}

bool TraceToExit(Vector &end, CGameTrace *enter_trace, Vector start, Vector dir, CGameTrace *exit_trace)
{
	auto distance = 0.0f;
	int first_contents = 0;

	while (distance <= 90.0f)
	{
		distance += 4.0f;
		end = start + (dir * distance);

		if (!first_contents)
			first_contents = pTrace->GetPointContents(end, MASK_SHOT_HULL | CONTENTS_HITBOX);

		auto point_contents = pTrace->GetPointContents(end, MASK_SHOT_HULL | CONTENTS_HITBOX);

		if (point_contents & MASK_SHOT_HULL && (!(point_contents & CONTENTS_HITBOX) || first_contents == point_contents))
			continue;

		auto new_end = end - (dir * 4.0f);

		traceIt(end, new_end, MASK_SHOT | CONTENTS_GRATE, nullptr, exit_trace);

		if (exit_trace->startsolid && (exit_trace->surface.flags & SURF_HITBOX) < 0)
		{
			traceIt(end, start, MASK_SHOT_HULL, reinterpret_cast<C_BasePlayer*>(exit_trace->m_pEnt), exit_trace);

			if (exit_trace->DidHit() && !exit_trace->startsolid)
			{
				end = exit_trace->endpos;
				return true;
			}
			continue;
		}

		if (!exit_trace->DidHit() || exit_trace->startsolid)
		{
			if (enter_trace->m_pEnt)
			{
				if (enter_trace->DidHitNonWorldEntity() && IsBreakableEntity(reinterpret_cast<C_BasePlayer*>(enter_trace->m_pEnt)))
				{
					*exit_trace = *enter_trace;
					exit_trace->endpos = start + dir;
					return true;
				}
			}
			continue;
		}

		if ((exit_trace->surface.flags >> 7) & SURF_LIGHT)
		{
			if (IsBreakableEntity(reinterpret_cast<C_BasePlayer*>(exit_trace->m_pEnt)) && IsBreakableEntity(reinterpret_cast<C_BasePlayer*>(enter_trace->m_pEnt)))
			{
				end = exit_trace->endpos;
				return true;
			}

			if (!((enter_trace->surface.flags >> 7) & SURF_LIGHT))
				continue;
		}

		if (exit_trace->plane.normal.Dot(dir) <= 1.0f)
		{
			float fraction = exit_trace->fraction * 4.0f;
			end = end - (dir * fraction);

			return true;
		}
	}
	return false;
}

bool HandleBulletPenetration(CCSWeaponInfo *wpn_data, Autowall::FireBulletData &data)
{
	bool bSolidSurf = ((data.enter_trace.contents >> 3) & CONTENTS_SOLID);
	bool bLightSurf = (data.enter_trace.surface.flags >> 7) & SURF_LIGHT;

	surfacedata_t *enter_surface_data = pPhysics->GetSurfaceData(data.enter_trace.surface.surfaceProps);
	unsigned short enter_material = enter_surface_data->game.material;
	float enter_surf_penetration_mod = enter_surface_data->game.flPenetrationModifier;

	if (!data.penetrate_count && !bLightSurf && !bSolidSurf && enter_material != 89)
	{
		if (enter_material != 71)
			return false;
	}

	if (data.penetrate_count <= 0 || wpn_data->GetPenetration() <= 0.f)
		return false;

	Vector dummy;
	trace_t trace_exit;

	if (!TraceToExit(dummy, &data.enter_trace, data.enter_trace.endpos, data.direction, &trace_exit))
	{
		if (!(pTrace->GetPointContents(dummy, MASK_SHOT_HULL) & MASK_SHOT_HULL))
			return false;
	}

	surfacedata_t *exit_surface_data = pPhysics->GetSurfaceData(trace_exit.surface.surfaceProps);
	unsigned short exit_material = exit_surface_data->game.material;

	float exit_surf_penetration_mod = exit_surface_data->game.flPenetrationModifier;
	float exit_surf_damage_mod = exit_surface_data->game.flDamageModifier;

	float final_damage_modifier = 0.16f;
	float combined_penetration_modifier = 0.0f;

	if (enter_material == 89 || enter_material == 71)
	{
		combined_penetration_modifier = 3.0f;
		final_damage_modifier = 0.05f;
	}
	else if (bSolidSurf || bLightSurf)
	{
		combined_penetration_modifier = 1.0f;
		final_damage_modifier = 0.16f;
	}
	else
	{
		combined_penetration_modifier = (enter_surf_penetration_mod + exit_surf_penetration_mod) * 0.5f;
	}

	if (enter_material == exit_material)
	{
		if (exit_material == 87 || exit_material == 85)
			combined_penetration_modifier = 3.0f;
		else if (exit_material == 76)
			combined_penetration_modifier = 2.0f;
	}

	float modifier = fmaxf(0.0f, 1.0f / combined_penetration_modifier);
	float thickness = (trace_exit.endpos - data.enter_trace.endpos).LengthSqr();
	float taken_damage = ((modifier * 3.0f) * fmaxf(0.0f, (3.0f / wpn_data->GetPenetration()) * 1.25f) + (data.current_damage * final_damage_modifier)) + ((thickness * modifier) / 24.0f);

	float lost_damage = fmaxf(0.0f, taken_damage);

	if (lost_damage > data.current_damage)
		return false;

	if (lost_damage > 0.0f)
		data.current_damage -= lost_damage;

	if (data.current_damage < 1.0f)
		return false;

	data.src = trace_exit.endpos;
	data.penetrate_count--;

	return true;
}

bool SimulateFireBullet(C_BaseCombatWeapon *weap, Autowall::FireBulletData &data, C_BasePlayer *player, CSGOHitbox hitbox, bool bt = false, Vector aimSpot = Vector(0, 0, 0))
{
	if (!weap)
		return false;

	auto GetHitgroup = [](CSGOHitbox hitbox) -> HitGroups
	{
		switch (hitbox)
		{
		case CSGOHitbox::HITBOX_HEAD:
		case CSGOHitbox::HITBOX_NECK:
			return HitGroups::HITGROUP_HEAD;
		case CSGOHitbox::HITBOX_LOWER_CHEST:
		case CSGOHitbox::HITBOX_THORAX:
		case CSGOHitbox::HITBOX_UPPER_CHEST:
			return HitGroups::HITGROUP_CHEST;
		case CSGOHitbox::HITBOX_BELLY:
		case CSGOHitbox::HITBOX_PELVIS:
			return HitGroups::HITGROUP_STOMACH;
		case CSGOHitbox::HITBOX_LEFT_HAND:
		case CSGOHitbox::HITBOX_LEFT_UPPER_ARM:
		case CSGOHitbox::HITBOX_LEFT_FOREARM:
			return HitGroups::HITGROUP_LEFTARM;
		case CSGOHitbox::HITBOX_RIGHT_HAND:
		case CSGOHitbox::HITBOX_RIGHT_UPPER_ARM:
		case CSGOHitbox::HITBOX_RIGHT_FOREARM:
			return HitGroups::HITGROUP_RIGHTARM;
		case CSGOHitbox::HITBOX_LEFT_THIGH:
		case CSGOHitbox::HITBOX_LEFT_CALF:
		case CSGOHitbox::HITBOX_LEFT_FOOT:
			return HitGroups::HITGROUP_LEFTLEG;
		case CSGOHitbox::HITBOX_RIGHT_THIGH:
		case CSGOHitbox::HITBOX_RIGHT_CALF:
		case CSGOHitbox::HITBOX_RIGHT_FOOT:
			return HitGroups::HITGROUP_RIGHTLEG;
		default:
			return HitGroups::HITGROUP_GEAR;
		}
	};

	data.penetrate_count = 4;
	data.trace_length = 0.0f;

	C_BaseCombatWeapon* pActiveWeapon = (C_BaseCombatWeapon*)pEntityList->GetClientEntityFromHandle(Globals::pLocal->GetActiveWeapon());

	CCSWeaponInfo *weaponData = pActiveWeapon->GetCSWpnData();

	if (!weaponData)
		return false;

	data.current_damage = (float)weaponData->GetDamage();

	while ((data.penetrate_count > 0) && (data.current_damage >= 1.0f))
	{
		data.trace_length_remaining = weaponData->GetRange() - data.trace_length;

		Vector end = data.src + data.direction * data.trace_length_remaining;

		traceIt(data.src, end, MASK_SHOT | CONTENTS_GRATE, Globals::pLocal, &data.enter_trace);
		ClipTraceToPlayers(data.src, end + data.direction * 40.f, MASK_SHOT | CONTENTS_GRATE, &data.filter, &data.enter_trace);

		if (data.enter_trace.fraction == 1.0f)
		{
			if (player && !(player->GetFlags() & FL_ONGROUND))
			{
				data.enter_trace.hitgroup = GetHitgroup(hitbox);
				data.enter_trace.m_pEnt = player;
			}
			else
				break;
		}

		surfacedata_t *enter_surface_data = pPhysics->GetSurfaceData(data.enter_trace.surface.surfaceProps);
		int enter_material = enter_surface_data->game.material;
		float enter_surf_penetration_mod = enter_surface_data->game.flPenetrationModifier;

		data.trace_length += data.enter_trace.fraction * data.trace_length_remaining;
		data.current_damage *= pow(weaponData->GetRangeModifier(), data.trace_length * 0.002);

		if (data.trace_length > 3000.f && weaponData->GetPenetration() > 0.f || enter_surf_penetration_mod < 0.1f)
			break;

		if (bt) {
			if ((data.enter_trace.endpos - data.src).Length() >= (aimSpot - data.src).Length() || data.enter_trace.m_pEnt) {
				C_BasePlayer *pPlayer = (C_BasePlayer*)data.enter_trace.m_pEnt;
				if (!pPlayer)
					pPlayer = player;

				if (pPlayer->IsPlayer() && pPlayer->GetTeam() == Globals::pLocal->GetTeam())
					return false;

				// p backtrack fix
				if (data.enter_trace.hitgroup == HitGroups::HITGROUP_GENERIC || data.enter_trace.hitgroup == HitGroups::HITGROUP_GEAR)
					data.enter_trace.hitgroup = GetHitgroup(hitbox);

				ScaleDamage(data.enter_trace.hitgroup, pPlayer, weaponData->GetWeaponArmorRatio(), data.current_damage);

				return true;
			}
		}
		else {
			if ((data.enter_trace.hitgroup <= HitGroups::HITGROUP_RIGHTLEG) && (data.enter_trace.hitgroup > HitGroups::HITGROUP_GENERIC))
			{
				C_BasePlayer *pPlayer = (C_BasePlayer*)data.enter_trace.m_pEnt;
				if (pPlayer->IsPlayer() && pPlayer->GetTeam() == Globals::pLocal->GetTeam())
					return false;

				ScaleDamage(data.enter_trace.hitgroup, pPlayer, weaponData->GetWeaponArmorRatio(), data.current_damage);

				return true;
			}
		}

		if (!HandleBulletPenetration(weaponData, data))
			break;
	}

	return false;
}

float Autowall::GetDamageVec(const Vector &vecPoint, C_BasePlayer *player, CSGOHitbox hitbox, bool bt)
{
	float damage = 0.f;

	Vector rem = vecPoint;
	
	FireBulletData data;

	data.src = Globals::pLocal->GetEyePosition();
	data.filter.pSkip = Globals::pLocal;

	Vector angle = Math::CalcAngle(data.src, rem);
	Math::AngleVector(angle, data.direction);

	data.direction.Normalized();

	C_BaseCombatWeapon* pActiveWeapon = (C_BaseCombatWeapon*)pEntityList->GetClientEntityFromHandle(Globals::pLocal->GetActiveWeapon());

	if (SimulateFireBullet(pActiveWeapon, data, player, hitbox, bt, vecPoint))
		damage = data.current_damage;

	return damage;
}
