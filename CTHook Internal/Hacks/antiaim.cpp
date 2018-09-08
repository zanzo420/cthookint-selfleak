#include "antiaim.h"

bool Settings::AntiAim::Yaw::enabled = true;
bool Settings::AntiAim::Pitch::enabled = true;
AntiAimYType Settings::AntiAim::Yaw::real = AntiAimYType::AUTISM;
float Settings::AntiAim::Yaw::realAdd = 0.f;
AntiAimYType Settings::AntiAim::Yaw::fake = AntiAimYType::MASTERLOOSER;
float Settings::AntiAim::Yaw::fakeAdd = 0.f;
int Settings::AntiAim::Yaw::spinSpeed = 1;
AntiAimXType Settings::AntiAim::Pitch::pitch = AntiAimXType::DOWN;
bool Settings::AntiAim::Yaw::antiResolver = true;
bool Settings::AntiAim::antiUt = true;
bool Settings::AntiAim::Roll::enabled = false;
float Settings::AntiAim::Roll::angle = 0;

AntiAimLBYType Settings::AntiAim::LBY::type = AntiAimLBYType::FAKE;
float Settings::AntiAim::LBY::add = 0.f;
bool Settings::AntiAim::LBY::preBreak = false;
AntiAimLBYType Settings::AntiAim::LBY::preBreakType = AntiAimLBYType::BACK;
float Settings::AntiAim::LBY::preBreakAdd = 180.f;

bool Settings::AntiAim::AntiLastMove::enabled = true;
AntiAimLBYType Settings::AntiAim::AntiLastMove::type = AntiAimLBYType::FAKE;
float Settings::AntiAim::AntiLastMove::add = 0.f;

float AntiAim::lastReal;
float AntiAim::lastFake;
bool AntiAim::doingAA;
bool AntiAim::lbyUpdated;
bool AntiAim::lbyUpdatedd;
float AntiAim::nextLBYUpdate;

void Friction(Vector &outVel);
void WalkMove(C_BasePlayer *player, Vector &outVel);
float get_curtime(CUserCmd* ucmd) {
	auto local_player = (C_BasePlayer*)pEntityList->GetClientEntity(pEngine->GetLocalPlayer());

	if (!local_player)
		return 0;

	int g_tick = 0;
	CUserCmd* g_pLastCmd = nullptr;
	if (!g_pLastCmd || g_pLastCmd->hasbeenpredicted) {
		g_tick = local_player->GetTickBase();
	}
	else {
		++g_tick;
	}
	g_pLastCmd = ucmd;
	float curtime = g_tick * pGlobalVars->interval_per_tick;
	return curtime;
}
float local_update;
bool next_lby_update(CUserCmd* m_pcmd) {
	auto local_player = (C_BasePlayer*)pEntityList->GetClientEntity(pEngine->GetLocalPlayer());
	if (local_player) {
		static float next_lby_update_time;
		const float current_time = get_curtime(m_pcmd);
		local_update = next_lby_update_time;
		static bool in_air;
		static bool stop;
		stop = local_player->GetVelocity().Length2D() < 0.1 && local_player->GetFlags() & FL_ONGROUND;
		in_air = !(local_player->GetFlags() & FL_ONGROUND);
		if (stop  || in_air) {
			if ((next_lby_update_time < current_time)) {
				next_lby_update_time = current_time + 1.1f;
				return true;
			}
		}
	}
	return false;
}
void normalize(Vector &vIn, Vector &vOut)
{
	float flLen = vIn.Length();
	if (flLen == 0) {
		vOut.Init(0, 0, 1);
		return;
	}
	flLen = 1 / flLen;
	vOut.Init(vIn.x * flLen, vIn.y * flLen, vIn.z * flLen);
}
void angle_vectors(const Vector &angles, Vector& forward)
{
	Assert(s_bMathlibInitialized);
	Assert(forward);

	float	sp, sy, cp, cy;

	sy = sin(DEG2RAD(angles[1]));
	cy = cos(DEG2RAD(angles[1]));

	sp = sin(DEG2RAD(angles[0]));
	cp = cos(DEG2RAD(angles[0]));

	forward.x = cp * cy;
	forward.y = cp * sy;
	forward.z = -sp;
}
void inline sincos(float radians, float *sine, float *cosine)
{
	*sine = sin(radians);
	*cosine = cos(radians);
}
void angle_vectors(const Vector &angles, Vector *forward) {
	float sp, sy, cp, cy;

	sincos(DEG2RAD(angles[1]), &sy, &cy);
	sincos(DEG2RAD(angles[0]), &sp, &cp);

	forward->x = cp * cy;
	forward->y = cp * sy;
	forward->z = -sp;
}
void angle_vectors(const Vector &angles, Vector *forward, Vector *right, Vector *up)
{
	float sr, sp, sy, cr, cp, cy;

	sincos(DEG2RAD(angles[1]), &sy, &cy);
	sincos(DEG2RAD(angles[0]), &sp, &cp);
	sincos(DEG2RAD(angles[2]), &sr, &cr);

	if (forward)
	{
		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}

	if (right)
	{
		right->x = (-1 * sr*sp*cy + -1 * cr*-sy);
		right->y = (-1 * sr*sp*sy + -1 * cr*cy);
		right->z = -1 * sr*cp;
	}

	if (up)
	{
		up->x = (cr*sp*cy + -sr * -sy);
		up->y = (cr*sp*sy + -sr * cy);
		up->z = cr * cp;
	}
}
FORCEINLINE vec_t DotProduct(const Vector& a, const Vector& b)
{
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}

