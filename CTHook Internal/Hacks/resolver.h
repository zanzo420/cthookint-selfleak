#pragma once 

#include "../settings.h"
#include "../SDK/SDK.h"
#include "../interfaces.h"
#include "../math.h"
#include "aimbot.h"
#include "backtrack.h"
#include "eventlogger.h"

/*enum AAType : int {
	CANTDETECT,
	FAKESPIN,
	LBY,
	STATIC,
	JITTER2VALUES,
};*/

enum ResolverType : int {
	LEGIT,
	SHOOTING,
	LBY,
	SPIN,
	LBYDELTA,
	LBYINVERSE,
	LBYBT,
	BRUTE,
	LASTMOVE,
	LBYAFTERMOVING,
	LASTMOVEFW,
	FAKEWALK,
	FREESTANDINGR,
	FREESTANDINGL,
	FREESTANDINGB,
	FREESTANDINGBRUTE,
	FREESTANDINGAUTO,
	LASTHIT,
	PLIST,
	OVERRIDE,
};

enum BreakingState : int {
	NOTBREAKING,
	BREAKING,
	BREAKINGOVER120,
	BREAKINGUNDER120
};

const inline float_t LBYDelta(const CTickRecord &v)
{
	return v.m_angEyeAngles.y - v.m_flLowerBodyYawTarget;
}

const inline float maximum(float a, float b, float c)
{
	float max = (a < b) ? b : a;
	return ((max < c) ? c : max);
}

namespace Resolver {
	void FrameStageNotify(ClientFrameStage_t pStage);
	void FireGameEvent(IGameEvent* pEvent);
	int GetSeqActivity(C_BaseEntity* player, studiohdr_t* hdr, int sequence);

	extern bool lbyUpdated[64];
	extern bool lbyBacktrackable[64];
	extern float nextUpdate[64];
	extern int shotsMissed[64];
	extern bool dontShoot[64];
	extern ResolverType resolveType[64];
	extern float simTimeDelta[64];
	extern bool fakeAngles[64];
	extern bool shooting[64];
	extern float updateTime[64];
	extern int wasBreakingOver120[64];
	extern float paramDiff[64];


	//pList
	extern int yawCorrection[64];
	extern float yawCorrectionAdd[64];
	extern int pitchCorrection[64];
	extern float pitchCorrectionAdd[64];
}
