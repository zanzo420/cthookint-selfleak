#include "IEngineTrace.h"
#include "../interfaces.h"

// needed to put this into cpp because vs is gay

bool CGameTrace::DidHitWorld() const
{
	return m_pEnt == pEntityList->GetClientEntity(0);
}

bool CGameTrace::DidHitNonWorldEntity() const
{
	return m_pEnt != nullptr && !DidHitWorld();
}