void vector_transform(const Vector in1, float in2[3][4], Vector &out)
{
	out[0] = DotProduct(in1, Vector(in2[0][0], in2[0][1], in2[0][2])) + in2[0][3];
	out[1] = DotProduct(in1, Vector(in2[1][0], in2[1][1], in2[1][2])) + in2[1][3];
	out[2] = DotProduct(in1, Vector(in2[2][0], in2[2][1], in2[2][2])) + in2[2][3];
}
Vector get_hitbox_location(C_BasePlayer* obj, int hitbox_id) {
	matrix3x4 bone_matrix[128];

	if (obj->SetupBones2(bone_matrix, 128, 0x00000100, 0.0f)) {
		if (obj->GetModel()) {
			auto studio_model = pModelInfo->GetStudioModel(obj->GetModel());
			if (studio_model) {
				auto hitbox = studio_model->pHitboxSet(0)->pHitbox(hitbox_id);
				if (hitbox) {
					auto min = Vector{}, max = Vector{};

					vector_transform(hitbox->bbmin, bone_matrix[hitbox->bone], min);
					vector_transform(hitbox->bbmax, bone_matrix[hitbox->bone], max);

					return (min + max) / 2.0f;
				}
			}
		}
	}
	return Vector{};
}
void calculate_angle(Vector src, Vector dst, Vector &angles)
{
	Vector delta = src - dst;
	double hyp = delta.Length2D();
	angles.y = (atan(delta.y / delta.x) * 57.295779513082f);
	angles.x = (atan(delta.z / hyp) * 57.295779513082f);
	angles[2] = 0.0f;
	if (delta.x >= 0.0) angles.y += 180.0f;
}
FORCEINLINE void vector_subtract(const Vector& a, const Vector& b, Vector& c)
{
	c.x = a.x - b.x;
	c.y = a.y - b.y;
	c.z = a.z - b.z;
}
bool is_viable_target(C_BasePlayer* pEntity)
{
	if (!pEntity) return false;
	if (pEntity->GetClientClass()->m_ClassID != EClassIds::CCSPlayer) return false;
	if (pEntity == Globals::pLocal) return false;
	if (pEntity->GetTeam() == Globals::pLocal->GetTeam()) return false;
	if (!pEntity->GetAlive() || pEntity->GetDormant()) return false;
	return true;
}
bool screen_transform(const Vector& point, Vector& screen)
{
	const VMatrix& w2sMatrix = pEngine->WorldToScreenMatrix();
	screen.x = w2sMatrix[0][0] * point.x + w2sMatrix[0][1] * point.y + w2sMatrix[0][2] * point.z + w2sMatrix[0][3];
	screen.y = w2sMatrix[1][0] * point.x + w2sMatrix[1][1] * point.y + w2sMatrix[1][2] * point.z + w2sMatrix[1][3];
	screen.z = 0.0f;

	float w = w2sMatrix[3][0] * point.x + w2sMatrix[3][1] * point.y + w2sMatrix[3][2] * point.z + w2sMatrix[3][3];

	if (w < 0.001f) {
		screen.x *= 100000;
		screen.y *= 100000;
		return true;
	}

	float invw = 1.0f / w;
	screen.x *= invw;
	screen.y *= invw;

	return false;
}
bool world_to_screen(const Vector &origin, Vector &screen)
{
	if (!screen_transform(origin, screen)) {
		int iScreenWidth, iScreenHeight;
		pEngine->GetScreenSize(iScreenWidth, iScreenHeight);

		screen.x = (iScreenWidth / 2.0f) + (screen.x * iScreenWidth) / 2;
		screen.y = (iScreenHeight / 2.0f) - (screen.y * iScreenHeight) / 2;

		return true;
	}
	return false;
}

