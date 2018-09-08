#include "thirdperson.h"

bool Settings::ThirdPerson::enabled = true;
float Settings::ThirdPerson::distance = 100.f;
bool Settings::ThirdPerson::realAngles = true;
bool Settings::ThirdPerson::fakeChams = true;

void ThirdPerson::SceneEnd() {
	//C_BasePlayer* pLocal = (C_BasePlayer*)pEntityList->GetClientEntity(pEngine->GetLocalPlayer());

	if (!pEngine->IsInGame())
		return;
	
	if (!Globals::pLocal || !Globals::pLocal->GetAlive())
		return;

	Globals::eyePos = Globals::pLocal->GetEyePosition();
	Globals::absOrigin = Globals::pLocal->GetAbsOriginal2();

	if (!Settings::ThirdPerson::fakeChams)
		return;

	if (!pInput->ThirdPerson)
		return;

	IMaterial* mat = Chams::mat;
	IMaterial* mat2 = Chams::mat;
	if (mat2) {
		mat2->ColorModulate(1, 1, 1);
		pModelRender->ForcedMaterialOverride(mat2);
		Globals::pLocal->DrawModel(0x1/*STUDIO_RENDER*/, 255);
		pModelRender->ForcedMaterialOverride(nullptr);
	}

	if (mat && (Settings::AntiAim::Yaw::enabled && AntiAim::doingAA)) {
		Vector savedAbs = Globals::pLocal->GetAbsOriginal2();
		if (!FakeLag::lastUnchokedPos.IsZero())
			Globals::pLocal->setAbsOriginal(FakeLag::lastUnchokedPos);
		Vector angle = *Globals::pLocal->GetEyeAngles();
		Globals::pLocal->SetAngle2(Vector(0, AntiAim::lastFake, 0));
		mat->ColorModulate(0.33, 0.33, 0.33);
		mat->AlphaModulate(0.75);
		pModelRender->ForcedMaterialOverride(mat);
		Globals::pLocal->DrawModel(0x1/*STUDIO_RENDER*/, 255);
		pModelRender->ForcedMaterialOverride(nullptr);
		Globals::pLocal->SetAngle2(angle);
		if (!savedAbs.IsZero())
			Globals::pLocal->setAbsOriginal(savedAbs);
	}
}

void ThirdPerson::FrameStageNotify(ClientFrameStage_t pStage) {
	//C_BasePlayer* pLocal = (C_BasePlayer*)pEntityList->GetClientEntity(pEngine->GetLocalPlayer());

	/*static float stepSize = 0.25f;

	if (GetAsyncKeyState(VK_UP) & 0x8000 && Settings::ThirdPerson::distance < 500)
		Settings::ThirdPerson::distance += stepSize;

	if (GetAsyncKeyState(VK_DOWN) & 0x8000 && Settings::ThirdPerson::distance > 5)
		Settings::ThirdPerson::distance -= stepSize;*/

	float currentTime = pGlobalVars->curtime;
	static float lastPressed = 0;

	if (!pEngine->IsInGame()) {
		lastPressed = 0;
		return;
	}

	if (pInputSystem->IsButtonDown(ButtonCode_t::KEY_F) && currentTime - lastPressed > 0.2f) {
		Settings::ThirdPerson::enabled = !Settings::ThirdPerson::enabled;
		lastPressed = currentTime;
	}

	if (pEngine->IsInGame() && Globals::pLocal && pStage == ClientFrameStage_t::FRAME_RENDER_START)
	{
		static Vector vecAngles;
		pEngine->GetViewAngles(vecAngles);
		if (Settings::ThirdPerson::enabled && Globals::pLocal->GetAlive())
		{
			if (!pInput->ThirdPerson)
				pInput->ThirdPerson = true;
			else {
				pInput->CameraVac = Vector(vecAngles.x, vecAngles.y, Settings::ThirdPerson::distance);

				Vector angle = *Globals::pLocal->GetEyeAngles();

				if (Settings::ThirdPerson::realAngles && Settings::AntiAim::Yaw::enabled && AntiAim::doingAA)
					angle.y = AntiAim::lastReal;

				*Globals::pLocal->GetVAngles() = angle;
			}
		}
		else if (pInput->ThirdPerson) {
			pInput->ThirdPerson = false;
			pInput->CameraVac = Vector(vecAngles.x, vecAngles.y, 0);
		}
	}
}
