#include "chams.h"

IMaterial* Chams::mat;
IMaterial* Chams::matZ;
IMaterial* Chams::matZF;
IMaterial* Chams::fakeChams;

bool Settings::ESP::chams = true;

void DrawPlayer(IMatRenderContext* matctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4* pCustomBoneToWorld) {
	if (!Settings::ESP::chams)
		return;

	//C_BasePlayer* pLocal = (C_BasePlayer*)pEntityList->GetClientEntity(pEngine->GetLocalPlayer());
	if (!Globals::pLocal)
		return;

	C_BasePlayer* pEntity = (C_BasePlayer*)pEntityList->GetClientEntity(pInfo.entity_index);

	if (!pEntity || pEntity->GetDormant() || !pEntity->GetAlive())
		return;

	if (pEntity == Globals::pLocal)
		return;

	if (pEntity->GetTeam() == Globals::pLocal->GetTeam())
		return;

	Chams::mat->AlphaModulate(1.f);
	Chams::matZ->AlphaModulate(1.f);
	Chams::matZF->AlphaModulate(1.f);
	Color visColor = Color(167, 217, 7);
	Color color = Color(17, 141, 168); //77, 137, 197

	Chams::mat->ColorModulate(visColor.r / 255.f, visColor.g / 255.f, visColor.b / 255.f);
	Chams::matZ->ColorModulate(color.r / 255.f, color.g / 255.f, color.b / 255.f);
	Chams::matZF->ColorModulate(color.r / 255.f, color.g / 255.f, color.b / 255.f);

	if (pEntity->GetImmune()) {
		Chams::mat->AlphaModulate(0.75f);
		Chams::matZ->AlphaModulate(0.75f);
		Chams::matZF->AlphaModulate(0.75f);
	}

	pModelRender->ForcedMaterialOverride(Chams::matZ);
	ModelRenderHook->GetOriginalFunction<DrawModelExecuteFn>(21)(pModelRender, matctx, state, pInfo, pCustomBoneToWorld);

	pModelRender->ForcedMaterialOverride(Chams::mat);
	ModelRenderHook->GetOriginalFunction<DrawModelExecuteFn>(21)(pModelRender, matctx, state, pInfo, pCustomBoneToWorld);
}

void Chams::DrawModelExecute(IMatRenderContext* matctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4* pCustomBoneToWorld) {
	if (!pEngine->IsInGame())
		return;

	if (!Settings::ESP::enabled)
		return;

	if (!pInfo.pModel)
		return;

	static bool materialsCreated = false;
	if (!materialsCreated) {
		mat = Util::CreateMaterial("VertexLitGeneric", "VGUI/white_additive", false, true, true, true, true);
		matZ = Util::CreateMaterial("VertexLitGeneric", "VGUI/white_additive", true, true, true, true, true);
		matZF = Util::CreateMaterial("UnlitGeneric", "VGUI/white_additive", true, true, true, true, true);
		fakeChams = Util::CreateMaterial("VertexLitGeneric", "VGUI/white_additive", true, true, true, true, true);
		materialsCreated = true;
	}

	std::string modelName = pModelInfo->GetModelName(pInfo.pModel);

	if (modelName.find("models/player") != std::string::npos)
		DrawPlayer(matctx, state, pInfo, pCustomBoneToWorld);
}