// this needs better name
float AntiAim::DetectsThemWalls(float range)
{
	float Back, Right, Left;
	Vector src3D, dst3D, forward, right, up;
	trace_t tr;
	Ray_t ray, ray2, ray3;
	CTraceFilter filter;

	//C_BasePlayer* pLocal = (C_BasePlayer*)pEntityList->GetClientEntity(pEngine->GetLocalPlayer());
	if (!Globals::pLocal)
		return 0.f;

	Vector engineViewAngles;

	pEngine->GetViewAngles(engineViewAngles);

	engineViewAngles.x = 0;

	Math::AngleVectors(engineViewAngles, forward, right, up);

	filter.pSkip = Globals::pLocal;
	src3D = Globals::pLocal->GetEyePosition();
	dst3D = src3D + (forward * range);

	ray.Init(src3D, dst3D);
	pTrace->TraceRay(ray, MASK_ALL, &filter, &tr);
	Back = (tr.endpos - tr.startpos).Length();

	ray2.Init(src3D + right * 20, dst3D + right * 20);
	pTrace->TraceRay(ray2, MASK_ALL, &filter, &tr);
	Right = (tr.endpos - tr.startpos).Length();

	ray3.Init(src3D - right * 20, dst3D - right * 20);
	pTrace->TraceRay(ray3, MASK_ALL, &filter, &tr);
	Left = (tr.endpos - tr.startpos).Length();

	if (Back < Left && Back < Right) // put your body backwards
		return 180.f;
	else if (Left < Right) // put your body left
		return 90.f;
	else if (Right < Left) // put your body right
		return -90.f;
	else // If anything is not the best just do backwards
		return 180.f;
}
mstudiobbox_t* get_hitbox(C_BasePlayer* entity, int hitbox_index)
{
	if (entity->GetDormant() || entity->GetHealth() <= 0)
		return NULL;

	const auto pModel = entity->GetModel();
	if (!pModel)
		return NULL;

	auto pStudioHdr = pModelInfo->GetStudioModel(pModel);
	if (!pStudioHdr)
		return NULL;

	auto pSet = pStudioHdr->pHitboxSet(0);
	if (!pSet)
		return NULL;

	if (hitbox_index >= pSet->numhitboxes || hitbox_index < 0)
		return NULL;

	return pSet->pHitbox(hitbox_index);
}

Vector get_hitbox_pos(C_BasePlayer* entity, int hitbox_id)
{
	auto hitbox = get_hitbox(entity, hitbox_id);
	if (!hitbox)
		return Vector(0, 0, 0);

	auto bone_matrix = entity->GetBoneMatrix(hitbox->bone);

	Vector bbmin, bbmax;
	Math::VectorTransform(hitbox->bbmin, bone_matrix, bbmin);
	Math::VectorTransform(hitbox->bbmax, bone_matrix, bbmax);

	return (bbmin + bbmax) * 0.5f;
}
void NormalizeNum(Vector &vIn, Vector &vOut)
{
	float flLen = vIn.Length();
	if (flLen == 0) {
		vOut.Init(0, 0, 1);
		return;
	}
	flLen = 1 / flLen;
	vOut.Init(vIn.x * flLen, vIn.y * flLen, vIn.z * flLen);
}
void inline SinCos(float radians, float *sine, float *cosine)
{
	*sine = sin(radians);
	*cosine = cos(radians);

}
void AngleVectors(const Vector &angles, Vector *forward)
{
	float	sp, sy, cp, cy;

	SinCos(DEG2RAD(angles[1]), &sy, &cy);
	SinCos(DEG2RAD(angles[0]), &sp, &cp);

	forward->x = cp * cy;
	forward->y = cp * sy;
	forward->z = -sp;
}
float fov_player(Vector ViewOffSet, Vector View, C_BasePlayer* entity, int hitbox)
{
	// Anything past 180 degrees is just going to wrap around
	CONST FLOAT MaxDegrees = 180.0f;

	// Get local angles
	Vector Angles = View;

	// Get local view / eye position
	Vector Origin = ViewOffSet;

	// Create and intiialize vectors for calculations below
	Vector Delta(0, 0, 0);
	//Vector Origin(0, 0, 0);
	Vector Forward(0, 0, 0);

	// Convert angles to normalized directional forward vector
	AngleVectors(Angles, &Forward);

	Vector AimPos = get_hitbox_pos(entity, hitbox); //pvs fix disabled

	VectorSubtract(AimPos, Origin, Delta);
	//Delta = AimPos - Origin;

	// Normalize our delta vector
	NormalizeNum(Delta, Delta);

	// Get dot product between delta position and directional forward vectors
	FLOAT DotProduct = Forward.Dot(Delta);

	// Time to calculate the field of view
	return (acos(DotProduct) * (MaxDegrees / M_PI));
}

