#include "backtrack.h"

bool Settings::Aimbot::backtrack = true;

std::vector<CTickRecord> BackTrack::backtrackRecords[64];
//std::deque<CTickRecord> BackTrack::backtrackRecords[64];

bool BackTrack::canBacktrackLby[64];
float lastSimTime[64];

template<class T, class U>
T clamp(T in, U low, U high)
{
	if (in <= low)
		return low;

	if (in >= high)
		return high;

	return in;
}

float BackTrack::GetLerpTime()
{
	int ud_rate = pCvar->FindVar("cl_updaterate")->GetInt();
	ConVar *min_ud_rate = pCvar->FindVar("sv_minupdaterate");
	ConVar *max_ud_rate = pCvar->FindVar("sv_maxupdaterate");

	if (min_ud_rate && max_ud_rate)
		ud_rate = max_ud_rate->GetInt();

	float ratio = pCvar->FindVar("cl_interp_ratio")->GetFloat();

	if (ratio == 0)
		ratio = 1.0f;

	float lerp = pCvar->FindVar("cl_interp")->GetFloat();
	ConVar *c_min_ratio = pCvar->FindVar("sv_client_min_interp_ratio");
	ConVar *c_max_ratio = pCvar->FindVar("sv_client_max_interp_ratio");

	if (c_min_ratio && c_max_ratio && c_min_ratio->GetFloat() != 1)
		ratio = clamp(ratio, c_min_ratio->GetFloat(), c_max_ratio->GetFloat());

	return max(lerp, (ratio / ud_rate));
}

bool BackTrack::IsTickValid(int tick)
{
	// better use polak's version than our old one, getting more accurate results

	INetChannelInfo *nci = pEngine->GetNetChannelInfo();

	if (!nci)
		return false;

	float sv_maxunlag = pCvar->FindVar("sv_maxunlag")->GetFloat();

	float correct = clamp(nci->GetLatency(FLOW_OUTGOING) + GetLerpTime(), 0.f, sv_maxunlag/*1.f*//*sv_maxunlag*/);

	float deltaTime = correct - (pGlobalVars->curtime - TICKS_TO_TIME(tick));

	return fabsf(deltaTime) < 0.2f;
}

bool TickValid(int tick) {
	float delta = 0 - (TICKS_TO_TIME(pGlobalVars->tickcount) - TICKS_TO_TIME(tick));

	return fabsf(delta) <= 0.2f;
}

void BackTrack::OverwriteTick(C_BasePlayer* pTarget, float correctTime, int priority, bool lbyFlick) {
	int id = pTarget->GetIndex();

	BackTrack::backtrackRecords[id].emplace_back(CTickRecord(pTarget, TIME_TO_TICKS(correctTime), priority, pGlobalVars->tickcount, lbyFlick));
}

// polak pcodenz
void AnimFix(C_BasePlayer* entity, CTickRecord record)
{
	float simTime; // ebx
	float curtime; // xmm0_4 MAPDST
	float frameTime; // xmm0_4
					 //int v6; // ecx
	int v7; // eax
	int v8; // edi
	int v9; // ecx
			//int v11; // edx
			//char v13; // [esp+10h] [ebp-348h]

	//	memcpy((int*)&v13, &*entity->GetRagdolPosPointer(), 56 * *reinterpret_cast<uintptr_t*>(uintptr_t(&entity->GetRagdolPos()) + ptrdiff_t(12)));
	//memcpy(&v13, *entity->GetRagdolPos(), 56 * ragpos + 12);
	//j_memcpy(&v13, *(m_ragPos + entity), 56 * *(m_ragPos + entity + 12));
	//GlobalVars = ::GlobalVars;

	// save data
	auto oldRagdoll = entity->GetRagdollPos();
	simTime = entity->GetSimulationTime();

	*entity->ClientSideAnimation() = true;
	curtime = pGlobalVars->curtime;
	pGlobalVars->curtime = simTime;
	frameTime = pGlobalVars->frametime;
	pGlobalVars->frametime = pGlobalVars->interval_per_tick;
	auto animstate = *reinterpret_cast<uintptr_t*>(uintptr_t(entity) + ptrdiff_t(0x3884));
	auto modeltime = reinterpret_cast<int*>(animstate + ptrdiff_t(112));
	if (animstate)
	{
		if (*modeltime == pGlobalVars->framecount)
			*modeltime = pGlobalVars->framecount - 1;

		v8 = entity->GetFlags();
		v9 = record.flags;
		*entity->GetFlagsPointer() = v9;
		if (v9 & 1
			&& !(BackTrack::backtrackRecords[entity->GetIndex()].end()[-2].flags & 1)
			&& (TIME_TO_TICKS(simTime - BackTrack::backtrackRecords[entity->GetIndex()].end()[-2].m_flSimulationTime) > 2))
		{
			*entity->GetFlagsPointer() = v9 & 0xFFFFFFFE;
		}
	}
	//(*(*entity + 0x368))(entity);
	//GlobalVars_1 = ::GlobalVars;
	//v11 = entity->GetRagdolPos();
	*entity->GetRagdollPosPointer() = oldRagdoll;	
	*entity->ClientSideAnimation() = false;

	pGlobalVars->curtime = curtime;
	pGlobalVars->frametime = frameTime;

	//memcpy(&*entity->GetRagdolPosPointer(), (int*)&v13, 56 * *reinterpret_cast<uintptr_t*>(uintptr_t(&entity->GetRagdolPos()) + ptrdiff_t(12)));
}

