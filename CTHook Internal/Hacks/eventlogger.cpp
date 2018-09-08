#include "eventlogger.h"

bool Settings::UI::EventLogger::enabled = true;
float Settings::UI::EventLogger::fadeOut = 6.f;
										// UNKNOWN, ITEMPURCHASE, OBJECTIVE, PLAYERHURT, RESOLVER
bool Settings::UI::EventLogger::filters[] = { true, false, true, true, true };

const char* team[]{ "", "", " (T)", " (CT)" };
const char* defKit[]{ "without a defuse kit.", "with a defuse kit." };

std::map<int, const char*> bombsites = {
	{ 275, "Bombsite A" }, // Dust 2 v2
	{ 276, "Bombsite B" }, // Dust 2 v2
	{ 369, "Bombsite A" }, // Dust 2
	{ 366, "Bombsite B" }, // Dust 2
	{ 451, "Bombsite A" }, // Mirage
	{ 452, "Bombsite B" }, // Mirage
	{ 315, "Bombsite A" }, // Cache
	{ 316, "Bombsite B" }, // Cache
	{ 334, "Bombsite A" }, // Inferno
	{ 423, "Bombsite B" }, // Inferno
	{ 260, "Bombsite A" }, // Cobblestone
	{ 95,  "Bombsite B" }, // Cobblestone
	{ 79,  "Bombsite A" }, // Overpass
	{ 507, "Bombsite B" }, // Overpass
	{ 149, "Bombsite A" }, // Nuke
	{ 441, "Bombsite B" }, // Nuke
	{ 93,  "Bombsite A" }, // Train
	{ 538, "Bombsite B" }, // Train
	{ 222, "Bombsite A" }, // Canals
	{ 221, "Bombsite B" }, // Canals
	{ 239, "Bombsite A" }, // Aztec
	{ 238, "Bombsite B" }, // Aztec
	{ 81,  "Bombsite A" }, // Vertigo
	{ 82,  "Bombsite B" }, // Vertigo
	{ 526, "Bombsite A" }, // Dust 1
	{ 525, "Bombsite B" }, // Dust 1
};

void Capitalize(std::string& s);

std::string GetBombsiteByID(int id);

void EventLogger::Paint() {
	if (!Settings::UI::EventLogger::enabled || !pEngine->IsInGame())
		return;

	for (size_t i = 0; i < events.size(); i++) {
		float time = pGlobalVars->curtime - events[i].time;

		if (time > Settings::UI::EventLogger::fadeOut)
			events[i].color.a *= 0.98f;

		if (events[i].color.a < 0.5f)
			return;

		/*float height = i * ImGui::GetFontSize() + (Settings::UI::waterMark ?
			ImGui::GetFontSize() : 4.0f);*/

		Vector2D textSize = Draw::GetTextSize("A", esp_font);

		float height = i * textSize.y + (Settings::UI::waterMark ? textSize.y : 4.f);

		/*Draw::ImText(
			ImVec2(4.0f, 4.0f + height), events[i].color, events[i].text.c_str(), NULL, 0.0f, NULL,
			ImFontFlags_Shadow
		);*/
		Draw::Text(Vector2D(4.f, 4.f + height), events[i].text.c_str(), esp_font, events[i].color);
	}
}

