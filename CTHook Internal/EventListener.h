#pragma once

#include "SDK\SDK.h"
#include "hooks.h"

#define EVENT_DEBUG_ID_INIT 42

class EventListener : public IGameEventListener2 {
public:
	EventListener(std::vector<const char*> events) {
		for (auto& it : events)
			pGameEvent->AddListener(this, it, false);
	}

	~EventListener() {
		pGameEvent->RemoveListener(this);
	}

	void FireGameEvent(IGameEvent* event) {
		HitMarker::FireGameEvent(event);
		Resolver::FireGameEvent(event);
		EventLogger::FireGameEvent(event);
	}

	int GetEventDebugID() override {
		return EVENT_DEBUG_ID_INIT;
	}
};