void AddTick(C_BasePlayer* pTarget, CUserCmd* pCmd) {
	int id = pTarget->GetIndex();

	CTickRecord record = CTickRecord(pTarget, pCmd->tick_count, Resolver::lbyUpdated[id]);

	//if (BackTrack::backtrackRecords[id].size() > 1)
	//	AnimFix(pTarget, record);

	BackTrack::backtrackRecords[id].push_back(record);
}

class VectorHitbox {
public:
	VectorHitbox(Vector vectoR, CSGOHitbox Hitbox) {
		vectoR = vector;
		hitbox = Hitbox;
	}

	Vector vector;
	CSGOHitbox hitbox;
};

std::vector<VectorHitbox> GetAimSpots(C_BasePlayer* pTarget, std::vector<CTickRecord>::iterator record) {
	std::vector<VectorHitbox> pointsToScan;

	if (!record->matrixBuilt)
		return pointsToScan;

	if (!record->pModel)
		return pointsToScan;

	studiohdr_t* hdr = pModelInfo->GetStudioModel(record->pModel);
	if (!hdr)
		return pointsToScan;

	for (int i = 0; i < sizeof(scanHitboxes) / sizeof(scanHitboxes[0]); i++) {
		if (!Settings::Aimbot::hitboxes[i]) // skip not selected hitboxes
			continue;

		mstudiobbox_t* bbox = hdr->pHitbox((int)scanHitboxes[i], 0);
		if (!bbox)
			return pointsToScan;

		float mod = bbox->radius != -1.f ? bbox->radius : 0.f;

		Vector max;
		Vector min;

		Vector in1 = bbox->bbmax + mod;
		Vector in2 = bbox->bbmin - mod;

		Math::VectorTransform(in1, record->matrix[bbox->bone], max);
		Math::VectorTransform(in2, record->matrix[bbox->bone], min);

		auto center = (min + max) * 0.5f;

		pointsToScan.push_back(VectorHitbox(center, scanHitboxes[i]));
	}

	return pointsToScan;
}

// calcs dmg for all records
void FindBest(C_BasePlayer* pTarget, std::vector<CTickRecord>::iterator record) {
	float bestDmg = 0.f;
	Vector bestAimSpot = Vector(0, 0, 0);
	// lets check dmg

	if (record->calcPos != Globals::pLocal->GetEyePosition()) {
		pTarget->InvalidateBoneCache();

		pTarget->GetCollideable()->OBBMins() = record->mins;
		pTarget->GetCollideable()->OBBMaxs() = record->maxs;
		pTarget->setAbsAngle(Vector(0, record->m_angEyeAngles.y, 0));
		pTarget->setAbsOriginal(record->absOrigin);
		*pTarget->GetFlagsPointer() = record->flags;

		if (!record->matrixBuilt) {
			if (!pTarget->SetupBones(record->matrix, 128, 256, record->m_flSimulationTime))
				return;

			record->matrixBuilt = true;
		}

		auto pointsToScan = GetAimSpots(pTarget, record);
		for (int i = 0; i < pointsToScan.size(); i++) {
			float dmg = Autowall::GetDamageVec(pTarget->GetBonePositionBacktrack(int(Bone::BONE_HEAD), record->matrix), pTarget, pointsToScan[i].hitbox);

			if (dmg > bestDmg) {
				bestDmg = dmg;
				bestAimSpot = pTarget->GetBonePositionBacktrack(int(Bone::BONE_HEAD), record->matrix);
			}
		}

		record->calcDmg = bestDmg;
		record->calcBestPos = bestAimSpot;
		record->calcPos = Globals::pLocal->GetEyePosition();
	}
}

void BackTrack::CreateMove(CUserCmd* pCmd) {
	if (!pEngine->IsInGame() || !Settings::Aimbot::backtrack) {
		BackTrack::backtrackRecords->clear();
		return;
	}
	
	//Globals::C_BasePlayer* pLocal = (C_BasePlayer*)pEntityList->GetClientEntity(pEngine->GetLocalPlayer());

	for (int i = 0; i < pEngine->GetMaxClients(); ++i) {
		C_BasePlayer* pTarget = (C_BasePlayer*)pEntityList->GetClientEntity(i);
		//devalid the tickrecord from best so we dont take the old one again, since its prolly outdated anyways
		if (!pTarget
			|| pTarget == Globals::pLocal
			|| !pTarget->GetAlive()
			|| pTarget->GetDormant()
			|| lastSimTime[pTarget->GetIndex()] == pTarget->GetSimulationTime()
			|| pTarget->GetTeam() == Globals::pLocal->GetTeam())
			continue;

		lastSimTime[pTarget->GetIndex()] = pTarget->GetSimulationTime(); // dont wanna record fakelag records/position

		AddTick(pTarget, pCmd);

		bool restart = false;

		for (auto record = BackTrack::backtrackRecords[i].begin(); record != BackTrack::backtrackRecords[i].end(); record++) {
			if (restart) {//if we deleted the oldest last continue, start from the begin()
				record = BackTrack::backtrackRecords[i].begin();
				restart = false;
			}

			if (!IsTickValid(TIME_TO_TICKS(record->m_flSimulationTime + GetLerpTime()))) {
				BackTrack::backtrackRecords[i].erase(record);
				if (!BackTrack::backtrackRecords[i].empty()) {
					restart = true; // flag for restart
					record = BackTrack::backtrackRecords[i].begin();
					continue;
				}
				else
					break;
			}
			//make a function, call it like findbest, call it here instead of the else,
			//so u check each record each tick each entity

			// should have only valid records left

			//FindBest(pTarget, record);
		}
	}
	
}