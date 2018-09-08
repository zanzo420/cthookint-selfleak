#include "resolver.h"

bool Settings::Resolver::enabled = true;
bool Settings::Resolver::override = true;
ButtonCode_t Settings::Resolver::overrideKey = KEY_C;
int Settings::Resolver::baimAfterXShots = 5;

bool Resolver::lbyUpdated[64];
bool Resolver::lbyBacktrackable[64];
float Resolver::nextUpdate[64];
float Resolver::updateTime[64];
int Resolver::shotsMissed[64];
bool Resolver::dontShoot[64];
ResolverType Resolver::resolveType[64];
float Resolver::simTimeDelta[64];
bool Resolver::fakeAngles[64];
bool Resolver::shooting[64];
float Resolver::paramDiff[64];

CTickRecord curTick[64];
CTickRecord prevTick[64];
AnimationLayer curLayer[64];
AnimationLayer prevLayer[64];

std::deque<CTickRecord> simRecords[64];

CTickRecord lbyTick[64];
CTickRecord shootingTick[64];

bool firstUpdate[64];
bool sawUpdate[64];
float lbyFlickTime[64];
float lbySave[64];
float lbySave2[64];
float simTimeSave[64];
float prevSimTimeSave[64];
float stoppedMovingTime[64];
float lastAnimFlickTime[64];
BreakingState prevBreakingState[64];

Vector firstMovingDir[64];
int prevLayerAct[64];

float avgSpinDelta[64];
int ticksAfterLbyUpdate[64];

bool sawUpdateSinceMoving[64];
bool lbyAfterMovingChanged[64];
float lbyAfterMoving[64];

float lastMove[64];
bool lastMoveChanged[64];

bool hitAngle[64];
float hitAngleDelta[64];
float tryAngle[64];

int Resolver::wasBreakingOver120[64];

//--------------------------------------------------------
//int impactNum;
std::deque<Vector> impacts;

bool playerHurt[64];
bool bulletImpact[64];
//--------------------------------------------------------



//plist
int Resolver::yawCorrection[64];
float Resolver::yawCorrectionAdd[64];
int Resolver::pitchCorrection[64];
float Resolver::pitchCorrectionAdd[64];



//override
int overridePlayer = -1;

int Resolver::GetSeqActivity(C_BaseEntity* player, studiohdr_t* hdr, int sequence)
{
	if (!hdr)
		return -1;

	// c_csplayer vfunc 242, follow calls to find the function.
	static auto GetSequenceActivity = reinterpret_cast<int(__fastcall*)(void*, studiohdr_t*, int)>((DWORD)(FindPattern("client_panorama.dll", "\x55\x8B\xEC\x83\x7D\x08\xFF\x56\x8B\xF1\x74", "xxxxxxxxxxx")));

	return GetSequenceActivity(player, hdr, sequence);
}

bool LBYBacktrack(C_BasePlayer* pTarget) {
	if (!pTarget)
		return false;

	int i = pTarget->GetIndex();

	if (lbyTick[i].m_vecOrigin.IsZero())
		return false;

	if (!BackTrack::IsTickValid(TIME_TO_TICKS(lbyTick[i].m_flSimulationTime + BackTrack::GetLerpTime())))
		return false;

	if (lbyTick[i].m_vecOrigin.DistTo(pTarget->GetVecOrigin()) < 3.f)
		return true;
	else
		return false;
}

bool ShootingBacktrack(C_BasePlayer* pTarget) {
	if (!pTarget)
		return false;

	int i = pTarget->GetIndex();

	if (shootingTick[i].m_vecOrigin.IsZero())
		return false;

	if (!BackTrack::IsTickValid(TIME_TO_TICKS(shootingTick[i].m_flSimulationTime + BackTrack::GetLerpTime())))
		return false;

	if (shootingTick[i].m_vecOrigin.DistTo(pTarget->GetVecOrigin()) < 3.f)
		return true;
	else
		return false;
}