int closest_to_crosshair()
{
	int index = -1;
	float lowest_fov = INT_MAX;

	C_BasePlayer* local_player = (C_BasePlayer*)pEntityList->GetClientEntity(pEngine->GetLocalPlayer());

	if (!local_player)
		return -1;

	Vector local_position = local_player->GetEyePosition();

	Vector angles;
	pEngine->GetViewAngles(angles);

	for (int i = 1; i <= pGlobalVars->maxClients; i++)
	{
		C_BasePlayer *entity = (C_BasePlayer*)pEntityList->GetClientEntity(i);

		if (!entity || !entity->GetAlive() || entity->GetTeam() == local_player->GetTeam() || entity->GetDormant() || entity == local_player)
			continue;

		float fov = fov_player(local_position, angles, entity, 0);

		if (fov < lowest_fov)
		{
			lowest_fov = fov;
			index = i;
		}
	}

	return index;
}

void freestand(CUserCmd* cmd, Vector& angle)
{
	auto local_player = (C_BasePlayer*)pEntityList->GetClientEntity(pEngine->GetLocalPlayer());

	if (!local_player)
		return;

	static float last_real;
	bool no_active = true;
	float bestrotation = 0.f;
	float highestthickness = 0.f;
	Vector besthead;

	auto leyepos = local_player->GetVecOrigin() + local_player->GetVecViewOffset();
	auto headpos = get_hitbox_pos(local_player, 0);
	auto origin = local_player->GetAbsOrigin();

	auto checkWallThickness = [&](C_BasePlayer* pPlayer, Vector newhead) -> float
	{
		Vector endpos1, endpos2;
		Vector eyepos = pPlayer->GetVecOrigin() + pPlayer->GetVecViewOffset();

		Ray_t ray;
		ray.Init(newhead, eyepos);

		CTraceFilterSkipTwoEntities filter(pPlayer, local_player);

		trace_t trace1, trace2;
		pTrace->TraceRay(ray, MASK_SHOT_BRUSHONLY, &filter, &trace1);

		if (trace1.DidHit())
			endpos1 = trace1.endpos;
		else
			return 0.f;

		ray.Init(eyepos, newhead);
		pTrace->TraceRay(ray, MASK_SHOT_BRUSHONLY, &filter, &trace2);

		if (trace2.DidHit())
			endpos2 = trace2.endpos;

		float add = newhead.DistTo(eyepos) - leyepos.DistTo(eyepos) + 3.f;
		return endpos1.DistTo(endpos2) + add / 3;
	};

	int index = closest_to_crosshair();

	static C_BasePlayer* entity;

	if (index != -1)
		entity = (C_BasePlayer*)pEntityList->GetClientEntity(index);

	float step = (2 * M_PI) / 18.f; // One PI = half a circle ( for stacker cause low iq :sunglasses: ), 28

	float radius = fabs(Vector(headpos - origin).Length2D());

	if (index == -1)
	{
		no_active = true;
	}
	else
	{
		for (float rotation = 0; rotation < (M_PI * 2.0); rotation += step)
		{
			Vector newhead(radius * cos(rotation) + leyepos.x, radius * sin(rotation) + leyepos.y, leyepos.z);

			float totalthickness = 0.f;

			no_active = false;

			totalthickness += checkWallThickness(entity, newhead);

			if (totalthickness > highestthickness)
			{
				highestthickness = totalthickness;
				bestrotation = rotation;
				besthead = newhead;
			}
		}
	}

	int ServerTime = (float)Globals::pLocal->GetTickBase() * pGlobalVars->interval_per_tick;
	int value = ServerTime % 2;

	static int Ticks = 120;

	//auto air_value = fmod(Interfaces.pGlobalVars->curtime / 0.9f /*speed*/ * 130.0f, 90.0f); // first number is where the angle will end and how fast it goes about it, adding to the second number which is the starting angle.
	//  auto stand_value = fmod(Interfaces.pGlobalVars->curtime /*speed can go here too*/ * 120.0f, 70.0f); // again, first number is where the angle will end and how fast it goes about it, adding to the second number which is the starting angle.
	//pCmd->viewangles.y += Hacks.LocalPlayer->GetFlags() & FL_ONGROUND ? NextLBYUpdate() ? 165.0f - 35 : stand_value + 145.0f : 90 + air_value;

	if (no_active)
	{
		angle.y -= Ticks; // 180z using ticks
		Ticks += 4;

		if (Ticks > 240)
			Ticks = 120;
	}
	else
		angle.y = RAD2DEG(bestrotation);
}
float WallThickness(Vector from, Vector to, C_BasePlayer* skip, C_BasePlayer* skip2)
{
	Vector endpos1, endpos2;

	Ray_t ray;
	ray.Init(from, to);

	CTraceFilterSkipTwoEntities filter(skip, skip2);

	trace_t trace1, trace2;
	pTrace->TraceRay(ray, MASK_SHOT_BRUSHONLY, &filter, &trace1);

	if (trace1.DidHit())
		endpos1 = trace1.endpos;
	else
		return -1.f;

	ray.Init(to, from);
	pTrace->TraceRay(ray, MASK_SHOT_BRUSHONLY, &filter, &trace2);

	if (trace2.DidHit())
		endpos2 = trace2.endpos;

	return endpos1.DistTo(endpos2);
}
void AntiAimY(Vector& angle, bool bFlip,CUserCmd*pCmd) {
	AntiAimYType aa = bFlip ? Settings::AntiAim::Yaw::fake : Settings::AntiAim::Yaw::real;

	//C_BasePlayer* pLocal = (C_BasePlayer*)pEntityList->GetClientEntity(pEngine->GetLocalPlayer());
	float velocity = Globals::pLocal->GetVelocity().Length2D();

	switch (aa) {
		case AntiAimYType::BACKWARDS: {
			angle.y += 180.0f;
			break;
		}
		case AntiAimYType::LBY: {
			angle.y = *Globals::pLocal->GetLowerBodyYawTarget();
			break;
		}
		case AntiAimYType::FREESTANDING: {
			float hideHeadAng = AntiAim::DetectsThemWalls(200);

			Vector viewAngles;
			pEngine->GetViewAngles(viewAngles);

			//*pLocal->GetLowerBodyYawTarget() = viewAngles.y + 180 + 360 * 5;

			angle.y = viewAngles.y + hideHeadAng;
		}
		case AntiAimYType::RANDOMSPIN: {
			/*int gay = rand() % 100 + 1;
			float curTime = pGlobalVars->curtime;
			float gayyy = (int)curTime % 361;
			gayyy *= 1.1;
			if (gay < 50)
				angle.y = *pLocal->GetLowerBodyYawTarget() + gayyy;
			else
				angle.y = *pLocal->GetLowerBodyYawTarget() - gayyy;
			*/
			/*int rng = rand() % 100 + 1;
			int rng2 = rand() % 100 + 1;
			int serverTime = pLocal->GetTickBase() * pGlobalVars->interval_per_tick;
			static int count;

			if (serverTime != serverTime)
				count += Settings::AntiAim::Yaw::spinSpeed;

			if (count > 360)
				count = 0;
				
			static bool g = false;
			if (rng > rng2)
				g = !g;

			angle.y += g ? count : -count;*/

			float factor = 360.0 / M_PHI;
			factor *= Settings::AntiAim::Yaw::spinSpeed / 1000;
			angle.y = fmodf(pGlobalVars->curtime * factor, 360.0);

			break;
		}
		case AntiAimYType::MASTERLOOSER: {
			/*if (bFlip)
				angle.y = AntiAim::lastReal + rand() % 181 + 120;
			else
				angle.y = AntiAim::lastFake + rand() % 181 + 120;*/

			//int cur = pGlobalVars->curtime * 2;

			static bool flip = false;
			flip = !flip;

			angle.y = AntiAim::lastReal;
			int rng = rand() % 40 + 80;
			angle.y += flip ? rng : -rng;

			//angle.y = *pLocal->GetLowerBodyYawTarget() + cur % 360;
			break;
		}
		case AntiAimYType::AUTISM: {
			freestand(pCmd, angle);
			break;
		}
		default:
			break;
	}
}

