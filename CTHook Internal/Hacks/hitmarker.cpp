#include "hitmarker.h"
bool Settings::ESP::backtrackHitboxes = false;
bool Settings::HitMarker::enabled = true;

void HitMarker::FrameStageNotify() {
	if (impacts.empty())
		return;

	if (!pEngine->IsInGame())
		return;

	if (impacts.size() > 10)
		impacts.pop_front();

	bool restart = false;
	for (auto impact = impacts.begin(); impact != impacts.end(); impact++) {
		if (restart) {
			impact = impacts.begin();
			restart = false;
		}

		float time = pGlobalVars->curtime - impact->time;

		if (time > 0.5f || impact->color.a < 50) {
			impacts.erase(impact);
			if (!impacts.empty()) {
				restart = true; // flag for restart
				impact = impacts.begin();
				continue;
			}
			else
				break;
		}

		impact->color.a = 255 / (time * 10);

		Vector screenposend;
		Vector screenposstart;

		if (pDebugOverlay->ScreenPosition(impact->pos, screenposend))
			continue;

		if (pDebugOverlay->ScreenPosition(impact->startPos, screenposstart))
			return;

		/*screenpos.y -= time * 50;

		Vector2D textSize = Draw::GetTextSize("X", esp_font);
		Draw::Text(screenpos.x - textSize.x / 2, screenpos.y - textSize.y / 2, "X", esp_font, impact->color);*/
		//Draw::Line(screenposstart.x, screenposstart.y, screenposend.x, screenposend.y, impact->color);
		Draw::FilledRectangle(screenposstart.x - 1, screenposstart.y - 1, screenposend.x + 1, screenposend.y + 1, impact->color);
	}
}

void HitMarker::FireGameEvent(IGameEvent* pEvent) {
	if (!pEngine->IsInGame())
		return;

	//C_BasePlayer* pLocal = (C_BasePlayer*)pEntityList->GetClientEntity(pEngine->GetLocalPlayer());

	if (!Globals::pLocal || !Globals::pLocal->GetAlive())
		return;
	
	if (strcmp(pEvent->GetName(), "player_hurt") == 0) {
		int hurtPlayerId = pEvent->GetInt("userid");
		int attackerId = pEvent->GetInt("attacker");

		// Self damage check
		if (pEngine->GetPlayerForUserID(hurtPlayerId) == pEngine->GetLocalPlayer())
			return;

		if (pEngine->GetPlayerForUserID(attackerId) != pEngine->GetLocalPlayer())
			return;

		if (Settings::HitMarker::enabled) {
			// Play sk00ter hitsound
			pEngine->ClientCmd_Unrestricted("play buttons\\arena_switch_press_02");
		}

		if (Settings::ESP::backtrackHitboxes)
			ESP::DrawHitboxes(pEngine->GetPlayerForUserID(hurtPlayerId), 167, 217, 7, 100, 3.5f);
	}

	if (strcmp(pEvent->GetName(), "bullet_impact") == 0) {
		int user = pEvent->GetInt("userid");

		if (pEngine->GetPlayerForUserID(user) != pEngine->GetLocalPlayer())
			return;

		Vector impact = Vector(pEvent->GetFloat("x"), pEvent->GetFloat("y"), pEvent->GetFloat("z"));

		impacts.push_back(impactInfo(impact, Globals::pLocal->GetEyePosition(), Color(255, 50, 10, 200), pGlobalVars->curtime));
	}
}