#include "fakelag.h"

bool Settings::FakeLag::enabled = true;
bool Settings::FakeLag::manualChoke = true;
int Settings::FakeLag::value = 9;
FakeLagType Settings::FakeLag::type = FakeLagType::KMETH;

bool Settings::FakeWalk::enabled = true;
ButtonCode_t Settings::FakeWalk::key = ButtonCode_t::KEY_LSHIFT;

int cmdCounter = 0;
bool FakeLag::lcBroken;
bool FakeLag::sendPacket;
Vector FakeLag::lastUnchokedPos = Vector(0, 0, 0);

bool FakeWalk(CUserCmd* pCmd) {
	if (!Settings::FakeWalk::enabled)
		return false;

	if (!pInputSystem->IsButtonDown(Settings::FakeWalk::key))
		return false;

	static int choked = 0;
	choked = choked > 15 ? 0 : choked + 1;
	SendPacket = choked > 14;
	pCmd->forwardmove = clamp(pCmd->forwardmove, -30.f, 30.f);
	pCmd->sidemove = clamp(pCmd->sidemove, -30.f, 30.f);

	if (SendPacket)
		FakeLag::lastUnchokedPos = Globals::pLocal->GetAbsOriginal2();

	Vector velocity = Globals::unpredictedVelocity;

	if (choked > 5) {
		if (choked < 8 && velocity.Length2D() != 0.f) {
			Vector direction = velocity.Direction();
			float speed = velocity.Length();
			direction.y = pCmd->viewangles.y - direction.y;

			Vector forward;
			Math::AngleVector(direction, forward);

			Vector negated_direcition = forward * -speed;

			pCmd->forwardmove = negated_direcition.x;
			pCmd->sidemove = negated_direcition.y;
		}
		else {
			pCmd->forwardmove = 0;
			pCmd->sidemove = 0;
		}
	}

	return true;
}

void AstroWalk(CUserCmd* pCmd) {
	/*C_BasePlayer* pLocal = (C_BasePlayer*)pEntityList->GetClientEntity(pEngine->GetLocalPlayer());
	
	Vector velocity = pLocal->GetVelocity();
	Vector direction = velocity.Direction();
	float speed = velocity.Length();

	direction.y = pCmd->viewangles.y - direction.y;

	C_BaseCombatWeapon* pActiveWeapon = (C_BaseCombatWeapon*)pEntityList->GetClientEntityFromHandle(pLocal->GetActiveWeapon());
	
	Vector negated_direction = direction * -speed;
	if (velocity.Length() >= (pActiveWeapon->GetCSWpnData()->GetMaxPlayerSpeed() * .25f))
	{
		pCmd->forwardmove = negated_direction.x;
		pCmd->sidemove = negated_direction.y;
	}*/
	//float delta = Globals::pLocal->GetSimulationTime() - TICKS_TO_TIME(pCmd->tick_count);
	//pCmd->tick_count += fabsf(delta);
	pCmd->tick_count = TIME_TO_TICKS(pGlobalVars->frametime);
}

void pWalk(CUserCmd* pCmd) {
	static int ppp = 0;

	int pTicks = TIME_TO_TICKS(2);
		
	if (ppp <= 2) {
		pCmd->tick_count = 0;
		//pCmd->forwardmove = INT_MAX;
		//pCmd->sidemove = INT_MAX;
		//pCmd->upmove = INT_MAX;
		pCmd->command_number = INT_MAX;
	}
	if (ppp > 2)
		ppp = 0;

	ppp++;
}