void Resolver::FrameStageNotify(ClientFrameStage_t pStage) {
	if (!Settings::Resolver::enabled)
		return;

	//C_BasePlayer* pLocal = (C_BasePlayer*)pEntityList->GetClientEntity(pEngine->GetLocalPlayer());
	//Override();

	if (pStage != ClientFrameStage_t::FRAME_NET_UPDATE_POSTDATAUPDATE_START)
		return;

	for (int i = 0; i <= pEngine->GetMaxClients(); i++) {
		C_BasePlayer* pTarget = (C_BasePlayer*)pEntityList->GetClientEntity(i);

		if (!pTarget
			|| !pTarget->GetAlive()
			|| pTarget == Globals::pLocal)
			continue;

		if (!pTarget->GetDormant()) { // ;)
			sawUpdate[i] = false;
			firstUpdate[i] = false;
			lbyAfterMovingChanged[i] = false;
			continue;
		}

		curTick[i] = CTickRecord(pTarget, pGlobalVars->tickcount);

		//AimBot::forceBaim[i] = Settings::Resolver::baimAfterXShots > shotsMissed[i];

		float lby = *pTarget->GetLowerBodyYawTarget();
		float simTime = pTarget->GetSimulationTime();
		float velocity = pTarget->GetVelocity().Length2D();
		float angle = lby;
		bool onGround = pTarget->GetFlags() & FL_ONGROUND;
		bool ducking = pTarget->GetFlags() & FL_DUCKING;

		lbyUpdated[i] = false;

		if (velocity > 0.1f) {
			//lbyUpdated[i] = true;
			sawUpdateSinceMoving[i] = false;
			stoppedMovingTime[i] = simTime;
			lbyAfterMovingChanged[i] = false;
			if (velocity > 1.f && velocity < 5.f && onGround) {
				lastMoveChanged[i] = true;
				lastMove[i] = lby;
			}
		}

		/*if (curTick[i].m_flLowerBodyYawTarget != prevTick[i].m_flLowerBodyYawTarget && prevTick[i].m_flLowerBodyYawTarget != 0.f && curTick[i].m_flLowerBodyYawTarget != 0.f) {
			if (!firstUpdate[i])
				firstUpdate[i] = true;
			else {
				lbyUpdated[i] = true;
				sawUpdate[i] = true;
				nextUpdate[i] = prevTick[i].m_flSimulationTime + pGlobalVars->interval_per_tick;
				lbyTick[i] = prevTick[i];
				sawUpdateSinceMoving[i] = true;
			}
		}

		if (prevTick[i].m_flSimulationTime - nextUpdate[i] > 1.1 && sawUpdate[i]) {
			lbyUpdated[i] = true;
			nextUpdate[i] = prevTick[i].m_flSimulationTime + pGlobalVars->interval_per_tick;
			lbyTick[i] = CTickRecord(pTarget, pGlobalVars->tickcount);
		}*/

		/*if (lby != prevTick[i].m_flLowerBodyYawTarget && prevTick[i].m_flLowerBodyYawTarget != 0.f) {
			if (!firstUpdate[i])
				firstUpdate[i] = true;
			else {
				lbyUpdated[i] = true;
				sawUpdate[i] = true;
				nextUpdate[i] = prevTick[i].m_flSimulationTime + 1.1 + pGlobalVars->interval_per_tick;
				sawUpdateSinceMoving[i] = true;
				lbyTick[i] = CTickRecord(pTarget, pGlobalVars->tickcount);
			}
		}

		if (prevTick[i].m_flSimulationTime > nextUpdate[i] && sawUpdate[i]) {
			lbyUpdated[i] = true;
			nextUpdate[i] = prevTick[i].m_flSimulationTime + 1.1 + pGlobalVars->interval_per_tick;
			lbyTick[i] = prevTick[i];
		}*/

		if (simTime != simTimeSave[i]) {
			simTimeDelta[i] = simTime - simTimeSave[i];
			simTimeSave[i] = simTime;
		}

		if (simRecords[i].empty())
			simRecords[i].push_front(CTickRecord(pTarget, pGlobalVars->tickcount));

		shooting[i] = false;

		Vector angToLocal = Math::CalcAngle(pTarget->GetEyePosition(), Globals::pLocal->GetEyePosition());
		float pitch = pTarget->GetEyeAngles()->x;

		if (simTime != simRecords[i].front().m_flSimulationTime)
		{
			if (lby != simRecords[i].front().m_flLowerBodyYawTarget) //checking if cur lby is different to previous simrecord
			{
				if (!firstUpdate[i]) // skip adding the first "change" after entity has been dormant/non-existant
					firstUpdate[i] = true;
				else
				{
					sawUpdateSinceMoving[i] = true;
					//lbyUpdated[i] = true;
					lbyTick[i] = CTickRecord(pTarget, pGlobalVars->tickcount);
					nextUpdate[i] = simRecords[i].front().m_flSimulationTime + pGlobalVars->interval_per_tick; // previous simtime + 1 tick should give us the tick where they actually flicked. (Only first tick of a choke cycle gets used in anims)
					sawUpdate[i] = true; // we saw the update, we are good to add "predicted" updates, (didn't break when dormant)
				}
			}
			simRecords[i].push_front(CTickRecord(pTarget, pGlobalVars->tickcount));

			// now comes predicting next lby update times, keeping in mind how an lby breaker works..
			if (simRecords[i].at(1).m_flSimulationTime - nextUpdate[i] > 1.1 && sawUpdate[i] && onGround) // previous sim update lby time delta over 1.1, meaning that next choked will be the break tick and -> the current sim is the next simupdate after that, we can shoot lby
			{
				//lbyUpdated[i] = true;
				nextUpdate[i] = simRecords[i].at(1).m_flSimulationTime + pGlobalVars->interval_per_tick; // save off the time where they actually did the break
				lbyTick[i] = simRecords[i].front(); // save the actual current record, as this is the one we can shoot
			}

			// shooting detection lol
			if (pitch != -89.f && pitch != 0.f && pitch != 89.f) { // diff pitch
				if (fabsf(pitch - angToLocal.x) < 15.f && pitch != simRecords[i].at(1).m_angEyeAngles.x) { // if delta is a lot diff prob not shooting at us / at all
					shooting[i] = true;
					shootingTick[i] = CTickRecord(pTarget, pGlobalVars->tickcount);
				}
			}
		}

		fakeAngles[i] = (TIME_TO_TICKS(simTimeDelta[i] - pGlobalVars->interval_per_tick) >= 1);

		//AimBot::forceTick[i] = false;

		curLayer[i] = pTarget->GetAnimationOverlay(3);

		if (prevTick[i].m_vecVelocity.Length2D() == 0.0f && curTick[i].m_vecVelocity.Length2D() != 0.0f)
			Math::VectorAngles(curTick[i].m_vecVelocity, firstMovingDir[i]);

		const int curLayerAct = GetSeqActivity(pTarget, pModelInfo->GetStudioModel(pTarget->GetModel()), curLayer[i].m_nSequence);
		const int prevLayerAct = GetSeqActivity(pTarget, pModelInfo->GetStudioModel(pTarget->GetModel()), prevLayer[i].m_nSequence);

		BreakingState breakingState = NOTBREAKING;

		if (curLayerAct == ACT_CSGO_IDLE_TURN_BALANCEADJUST) // the current layer must be triggering 979
		{
			if (prevLayerAct == ACT_CSGO_IDLE_TURN_BALANCEADJUST) // the previous layer must be trigerring 979
			{
				// we can tell now that he is surely breaking lby in some sort
				breakingState = BREAKING;
				prevBreakingState[i] = BREAKING;

				if ((prevLayer[i].m_flCycle != curLayer[i].m_flCycle) || curLayer[i].m_flWeight == 1.f)
				{
					float flickTime = simTime - curLayer[i].m_flCycle;
					
					/*if (curLayer[i].m_flCycle < 0.01f && prevLayer[i].m_flCycle > 0.01f && BackTrack::IsTickValid(TIME_TO_TICKS(flickTime))) // lby flicked here
					{
						float flickTime = simTime - curLayer[i].m_flCycle;

						if (flickTime - lastAnimFlickTime[i] > 1.f) // time check for retards breaking anims
							lbyTick[i] = CTickRecord(pTarget, pGlobalVars->tickcount, flickTime);

						lastAnimFlickTime[i] = flickTime;
					}*/

					breakingState = BREAKINGOVER120;
					prevBreakingState[i] = BREAKINGOVER120;
				}
				else if (curLayer[i].m_flWeight == 0.f && (prevLayer[i].m_flCycle > 0.92f && curLayer[i].m_flCycle > 0.92f)) // breaking lby with delta < 120; can fakewalk here aswell
				{
					breakingState = BREAKINGUNDER120;
					prevBreakingState[i] = BREAKINGUNDER120;
				}
			}
		}

		bool fakewalk = false;

		if (!lbyUpdated[i] && velocity > 0.1f && onGround) {
			if (velocity > 5.f && velocity < 50.f && !ducking)
				fakewalk = true;
			else
				lbyUpdated[i] = true;
		}

		if ((LBYBacktrack(pTarget) || lbyUpdated[i]))
			AimBot::forceBaim[i] = false;
		else if (shotsMissed[i] <= Settings::Resolver::baimAfterXShots)
			AimBot::forceBaim[i] = false;
		else
			AimBot::forceBaim[i] = true;

		float lastMoveDelta = Math::YawDelta(lby, lastMove[i]);
		float lbyAfterMovingDelta = Math::YawDelta(lby, lbyAfterMoving[i]);

		if (lbyUpdated[i]) {
			angle = lby;
			resolveType[i] = LBY;
		}
		else if (LBYBacktrack(pTarget)) {
			angle = lbyTick[i].m_flLowerBodyYawTarget;
			pTarget->GetEyeAngles()->x = lbyTick[i].m_angEyeAngles.x; // for the retards which change pitch on lby update
			AimBot::forceTick[i] = TIME_TO_TICKS(lbyTick[i].m_flSimulationTime - BackTrack::GetLerpTime());
			resolveType[i] = LBYBT;
		}
		else if (ShootingBacktrack(pTarget)) {
			angle = angToLocal.y;
			pTarget->GetEyeAngles()->x = angToLocal.x; 
			AimBot::forceTick[i] = TIME_TO_TICKS(shootingTick[i].m_flSimulationTime - BackTrack::GetLerpTime());
			resolveType[i] = SHOOTING;
		}
		else if (simTime - nextUpdate[i] < 0.22 && !sawUpdateSinceMoving[i] && velocity < 0.1f && onGround) {
			lbyAfterMoving[i] = lby;
			lbyAfterMovingChanged[i] = true;
			angle = lby;
			resolveType[i] = LBY;
		}
		else if (fakewalk) {
			if (shotsMissed[i] > 1)
				AimBot::forceBaim[i] = true;
			else
				AimBot::forceBaim[i] = false;

			switch (shotsMissed[i] % 3) {
				case 0: angle = firstMovingDir[i].y + 180.f; break;
				case 1: angle = lby + 140.f; break;
				case 2: angle = lby - 140.f; break;
			}
			resolveType[i] = FAKEWALK;
		}
		else if (breakingState == BREAKINGOVER120 || prevBreakingState[i] == BREAKINGOVER120) {
			if (lbyAfterMovingChanged[i] && lbyAfterMovingDelta > 120.f && shotsMissed[i] <= 1) {
				angle = lbyAfterMoving[i];
				resolveType[i] = LBYAFTERMOVING;
			}
			else if (lastMoveChanged[i] && lastMoveDelta > 120.f && shotsMissed[i] <= 2) {
				angle = lastMove[i];
				resolveType[i] = LASTMOVE;
			}
			else if (shotsMissed[i] <= 4) {
				std::vector<float> blacklistedangs;
				blacklistedangs.push_back(lby + 35.f);
				blacklistedangs.push_back(lby - 35.f);
				blacklistedangs.push_back(lby + 85.f);
				blacklistedangs.push_back(lby - 85.f);

				angle = GetBestEdgeAngle(pTarget, blacklistedangs); // not hard to make
				resolveType[i] = LBYINVERSE;
			}
			else {
				switch (shotsMissed[i] % 3) {
					case 0: angle = lby + 180.f; break;
					case 1: angle = lby + 140.f; break;
					case 2: angle = lby - 140.f; break;
				}
				resolveType[i] = BRUTE;
			}
		}
		else if (breakingState == BREAKINGUNDER120 || prevBreakingState[i] == BREAKINGUNDER120) {
			if (lbyAfterMovingChanged[i] && lbyAfterMovingDelta < 120.f && lbyAfterMovingDelta > 35.f && shotsMissed[i] <= 1) {
				angle = lbyAfterMoving[i];
				resolveType[i] = LBYAFTERMOVING;
			}
			else if (lastMoveChanged[i] && lastMoveDelta < 120.f && lastMoveDelta > 35.f && shotsMissed[i] <= 2) {
				angle = lastMove[i];
				resolveType[i] = LASTMOVE;
			}
			else if (shotsMissed[i] <= 4) {
				/*switch (shotsMissed[i] % 2) {
					case 0: angle = lby + 110.f; break;
					case 1: angle = lby - 110.f; break;
				}*/
				std::vector<float> blacklistedangs;
				blacklistedangs.push_back(lby + 65.f);
				blacklistedangs.push_back(lby - 65.f);

				blacklistedangs.push_back(lby + 155.f);
				blacklistedangs.push_back(lby - 155.f);

				angle = GetBestEdgeAngle(pTarget, blacklistedangs);
				resolveType[i] = FREESTANDINGAUTO;
			}
			else {
				switch (shotsMissed[i] % 2) {
				case 0: angle = lby + 110.f; break;
				case 1: angle = lby - 110.f; break;
				}
			}
		}
		else if (breakingState == BREAKING || prevBreakingState[i] == BREAKING) {
			if (lbyAfterMovingChanged[i] && lbyAfterMovingDelta > 35.f && shotsMissed[i] <= 1) {
				angle = lbyAfterMoving[i];
				resolveType[i] = LBYAFTERMOVING;
			}
			else if (lastMoveChanged[i] && lastMoveDelta > 35.f && shotsMissed[i] <= 2) {
				angle = lastMove[i];
				resolveType[i] = LASTMOVE;
			}
			else if (shotsMissed[i] <= 4) {
				std::vector<float> blacklistedangs;
				blacklistedangs.push_back(lby + 35.f);
				blacklistedangs.push_back(lby - 35.f);

				angle = GetBestEdgeAngle(pTarget, blacklistedangs);
				resolveType[i] = FREESTANDINGBRUTE;
			}
			else {
				switch (shotsMissed[i] % 4) {
					case 0: angle = lby + 110.f; break;
					case 1: angle = lby - 110.f; break;
					case 2: angle = lby - 165.f; break;
					case 3: angle = lby + 165.f; break;
				}
			}
		}
		else {
			if (lastMoveChanged[i] && shotsMissed[i] <= 1) {
				angle = lastMove[i];
				resolveType[i] = LASTMOVE;
			}
			else {
				switch (shotsMissed[i] % 6) {
					case 0: angle = lby + 70.f; break;
					case 1: angle = lby - 70.f; break;
					case 2: angle = lby - 135.f; break;
					case 3: angle = lby + 135.f; break;
					case 4: angle = lby - 180.f; break;
					case 5: angle = lby; break;
				}
				resolveType[i] = BRUTE;
			}
		}

		if (fakeAngles[i])
			pTarget->GetEyeAngles()->y = Math::ClampYaw(angle);
		else
			resolveType[i] = LEGIT;

		prevTick[i] = curTick[i];
		prevLayer[i] = curLayer[i];
		//prevLayerAct[i] = curLayerAct;

		if (simRecords[i].size() > 6)
			simRecords[i].pop_back();
	}
}