void AntiAimX(Vector& angle) {
	switch (Settings::AntiAim::Pitch::pitch) {
		case AntiAimXType::DOWN:
			angle.x = 89.f;
			break;
		case AntiAimXType::FAKEUP:
			angle.x = -179.f;
			break;
		default:
			break;
	}
}

float AntiAimLBY(AntiAimLBYType type, float add) {
	Vector viewAngles;
	pEngine->GetViewAngles(viewAngles);

	float lby = *Globals::pLocal->GetLowerBodyYawTarget();

	switch (type) {
		case AntiAimLBYType::BACK:
			lby = viewAngles.y + 180.f;
			break;
		case AntiAimLBYType::FAKE:
			lby = AntiAim::lastFake;
			break;
		case AntiAimLBYType::REAL:
			lby = AntiAim::lastReal;
			break;
	}

	lby += add;

	return lby;
}

float GetCurTime(CUserCmd* ucmd) {
	//C_BasePlayer* pLocal = (C_BasePlayer*)pEntityList->GetClientEntity(pEngine->GetLocalPlayer());
	
	static int g_tick = 0;
	static CUserCmd* g_pLastCmd = nullptr;
	if (!g_pLastCmd || g_pLastCmd->hasbeenpredicted) {
		g_tick = Globals::pLocal->GetTickBase();
	}
	else {
		// Required because prediction only runs on frames, not ticks
		// So if your framerate goes below tickrate, m_nTickBase won't update every tick
		++g_tick;
	}
	g_pLastCmd = ucmd;
	float curtime = g_tick * pGlobalVars->interval_per_tick;
	return curtime;
}

