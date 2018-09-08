#include "fovchanger.h"

bool Settings::FOVChanger::enabled = false;
bool Settings::FOVChanger::ignoreScope = true;
float Settings::FOVChanger::value = 100.f;

bool Settings::FOVChanger::viewmodelEnabled = false;
float Settings::FOVChanger::viewmodelValue = 90.f;

void FOVChanger::OverrideView(ViewSetup* pSetup) {
	if (!Settings::FOVChanger::enabled && !Settings::FOVChanger::viewmodelEnabled)
		return;

	C_BasePlayer* pLocal = Globals::pLocal;
	if (!pLocal)
		return;

	if (!pLocal->GetAlive()) {
		if (*pLocal->GetObserverMode() == ObserverMode_t::OBS_MODE_IN_EYE && pLocal->GetObserverTarget())
			pLocal = (C_BasePlayer*)pEntityList->GetClientEntityFromHandle(pLocal->GetObserverTarget());

		if (!pLocal)
			return;
	}

	if (Settings::FOVChanger::enabled && (Settings::FOVChanger::ignoreScope ? !pLocal->IsScoped() : true))
		pSetup->fov = Settings::FOVChanger::value;
}

void FOVChanger::GetViewModelFOV(float& fov) {
	if (!Settings::FOVChanger::enabled && !Settings::FOVChanger::viewmodelEnabled)
		return;

	//C_BasePlayer* pLocal = (C_BasePlayer*)pEntityList->GetClientEntity(pEngine->GetLocalPlayer());
	if (!Globals::pLocal)
		return;

	if (!Globals::pLocal->GetAlive()) {
		if (*Globals::pLocal->GetObserverMode() == ObserverMode_t::OBS_MODE_IN_EYE && Globals::pLocal->GetObserverTarget())
			Globals::pLocal = (C_BasePlayer*)pEntityList->GetClientEntityFromHandle(Globals::pLocal->GetObserverTarget());

		if (!Globals::pLocal)
			return;
	}

	if (Settings::FOVChanger::viewmodelEnabled &&
		(Settings::FOVChanger::ignoreScope ? !Globals::pLocal->IsScoped() : true))
		fov = Settings::FOVChanger::viewmodelValue;
}