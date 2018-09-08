#pragma once

#include <Windows.h>
#include <list>
#include "SDK/SDK.h"
#include "interfaces.h"

namespace Entity
{
	bool IsVisible(C_BasePlayer* player, Bone bone);
	bool IsPlanting(C_BasePlayer* player);
	Bone GetBoneByName(C_BasePlayer* player, const char* boneName);
}