void AntiAim::CreateMove(CUserCmd* pCmd) {
	if (!(*pGameRules))
		return;
	
	if (!Settings::AntiAim::Yaw::enabled || !Settings::AntiAim::Pitch::enabled)
		return;

	if (!pEngine->IsInGame())
		return;

	AntiAim::doingAA = false;

	Vector oldAngle = pCmd->viewangles;
	float oldForward = pCmd->forwardmove;
	float oldSideMove = pCmd->sidemove;

	//C_BasePlayer* pLocal = (C_BasePlayer*)pEntityList->GetClientEntity(pEngine->GetLocalPlayer());

	if (!Globals::pLocal || !Globals::pLocal->GetAlive())
		return;

	if (Globals::pLocal->GetMoveType() == MOVETYPE_LADDER || Globals::pLocal->GetMoveType() == MOVETYPE_NOCLIP)
		return;

	C_BaseCombatWeapon* pActiveWeapon = (C_BaseCombatWeapon*)pEntityList->GetClientEntityFromHandle(Globals::pLocal->GetActiveWeapon());

	if (!pActiveWeapon)
		return;

	if (pActiveWeapon->GetCSWpnData()->GetWeaponType() == CSWeaponType::WEAPONTYPE_GRENADE)
	{
		C_BaseCSGrenade* pCsGrenade = (C_BaseCSGrenade*)pActiveWeapon;

		if (pCsGrenade->GetThrowTime() > 0.f)
			return;
	}

	if (pCmd->buttons & IN_USE)
		return;

	if (*pActiveWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER) {
		if (Settings::Aimbot::autoRevolver && AimBot::shootingRevolver)
			return;
		else if (pCmd->buttons & IN_ATTACK2)
			return;
	}
	else if (*pActiveWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_KNIFE && pCmd->buttons & IN_ATTACK2)
		return;
	else if (pCmd->buttons & IN_ATTACK)
		return;

	static bool bFlip;

	bFlip = !bFlip;

	bool fLag = false;

	if (Settings::FakeLag::enabled && Globals::pLocal->GetVelocity().Length2D() > 1.f)
		fLag = true;
	else if (Settings::FakeLag::enabled && pInputSystem->IsButtonDown(Settings::FakeWalk::key))
		fLag = true;
	else
		fLag = false;

	if (fLag)
		bFlip = FakeLag::sendPacket;

	//if ((Settings::FakeLag::enabled || (Settings::FakeWalk::enabled && pInputSystem->IsButtonDown(Settings::FakeWalk::key)))
	//	|| (Settings::FakeLag::enabled && (pCmd->sidemove != 0 || pCmd->forwardmove != 0)))
	//	bFlip = FakeLag::sendPacket;

	Vector angle = pCmd->viewangles;

	static bool lbyUpdatedd = false;

	float velocity = Globals::pLocal->GetVelocity().Length2D();
	bool onGround = Globals::pLocal->GetFlags() & FL_ONGROUND;
	static float nextUpdate;
	float curTime = Globals::fixedCurTime;
	static bool preBreak;

	bool moving = false;

	static float lbySave;
	float lby = *Globals::pLocal->GetLowerBodyYawTarget();

	static bool firstBreak = false;

	INetChannelInfo *nci = pEngine->GetNetChannelInfo();
	auto ping = nci->GetLatency(FLOW_OUTGOING);

	if (lbySave == 0.f && lby != lbySave)
		lbySave = lby;

	if (velocity > 0.1f && onGround) {
		if (Settings::FakeWalk::enabled && pInputSystem->IsButtonDown(Settings::FakeWalk::key))
			nextUpdate = curTime + 1.1f;
		else {
			nextUpdate = curTime + 0.22f;
			AntiAim::lbyUpdated = true;
			moving = true;
			firstBreak = true;
		}
	}
	else if (nextUpdate <= curTime) {
		nextUpdate = curTime + 1.1f;
		lbyUpdatedd = true;
		AntiAim::lbyUpdated = true;
	}
	else if (lbySave != lby) {
		nextUpdate = curTime + 1.1f;

		lbySave = lby;
		AntiAim::lbyUpdated = true;
		firstBreak = true;
	}
	else {
		lbyUpdatedd = false;
		AntiAim::lbyUpdated = false;
	}

	nextLBYUpdate = nextUpdate;

	if (nextUpdate - pGlobalVars->interval_per_tick * 2 <= curTime)
		preBreak = true;
	else
		preBreak = false;

	AntiAim::lbyUpdatedd = lbyUpdatedd;

	if (fabsf(lby - AntiAim::lastReal) < 35.f)
		AntiAim::lbyUpdated = true;

	if (Settings::AntiAim::Pitch::enabled)
		AntiAimX(angle);

	//static bool flip = false;

	bool antiLast = false;

	Vector unpredVelocity = Globals::unpredictedVelocity;

	// broken either 0 or 15
	int ticks_to_stop;
	for (ticks_to_stop = 0; ticks_to_stop < 15; ticks_to_stop++)
	{
		if (unpredVelocity.Length2D() < 0.1f)
			break;

		if (Globals::pLocal->GetFlags() & FL_ONGROUND)
		{
			unpredVelocity[2] = 0.0f;
			Friction(unpredVelocity);
			WalkMove(Globals::pLocal, unpredVelocity);
		}
	}

	if (velocity > 1.f && velocity < 5.f)
		antiLast = true;

	if (Settings::AntiAim::Yaw::enabled) {
		//if (Settings::FakeLag::enabled || (Settings::FakeLag::manualChoke && pInputSystem->IsButtonDown(ButtonCode_t::KEY_X)) || (Settings::FakeWalk::enabled && pInputSystem->IsButtonDown(Settings::FakeWalk::key)))
		//	bFlip = SendPacket;
		AntiAimY(angle, bFlip, pCmd);

		angle.y += bFlip ? Settings::AntiAim::Yaw::fakeAdd : Settings::AntiAim::Yaw::realAdd;

		if (Settings::AntiAim::Yaw::antiResolver) {
			if (Settings::AntiAim::AntiLastMove::enabled && antiLast) {
				float lastMoveAng = AntiAimLBY(Settings::AntiAim::AntiLastMove::type, Settings::AntiAim::AntiLastMove::add);
				angle.y = lastMoveAng;
			}
			else {
				if (preBreak && Settings::AntiAim::LBY::preBreak) {
					// last unchoked tick before breaking
					float lbyPreBreakAng = AntiAimLBY(Settings::AntiAim::LBY::preBreakType, Settings::AntiAim::LBY::preBreakAdd);
					angle.y = lbyPreBreakAng;
					bFlip = false;
				}

				if (lbyUpdatedd) {
					static bool flip;
					flip = !flip;
					float lbyAng = AntiAimLBY(Settings::AntiAim::LBY::type, Settings::AntiAim::LBY::add);
					//float lbyFlip = flip ? lbyAng + 35 : lbyAng - 35;
					if (!firstBreak)
						lbyAng += flip ? 34.f : -34.f;
					angle.y = lbyAng;
					bFlip = false;

					// do this in prebreak
					// 979 supress?
					//if (Settings::AntiAim::LBY::supress) {
					//	INetChannelInfo *nci = pEngine->GetNetChannelInfo();
					//	nci->m_nOutSequenceNrAck += 150 * 3;
					//}
				}
			}
		}

		/*if (Settings::FakeLag::enabled || (Settings::FakeLag::manualChoke && pInputSystem->IsButtonDown(ButtonCode_t::KEY_X)) || (Settings::FakeWalk::enabled && pInputSystem->IsButtonDown(Settings::FakeWalk::key)))
			;
		else*/
		if (!fLag)
			SendPacket = bFlip;
	}

	if (Settings::AntiAim::Roll::enabled)
		angle.z = Settings::AntiAim::Roll::angle;

	if (Settings::AntiAim::antiUt) {
		Math::NormalizeAngles(angle);
		Math::ClampAngles(angle);
	}

	pCmd->viewangles = angle;

	AntiAim::doingAA = true;

	if (SendPacket)
		AntiAim::lastFake = pCmd->viewangles.y;
	else
		AntiAim::lastReal = pCmd->viewangles.y;

	Math::CorrectMovement(oldAngle, pCmd, oldForward, oldSideMove);
}

