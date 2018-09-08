#pragma once

#include "../settings.h"
#include "../SDK/SDK.h"
#include "../interfaces.h"

#define FCVAR_CHEAT				(1<<14)

namespace NightMode {
	void FrameStageNotify(ClientFrameStage_t pStage);
}