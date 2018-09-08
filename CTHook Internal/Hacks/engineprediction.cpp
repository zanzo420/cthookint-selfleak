#include "engineprediction.h"

typedef void(__thiscall* SetHost_t)(void*, void*);
float m_flOldCurtime;
float m_flOldFrametime;
MoveData data;
void Prediction::Start(CUserCmd* pCmd)
{
	//if (!Settings::Misc::EnginePrediction::Enabled)
	//	return;

	//C_BasePlayer * Local = (C_BasePlayer*)pEntityList->GetClientEntity(pEngine->GetLocalPlayer());
	if (!Globals::pLocal)
		return;

	*pPredSeed = pCmd->random_seed;

	Globals::unpredictedVelocity = Globals::pLocal->GetVelocity();

	m_flOldCurtime = pGlobalVars->frametime;
	Globals::oldCurTime = m_flOldCurtime;
	m_flOldFrametime = pGlobalVars->curtime;

	pGlobalVars->frametime = pGlobalVars->interval_per_tick;
	pGlobalVars->curtime = Globals::pLocal->GetTickBase() * pGlobalVars->interval_per_tick;

	pGameMovement->StartTrackPredictionErrors(Globals::pLocal);

	memset(&data, 0, sizeof(MoveData));

	pMoveHelper->SetHost(Globals::pLocal);
	pPrediction->SetupMove(Globals::pLocal, pCmd, pMoveHelper, &data);
	pGameMovement->ProcessMovement(Globals::pLocal, &data);
	pPrediction->FinishMove(Globals::pLocal, pCmd, &data);
}


void Prediction::End()
{
	//if (!Settings::Misc::EnginePrediction::Enabled)
	//	return;

	//C_BasePlayer *pLocal = (C_BasePlayer*)pEntityList->GetClientEntity(pEngine->GetLocalPlayer());
	if (!Globals::pLocal)
		return;

	pGameMovement->FinishTrackPredictionErrors(Globals::pLocal);
	pMoveHelper->SetHost(nullptr);

	*pPredSeed = -1;

	pGlobalVars->curtime = m_flOldCurtime;
	pGlobalVars->frametime = m_flOldFrametime;
}