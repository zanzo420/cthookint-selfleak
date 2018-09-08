#pragma once

#include "../SDK/SDK.h"
#include "../settings.h"

namespace FOVChanger {
	//Hooks
	void OverrideView(ViewSetup* pSetup);

	void GetViewModelFOV(float& fov);
};