#pragma once

#include "../settings.h"
#include "../SDK/SDK.h"
#include "../interfaces.h"
#include "resolver.h"
#include <deque>

#define TIME_TO_TICKS(dt) ((int)( 0.5f + (float)(dt) / pGlobalVars->interval_per_tick))
#define TICKS_TO_TIME(t) (pGlobalVars->interval_per_tick * (t) )
	
#define FLOW_OUTGOING 0		
#define FLOW_INCOMING 1
#define MAX_FLOWS 2	// in & out

namespace BackTrack
{
	void FrameStageNotify(ClientFrameStage_t pStage);

	void OverwriteTick(C_BasePlayer* pTarget, float correctTime, int priority, bool lbyFlick = false);
	bool RageBacktrack(C_BasePlayer* pTarget, CUserCmd* pCmd, Vector &aimSpot, float &damage);
	void CreateMove(CUserCmd* pCmd);
	bool IsTickValid(int tick);
	float GetLerpTime();

	const int MAX_QUEUE_SIZE = 12;

	extern std::vector<CTickRecord> backtrackRecords[64];

	extern Vector endposition[64][100];
	extern bool canBacktrackLby[64];
	//extern std::deque<CTickRecord> backtrackRecords[64];
}