void Resolver::FireGameEvent(IGameEvent* pEvent) {
	if (!pEngine->IsInGame())
		return;

	if (strcmp(pEvent->GetName(), "player_hurt") == 0) {
		int userid = pEvent->GetInt("userid");
		int attacker = pEvent->GetInt("attacker");

		// self damage check
		if (pEngine->GetPlayerForUserID(userid) == pEngine->GetLocalPlayer())
			return;

		if (pEngine->GetPlayerForUserID(attacker) != pEngine->GetLocalPlayer())
			return;

		HitGroups hitgroup = (HitGroups)pEvent->GetInt("hitgroup");

		if (hitgroup == HitGroups::HITGROUP_GENERIC || hitgroup == HitGroups::HITGROUP_GEAR)
			return;

		Resolver::shotsMissed[userid]--;
	}

	if (strcmp(pEvent->GetName(), "weapon_fire") == 0) {
		int userid = pEvent->GetInt("userid");

		if (pEngine->GetPlayerForUserID(userid) != pEngine->GetLocalPlayer())
			return;

		std::string weaponFiredStr = pEvent->GetString("weapon");
		const char* weaponFired = weaponFiredStr.c_str();

		if (strcmp(weaponFired, "weapon_knife") == 0
			|| strcmp(weaponFired, "weapon_taser") == 0
			|| strcmp(weaponFired, "weapon_hegrenade") == 0
			|| strcmp(weaponFired, "weapon_smokegrenade") == 0
			|| strcmp(weaponFired, "weapon_molotov") == 0
			|| strcmp(weaponFired, "weapon_decoy") == 0
			|| strcmp(weaponFired, "weapon_flashbang") == 0
			|| strcmp(weaponFired, "weapon_incgrenade") == 0
			|| strcmp(weaponFired, "weapon_tagrenade") == 0
			|| strcmp(weaponFired, "weapon_healthshot") == 0)
			return;

		Resolver::shotsMissed[AimBot::target]++;
	}

	// reset everything
	if (strstr(pEvent->GetName(), "round_start")) {
		for (int i = 0; i < 64; i++) {
			sawUpdate[i] = false;
			Resolver::shotsMissed[i] = 0;
			firstUpdate[i] = false;
			wasBreakingOver120[i] = 0;
			lastMoveChanged[i] = false;
			lastAnimFlickTime[i] = 0;
			lbyAfterMovingChanged[i] = false;
		}
	}
}
