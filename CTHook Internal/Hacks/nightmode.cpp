#include "nightmode.h"

bool Settings::NightMode::enabled = true;

void NightMode::FrameStageNotify(ClientFrameStage_t pStage) {
	static bool bPerformed = false, bLastSetting;

	//C_BasePlayer* pLocal = (C_BasePlayer*)pEntityList->GetClientEntity(pEngine->GetLocalPlayer());

	if (!Globals::pLocal || !pEngine->IsInGame())
		return;

	static bool once = false;

	if (!once) {
		for (auto i = pMaterial->FirstMaterial(); i != pMaterial->InvalidMaterial(); i = pMaterial->NextMaterial(i)) {
			static IMaterial* mat = pMaterial->GetMaterial(i);

			if (!mat || mat->IsErrorMaterial())
				continue;

			mat->ColorModulate(0.15, 0.15, 0.15);
		}
		once = true;
	}
}