void EventLogger::FireGameEvent(IGameEvent* event) {
	if (!event)
		return;

	if (!Settings::UI::EventLogger::enabled)
		return;

	if (strstr(event->GetName(), "player_hurt")) {
		C_BasePlayer* hurt = (C_BasePlayer*)pEntityList->GetClientEntity(
			pEngine->GetPlayerForUserID(event->GetInt("userid"))
		);
		if (!hurt)
			return;

		C_BasePlayer* attacker = (C_BasePlayer*)pEntityList->GetClientEntity(
			pEngine->GetPlayerForUserID(event->GetInt("attacker"))
		);
		if (event->GetInt("attacker") != 0) {
			if (!attacker)
				return;
		}

		int damage = event->GetInt("dmg_health");
		if (!damage)
			return;

		std::string hittext = "";
		int hitgroup = event->GetInt("hitgroup");

		switch ((HitGroups)hitgroup) {
		case HitGroups::HITGROUP_HEAD:
			hittext = " in head"; break;
		case HitGroups::HITGROUP_CHEST:
			hittext = " in chest"; break;
		case HitGroups::HITGROUP_STOMACH:
			hittext = " in stomach"; break;
		case HitGroups::HITGROUP_LEFTARM:
			hittext = " in left arm"; break;
		case HitGroups::HITGROUP_RIGHTARM:
			hittext = " in right arm"; break;
		case HitGroups::HITGROUP_LEFTLEG:
			hittext = " in left leg"; break;
		case HitGroups::HITGROUP_RIGHTLEG:
			hittext = " in right leg"; break;
		case HitGroups::HITGROUP_GEAR:
			hittext = " in gear"; break;
		case HitGroups::HITGROUP_GENERIC:
			hittext = " in generic"; break;
		}

		IEngineClient::player_info_t hurtPlayerInfo;
		pEngine->GetPlayerInfo(hurt->GetIndex(), &hurtPlayerInfo);

		IEngineClient::player_info_t attackerInfo;
		pEngine->GetPlayerInfo(attacker->GetIndex(), &attackerInfo);

		std::string attackerName = attackerInfo.name;
		if (event->GetInt("attacker") == 0) {
			attackerName = "World";
		}

		std::stringstream text;
		text << attackerName << " hit " << hurtPlayerInfo.name << hittext << " for " << damage
			<< " HP";

		if (hurt->GetAlive()) {
			if (!hurt->GetDormant()) {
				int health = hurt->GetHealth() - damage;

				if (health > 0) {
					text << " (" << health << " HP remaining)";
				}
				else {
					text << " (Dead)";
				}
			}
		}
		else {
			text << " (Dead)";
		}

		EventLogger::AddEvent(text.str(), Color(255, 255, 255), EventType::PLAYERHURT);
	}

	if (strstr(event->GetName(), "item_purchase")) {
		C_BasePlayer* user = (C_BasePlayer*)pEntityList->GetClientEntity(
			pEngine->GetPlayerForUserID(event->GetInt("userid"))
		);
		std::string weapon = std::string(event->GetString("weapon"));
		int teamNum = event->GetInt("team");

		if (!user)
			return;

		if (weapon.empty())
			return;

		if (!teamNum)
			return;

		IEngineClient::player_info_t entityInfo;
		pEngine->GetPlayerInfo(user->GetIndex(), &entityInfo);

		std::string prefix("weapon_");
		if (strncmp(weapon.c_str(), prefix.c_str(), prefix.size()) == 0) {
			weapon = weapon.substr(prefix.size());
		}
		std::replace(weapon.begin(), weapon.end(), '_', ' ');
		Capitalize(weapon);

		std::stringstream text;
		text << entityInfo.name << team[teamNum] << " purchased " << weapon;
		EventLogger::AddEvent(text.str(), Color(255, 255, 255), EventType::ITEMPURCHASE);
	}

	if (strstr(event->GetName(), "player_given_c4")) {
		C_BasePlayer* user = (C_BasePlayer*)pEntityList->GetClientEntity(
			pEngine->GetPlayerForUserID(event->GetInt("userid"))
		);

		if (!user)
			return;

		IEngineClient::player_info_t entityInfo;
		pEngine->GetPlayerInfo(user->GetIndex(), &entityInfo);

		std::stringstream text;
		text << entityInfo.name << " is now the bomb carrier";
		EventLogger::AddEvent(text.str(), Color(255, 255, 255), EventType::OBJECTIVE);
	}

	if (strstr(event->GetName(), "bomb_beginplant")) {
		C_BasePlayer* user = (C_BasePlayer*)pEntityList->GetClientEntity(
			pEngine->GetPlayerForUserID(event->GetInt("userid"))
		);
		int site = event->GetInt("site");

		if (!user)
			return;

		IEngineClient::player_info_t entityInfo;
		pEngine->GetPlayerInfo(user->GetIndex(), &entityInfo);

		std::stringstream text;
		text << entityInfo.name << " started planting at " << GetBombsiteByID(site);
		EventLogger::AddEvent(text.str(), Color(255, 111, 0), EventType::OBJECTIVE);
	}

	if (strstr(event->GetName(), "bomb_begindefuse")) {
		C_BasePlayer* user = (C_BasePlayer*)pEntityList->GetClientEntity(
			pEngine->GetPlayerForUserID(event->GetInt("userid"))
		);
		bool defuser = event->GetBool("haskit");

		if (!user)
			return;

		IEngineClient::player_info_t entityInfo;
		pEngine->GetPlayerInfo(user->GetIndex(), &entityInfo);

		std::stringstream text;
		text << entityInfo.name << " started defusing the bomb " << defKit[defuser];
		EventLogger::AddEvent(text.str(), Color(255, 111, 0), EventType::OBJECTIVE);
	}

	if (strstr(event->GetName(), "hostage_follows")) {
		C_BasePlayer* user = (C_BasePlayer*)pEntityList->GetClientEntity(
			pEngine->GetPlayerForUserID(event->GetInt("userid"))
		);

		if (!user)
			return;

		IEngineClient::player_info_t entityInfo;
		pEngine->GetPlayerInfo(user->GetIndex(), &entityInfo);

		std::stringstream text;
		text << entityInfo.name << " has picked up a hostage";
		EventLogger::AddEvent(text.str(), Color(255, 111, 0), EventType::OBJECTIVE);
	}


	if (strstr(event->GetName(), "hostage_rescued")) {
		C_BasePlayer* user = (C_BasePlayer*)pEntityList->GetClientEntity(
			pEngine->GetPlayerForUserID(event->GetInt("userid"))
		);

		if (!user)
			return;

		IEngineClient::player_info_t entityInfo;
		pEngine->GetPlayerInfo(user->GetIndex(), &entityInfo);

		std::stringstream text;
		text << entityInfo.name << " has rescued a hostage";
		EventLogger::AddEvent(text.str(), Color(255, 111, 0), EventType::OBJECTIVE);
	}

	if (strstr(event->GetName(), "bomb_planted")) {
		int site = event->GetInt("site");

		std::stringstream text("");
		text << "The bomb has been planted at " << GetBombsiteByID(site);
		EventLogger::AddEvent(text.str(), Color(255, 111, 0), EventType::OBJECTIVE);
	}

	if (strstr(event->GetName(), "enter_bombzone")) {
		C_BasePlayer* user = (C_BasePlayer*)pEntityList->GetClientEntity(
			pEngine->GetPlayerForUserID(event->GetInt("userid"))
		);
		bool bomb = event->GetBool("hasbomb");

		if (!user)
			return;

		IEngineClient::player_info_t entityInfo;
		pEngine->GetPlayerInfo(user->GetIndex(), &entityInfo);

		if (!event->GetBool("isplanted") && bomb) {
			std::stringstream text("Bomb carrier ");
			text << entityInfo.name << " has entered a bombsite";
			EventLogger::AddEvent(text.str(), Color(255, 255, 255), EventType::OBJECTIVE);
		}
	}
}

void EventLogger::AddEvent(std::string text, Color color, EventType eventType) {
	if (!Settings::UI::EventLogger::filters[(int)eventType])
		return;
	
	events.push_front(loginfo(text, color, pGlobalVars->curtime));

	if (events.size() > 12) {
		events.pop_back();
	}
}

std::string GetBombsiteByID(int id) {
	if (bombsites.find(id) == bombsites.end()) {
		pCvar->ConsoleColorPrintf(Color(255, 255, 255), "(EventLogger::GetBombsiteByID): Warning: "
			"Could not find localized string for ID \"%i\".\n",
			id);

		return "bombsite";
	}

	return bombsites.at(id);
}

void Capitalize(std::string& s) {
	bool cap = true;

	for (unsigned int i = 0; i <= s.length(); i++) {
		if (isalpha(s[i]) && cap) {
			s[i] = toupper(s[i]);
			cap = false;
		}
		else if (isspace(s[i])) {
			cap = true;
		}
	}
}