void FakeLag::CreateMove(CUserCmd* pCmd) {	
	//C_BasePlayer* pLocal = (C_BasePlayer*)pEntityList->GetClientEntity(pEngine->GetLocalPlayer());

	if (!Globals::pLocal || !Globals::pLocal->GetAlive()) {
		SendPacket = true;
		FakeLag::sendPacket = SendPacket;
		return;
	}

	/*if (pInputSystem->IsButtonDown(ButtonCode_t::KEY_C)) {
		//AstroWalk(pCmd);
		
		//static bool gay;
		//gay = !gay;

		//if (gay)
		//	pCmd->tick_count += 128;
		//else
		//	pCmd->tick_count -= 128;
		pWalk(pCmd);
	}*/

	if (FakeWalk(pCmd))
		return;

	if (Globals::pLocal->GetVelocity().Length2D() < 1.f) {
		lastUnchokedPos = Globals::pLocal->GetAbsOriginal2();
		return;
	}

	bool manualChoke = pInputSystem->IsButtonDown(ButtonCode_t::KEY_X);

	if (pCmd->buttons & IN_ATTACK || pCmd->buttons & IN_USE) {
		SendPacket = true;
		FakeLag::sendPacket = SendPacket;
		return;
	}

	long time = Util::GetEpochTime();
	static long lastSent = 0;

	if (SendPacket)
		lastSent = pCmd->tick_count;

	Vector curpos = Globals::pLocal->GetVecOrigin();
	static Vector originrecords[2];

	if (SendPacket) {
		static bool flip = false;
		originrecords[flip ? 0 : 1] = curpos;
		flip = !flip;
	}

	if ((originrecords[0] - originrecords[1]).LengthSqr() > 4096.f)
		FakeLag::lcBroken = true;
	else
		FakeLag::lcBroken = false;

	// this doesnt work as it should
	// but the idea is that server doesn't interpolate you after 0.5 seconds not sending any packet
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/client/c_baseentity.cpp#L3199
	// GetTimeSinceLastReceived() crashes
	/*static bool interp = true;

	if (time - lastSent >= 500)
		interp = false;
	else
		interp = true;*/

	if (Settings::FakeLag::manualChoke && pInputSystem->IsButtonDown(ButtonCode_t::KEY_X)) {
		/*bool spacket = false;

		if (pCmd->sidemove == 0 && pCmd->forwardmove == 0 && pLocal->GetVelocity().Length2D() == 0)
			spacket = true;

		static int cancer = 0;
		static int cancerCount = 0;

		cancerCount++;

		if (pCmd->sidemove > 39)
			pCmd->sidemove = 39;
		else if (pCmd->sidemove < -39)
			pCmd->sidemove = -39;

		if (pCmd->forwardmove > 39)
			pCmd->forwardmove = 39;
		else if (pCmd->forwardmove < -39)
			pCmd->forwardmove = -39;

		if (cancerCount > 3) {
			pCmd->sidemove = 0;
			pCmd->forwardmove = 0;
		}

		if (cancer == 2)
			pCmd->tick_count = INT_MAX;
		cancer++;

		if (cancerCount > 9)
			spacket = true;
		if (cancerCount > 10)
			cancerCount = 0;

		SendPacket = spacket;*/

		/*bool spacket = false;

		if (pInputSystem->IsButtonDown(ButtonCode_t::KEY_C) || time - lastSent > 5000)
			spacket = true;*/

		SendPacket = false;
	}
	
	if (Settings::FakeLag::enabled && !manualChoke && (!pInputSystem->IsButtonDown(Settings::FakeWalk::key) && Settings::FakeWalk::enabled)) {
		switch (Settings::FakeLag::type) {
			case FakeLagType::REACTIVE: {
				static int chokedTicks = 0;
				static int maxChokedTicks = 0;
				float speed = Globals::pLocal->GetVelocity().Length2D();

				if (speed > 350)
					maxChokedTicks = 8;
				else if (speed > 500)
					maxChokedTicks = 7;
				else if (speed > 750)
					maxChokedTicks = 6;
				else
					maxChokedTicks = 10;

				if (chokedTicks > maxChokedTicks) {
					SendPacket = true;
					chokedTicks = 0;
				}
				else {
					SendPacket = false;
					chokedTicks++;
				}
			}
			break;
			case FakeLagType::LUNICO: {
				static int chokeAmount;

				if (Globals::pLocal->GetVelocity().Length2D() > 0.1f) {
					chokeAmount = static_cast<int>((64.0f / pGlobalVars->interval_per_tick) /
						Globals::pLocal->GetVelocity().Length2D());
				}
				else {
					chokeAmount = 16;
				}

				if (chokeAmount > 16) {
					chokeAmount = 16;
				}

				cmdCounter++;

				if (cmdCounter >= chokeAmount) {
					SendPacket = true;
					cmdCounter = 0;
				}
				else {
					SendPacket = false;
				}
			}
			break;
			case FakeLagType::KMETH: {
				if (cmdCounter >= Settings::FakeLag::value) {
					SendPacket = true;
					cmdCounter = 0;
				}
				else {
					SendPacket = false;
				}
				cmdCounter++;
			}
			break;
		}
	}

	FakeLag::sendPacket = SendPacket;

	if (SendPacket) {
		lastSent = time;
		FakeLag::lastUnchokedPos = Globals::pLocal->GetAbsOriginal2();
	}
}
