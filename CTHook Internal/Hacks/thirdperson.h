#pragma once

#include "../settings.h"
#include "../SDK/SDK.h"
#include "../interfaces.h"
#include "antiaim.h"
#include "chams.h"

namespace ThirdPerson {
	void FrameStageNotify(ClientFrameStage_t pStage);
	void SceneEnd();
}