void Friction(Vector &outVel)
{
	float speed, newspeed, control;
	float friction;
	float drop;

	speed = outVel.Length();

	if (speed <= 0.1f)
		return;

	drop = 0;

	// apply ground friction
	if (Globals::pLocal->GetFlags() & FL_ONGROUND)
	{
		friction = pCvar->FindVar("sv_friction")->GetFloat() * Globals::pLocal->GetSurfaceFriction();

		// Bleed off some speed, but if we have less than the bleed
		// threshold, bleed the threshold amount.
		control = (speed < pCvar->FindVar("sv_stopspeed")->GetFloat()) ? pCvar->FindVar("sv_stopspeed")->GetFloat() : speed;

		// Add the amount to the drop amount.
		drop += control * friction * pGlobalVars->frametime;
	}

	newspeed = speed - drop;
	if (newspeed < 0)
		newspeed = 0;

	if (newspeed != speed)
	{
		// Determine proportion of old speed we are using.
		newspeed /= speed;
		// Adjust velocity according to proportion.
		VectorMultiply(outVel, newspeed, outVel);
	}
}

void Accelerate(C_BasePlayer *player, Vector &wishdir, float wishspeed, float accel, Vector &outVel)
{
	// See if we are changing direction a bit
	float currentspeed = outVel.Dot(wishdir);

	// Reduce wishspeed by the amount of veer.
	float addspeed = wishspeed - currentspeed;

	// If not going to add any speed, done.
	if (addspeed <= 0)
		return;

	// Determine amount of accleration.
	float accelspeed = accel * pGlobalVars->frametime * wishspeed * player->GetSurfaceFriction();

	// Cap at addspeed
	if (accelspeed > addspeed)
		accelspeed = addspeed;

	// Adjust velocity.
	for (int i = 0; i < 3; i++)
		outVel[i] += accelspeed * wishdir[i];
}

