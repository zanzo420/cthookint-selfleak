#pragma once 

#include "../settings.h"
#include "../SDK/SDK.h"
#include "../interfaces.h"
#include "../math.h"
#include "../draw.h"
#include "../utilSDK.h"
#include "../hooks.h"

namespace Chams {
	void DrawModelExecute(IMatRenderContext* matctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4* pCustomBoneToWorld);

	extern IMaterial* mat;
	extern IMaterial* matZ;
	extern IMaterial* matZF;
	extern IMaterial* fakeChams;
}