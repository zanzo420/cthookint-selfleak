#pragma once

#include "../settings.h"
#include "../SDK/SDK.h"
#include "../interfaces.h"
#include "../draw.h"
#include <deque>

struct loginfo {
	loginfo(std::string text, Color color, float time) {
		this->text = text;
		this->color = color;
		this->time = time;
	}

	std::string text;
	Color color;
	float time;
};

enum class EventType : int {
	UNKNOWN,
	ITEMPURCHASE,
	OBJECTIVE,
	PLAYERHURT,
	RESOLVER,
};

namespace EventLogger {
	static std::deque<loginfo> events = {};

	void AddEvent(std::string text, Color color = Color(255, 255, 255), EventType eventType = EventType::UNKNOWN);

	// Hooks
	void Paint();

	void FireGameEvent(IGameEvent* event);
}