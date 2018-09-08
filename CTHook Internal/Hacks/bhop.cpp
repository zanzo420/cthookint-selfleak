#include "bhop.h"

bool Settings::BHop::enabled = true;
bool Settings::BHop::autoStrafe = true;

void AutoStrafe(CUserCmd* pCmd) {
	if (!Settings::BHop::autoStrafe)
		return;

	//C_BasePlayer* pLocal = (C_BasePlayer*)pEntityList->GetClientEntity(pEngine->GetLocalPlayer());

	static bool leftRight;
	bool inMove = pCmd->buttons & IN_FORWARD || pCmd->buttons & IN_BACK || pCmd->buttons & IN_MOVELEFT ||
		pCmd->buttons & IN_MOVERIGHT;

	if (pCmd->buttons & IN_FORWARD && Globals::pLocal->GetVelocity().Length() <= 50.0f)
		pCmd->forwardmove = 450.0f;

	float yaw_change = 0.0f;
	if (Globals::pLocal->GetVelocity().Length() > 50.f)
		yaw_change = 30.0f * fabsf(30.0f / Globals::pLocal->GetVelocity().Length());

	C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*)pEntityList->GetClientEntityFromHandle(
		Globals::pLocal->GetActiveWeapon());
	if (activeWeapon && activeWeapon->GetAmmo() > 0 && pCmd->buttons & IN_ATTACK)
		yaw_change = 0.0f;

	Vector viewAngles;
	pEngine->GetViewAngles(viewAngles);

	if (!(Globals::pLocal->GetFlags() & FL_ONGROUND) && !inMove) {
		if (leftRight || pCmd->mousedx > 1) {
			viewAngles.y += yaw_change;
			pCmd->sidemove = 450.0f;
		}
		else if (!leftRight || pCmd->mousedx < 1) {
			viewAngles.y -= yaw_change;
			pCmd->sidemove = -450.0f;
		}

		leftRight = !leftRight;
	}

	Math::NormalizeAngles(viewAngles);
	Math::ClampAngles(viewAngles);

	Math::CorrectMovement(viewAngles, pCmd, pCmd->forwardmove, pCmd->sidemove);
}

void BHop::CreateMove(CUserCmd* pCmd) {
	AutoStrafe(pCmd);
	
	if (!Settings::BHop::enabled)
		return;

	static bool bLastJumped = false;
	static bool bShouldFake = false;

	//C_BasePlayer* pLocal = (C_BasePlayer*)pEntityList->GetClientEntity(pEngine->GetLocalPlayer());

	if (!Globals::pLocal)
		return;

	// Bhop on ladder or noclip op?
	if (Globals::pLocal->GetMoveType() == MOVETYPE_LADDER || Globals::pLocal->GetMoveType() == MOVETYPE_NOCLIP)
		return;

	if (!bLastJumped && bShouldFake)
	{
		bShouldFake = false;
		pCmd->buttons |= IN_JUMP;
	}
	else if (pCmd->buttons & IN_JUMP)
	{
		if (Globals::pLocal->GetFlags() & FL_ONGROUND)
		{
			bLastJumped = true;
			bShouldFake = true;
		}
		else
		{
			pCmd->buttons &= ~IN_JUMP;
			bLastJumped = false;
		}
	}
	else
	{
		bLastJumped = false;
		bShouldFake = false;
	}
}