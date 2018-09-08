#include "info.h"

bool Settings::Info::lbyBroken = true;
bool Settings::Info::lcBroken = true;

bool Settings::UI::waterMark = true;

AnimationLayer currentLayer;
AnimationLayer previousLayer;
int previousAct;

void WaterMark() {
	Draw::TextW(Vector2D(4, 4), L"CTHook re::code | BETA", esp_font, Color(255, 255, 255, 255));
}

void Info::Paint() {
	if (Settings::UI::waterMark)
		WaterMark();

	if (!pEngine->IsInGame())
		return;

	//C_BasePlayer* pLocal = (C_BasePlayer*)pEntityList->GetClientEntity(pEngine->GetLocalPlayer());

	if (!Globals::pLocal || !Globals::pLocal->GetAlive())
		return;

	int w, h;
	pEngine->GetScreenSize(w, h);

	Vector2D textSize = Draw::GetTextWSize(L"LBY", lby_font);
	Color lbyColor = AntiAim::lbyUpdated ? Color(255, 0, 0) : Color(0, 255, 0);
	Color lcColor = FakeLag::lcBroken ? Color(0, 255, 0) : Color(255, 0, 0);

	if (Settings::Info::lbyBroken && Settings::AntiAim::Yaw::enabled) {
		Draw::TextW(2, h - textSize.y * 2 + 5, L"LBY", lby_font, lbyColor);
		if (Settings::Info::lcBroken && Settings::FakeLag::enabled)
			Draw::TextW(7 + textSize.x, h - textSize.y * 2 + 5, L"LC", lby_font, lcColor);
	}
	else if (Settings::Info::lcBroken && Settings::FakeLag::enabled)
		Draw::TextW(2, h - textSize.y * 2 + 5, L"LC", lby_font, lcColor);

	/*auto poseParam = Globals::pLocal->GetPoseParameter();
	static auto poseParamBackup = poseParam;

	for (int i = 0; i <= DEATH_YAW; i++) {
		float poseParamValue = poseParam[i];

		std::string paramStr = "";

		switch (i) {
			case 0: paramStr = "STRAFE_YAW"; break;
			case 1: paramStr = "STAND"; break;
			case 2: paramStr = "LEAN_YAW"; break;
			case 3: paramStr = "SPEED"; break;
			case 4: paramStr = "LADDER_YAW"; break;
			case 5: paramStr = "LADDER_SPEED"; break;
			case 6: paramStr = "JUMP_FALL"; break;
			case 7: paramStr = "MOVE_YAW"; break;
			case 8: paramStr = "MOVE_BLEND_CROUCH"; break;
			case 9: paramStr = "MOVE_BLEND_WALK"; break;
			case 10: paramStr = "MOVE_BLEND_RUN"; break;
			case 11: paramStr = "BODY_YAW"; break;
			case 12: paramStr = "BODY_PITCH"; break;
			case 13: paramStr = "AIM_BLEND_STAND_IDLE"; break;
			case 14: paramStr = "AIM_BLEND_STAND_WALK"; break;
			case 15: paramStr = "AIM_BLEND_STAND_RUN"; break;
			case 16: paramStr = "AIM_BLEND_CROUCH_IDLE"; break;
			case 17: paramStr = "AIM_BLEND_CROUCH_WALK"; break;
			case 18: paramStr = "DEATH_YAW"; break;
		}

		Draw::Text(150, 100 + i * 13, std::string(std::to_string(i) + " " + paramStr + " " + std::to_string(poseParamValue) + " " + std::to_string(poseParamBackup[i])).c_str(), esp_font, Color(255, 255, 255));
		if (poseParamValue != poseParamBackup[i] && poseParamValue != 0.f && poseParamValue != 1.f)
			poseParamBackup[i] = poseParamValue;
	}*/

	/*AnimationLayer animLayer = Globals::pLocal->GetAnimationOverlay(3);
	float cycle = animLayer.m_flCycle, prevCycle = animLayer.m_flPrevCycle, 
		playbackRate = animLayer.m_flPlaybackRate, weight = animLayer.m_flWeight,
		weightDeltaRate = animLayer.m_flWeightDeltaRate;

	int order = animLayer.m_nOrder, sequence = animLayer.m_nSequence;

	for (int i = 0; i <= 4; i++) {
		std::string text = "";
		float value = 0.f;

		switch (i) {
		case 0: text = "Cycle"; value = cycle; break;
		case 1: text = "PrevCycle"; value = prevCycle; break;
		case 2: text = "PlaybackRate"; value = playbackRate; break;
		case 3: text = "Weight"; value = weight; break;
		case 4: text = "WeightDeltaRate"; value = weightDeltaRate; break;
		}

		Draw::Text(100, 100 + i * 13, std::string(text + " " + std::to_string(value)).c_str(), esp_font, Color(255, 255, 255));
	}

	for (int i = 5; i <= 6; i++) {
		std::string text = "";
		int value = 0.f;

		switch (i) {
		case 5: text = "Order"; value = order; break;
		case 6: text = "Sequence"; value = sequence; break;
		}

		Draw::Text(100, 100 + i * 13, std::string(text + " " + std::to_string(value)).c_str(), esp_font, Color(255, 255, 255));
	}*/

	/*currentLayer = Globals::pLocal->GetAnimationOverlay(3);
	const int currentAct = Resolver::GetSeqActivity(Globals::pLocal, pModelInfo->GetStudioModel(Globals::pLocal->GetModel()), currentLayer.m_nSequence);

	static int state = 0;

	if (currentAct == ACT_CSGO_IDLE_TURN_BALANCEADJUST) {
		state = 0;
		
		if (previousAct == ACT_CSGO_IDLE_TURN_BALANCEADJUST) // the previous layer must be trigerring 979
		{
			if ((previousLayer.m_flCycle != currentLayer.m_flCycle) || currentLayer.m_flWeight == 1.f)
			{
				if (currentLayer.m_flCycle < 0.01f && previousLayer.m_flCycle > 0.01f &&  BackTrack::IsTickValid(TIME_TO_TICKS(Globals::pLocal->GetSimulationTime() - currentLayer.m_flCycle))) // lby flicked here
				{
					state = 3;
				}
				state = 2;
			}
			else if (currentLayer.m_flWeight == 0.f && (previousLayer.m_flCycle > 0.92f && currentLayer.m_flCycle > 0.92f)) // breaking lby with delta < 120; can fakewalk here aswell
			{
				state = 1;
			}
		}
	}

	const wchar_t* lbyBreakState = L"";

	switch (state) {
	case 0: lbyBreakState = L"BROKEN"; break;
	case 1: lbyBreakState = L"UNDER"; break;
	case 2: lbyBreakState = L"OVER"; break;
	case 3: lbyBreakState = L"FLICK"; break;
	}

	Draw::TextW(145, 4, lbyBreakState, esp_font, Color(255, 255, 255));

	previousAct = currentAct;
	previousLayer = currentLayer;*/
}