void WalkMove(C_BasePlayer *player, Vector &outVel)
{
	Vector forward, right, up, wishvel, wishdir, dest;
	float fmove, smove, wishspeed;

	Math::AngleVectors(*player->GetEyeAngles(), forward, right, up);  // Determine movement angles
																	   // Copy movement amounts
	pMoveHelper->SetHost(player);
	fmove = pMoveHelper->m_flForwardMove;
	smove = pMoveHelper->m_flSideMove;
	pMoveHelper->SetHost(nullptr);

	if (forward[2] != 0)
	{
		forward[2] = 0;
		Math::NormalizeVector(forward);
	}

	if (right[2] != 0)
	{
		right[2] = 0;
		Math::NormalizeVector(right);
	}

	for (int i = 0; i < 2; i++)	// Determine x and y parts of velocity
		wishvel[i] = forward[i] * fmove + right[i] * smove;

	wishvel[2] = 0;	// Zero out z part of velocity

	wishdir = wishvel; // Determine maginitude of speed of move
	wishspeed = wishdir.NormalizeFl();

	// Clamp to server defined max speed
	pMoveHelper->SetHost(player);
	if ((wishspeed != 0.0f) && (wishspeed > pMoveHelper->m_flMaxSpeed))
	{
		VectorMultiply(wishvel, player->GetMaxSpeed() / wishspeed, wishvel);
		wishspeed = player->GetMaxSpeed();
	}
	pMoveHelper->SetHost(nullptr);
	// Set pmove velocity
	outVel[2] = 0;
	Accelerate(player, wishdir, wishspeed, pCvar->FindVar("sv_accelerate")->GetFloat(), outVel);
	outVel[2] = 0;

	// Add in any base velocity to the current velocity.
	VectorAdd(outVel, player->GetBaseVelocity(), outVel);

	float spd = outVel.Length();

	if (spd < 1.0f)
	{
		outVel.Init();
		// Now pull the base velocity back out. Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
		VectorSubtract(outVel, player->GetBaseVelocity(), outVel);
		return;
	}

	pMoveHelper->SetHost(player);
	pMoveHelper->m_outWishVel += wishdir * wishspeed;
	pMoveHelper->SetHost(nullptr);

	// Don't walk up stairs if not on ground.
	if (!(player->GetFlags() & FL_ONGROUND))
	{
		// Now pull the base velocity back out.   Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
		VectorSubtract(outVel, player->GetBaseVelocity(), outVel);
		return;
	}

	// Now pull the base velocity back out. Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
	VectorSubtract(outVel, player->GetBaseVelocity(), outVel);
}