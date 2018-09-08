#include "esp.h"

bool Settings::ESP::enabled = true;
bool Settings::ESP::players = true;
bool Settings::ESP::enemiesOnly = true;
bool Settings::ESP::drawLocal = false;
bool Settings::ESP::healthBar = true;
bool Settings::ESP::health = true;
bool Settings::ESP::resolverInfo = true;
bool Settings::ESP::boxes = true;
bool Settings::ESP::name = true;
bool Settings::ESP::c4 = true;
bool Settings::ESP::plantedC4 = true;
bool Settings::ESP::nades = true;
bool Settings::ESP::weapons = true;
WeaponType Settings::ESP::weaponsType = WeaponType::CSGO;
bool Settings::ESP::droppedWeapons = true;
WeaponType Settings::ESP::droppedWeaponsType = WeaponType::CSGO;
bool Settings::ESP::hostage = true;
bool Settings::ESP::other = true;
bool Settings::ESP::defuseKit = true;
bool Settings::ESP::skeleton = true;
bool Settings::ESP::backtrack = true;
bool Settings::ESP::forwardtrack = false;
bool Settings::ESP::aaArrows = true;
bool Settings::ESP::spreadCrosshair = true;
bool Settings::ESP::ammobar = false;
bool Settings::ESP::armorbar2 = false;

bool Settings::ESP::flag = false;

bool Settings::NoScopeBorder::enabled = true;
#undef min
#undef max

bool GetBox(C_BaseEntity* pEntity, int& x, int& y, int& w, int& h) {
	// Variables
	Vector vOrigin, min, max, flb, brt, blb, frt, frb, brb, blt, flt;
	float left, top, right, bottom;

	// Get the locations
	vOrigin = pEntity->GetVecOrigin();
	min = pEntity->GetCollideable()->OBBMins() + vOrigin;
	max = pEntity->GetCollideable()->OBBMaxs() + vOrigin;

	// Points of a 3d bounding box
	Vector points[] = { Vector(min.x, min.y, min.z),
		Vector(min.x, max.y, min.z),
		Vector(max.x, max.y, min.z),
		Vector(max.x, min.y, min.z),
		Vector(max.x, max.y, max.z),
		Vector(min.x, max.y, max.z),
		Vector(min.x, min.y, max.z),
		Vector(max.x, min.y, max.z) };

	// Get screen positions
	if (pDebugOverlay->ScreenPosition(points[3], flb) || pDebugOverlay->ScreenPosition(points[5], brt)
		|| pDebugOverlay->ScreenPosition(points[0], blb) || pDebugOverlay->ScreenPosition(points[4], frt)
		|| pDebugOverlay->ScreenPosition(points[2], frb) || pDebugOverlay->ScreenPosition(points[1], brb)
		|| pDebugOverlay->ScreenPosition(points[6], blt) || pDebugOverlay->ScreenPosition(points[7], flt))
		return false;

	// Put them in an array (maybe start them off in one later for speed?)
	Vector arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };

	// Init this shit
	left = flb.x;
	top = flb.y;
	right = flb.x;
	bottom = flb.y;

	// Find the bounding corners for our box
	for (int i = 1; i < 8; i++) {
		if (left > arr[i].x)
			left = arr[i].x;
		if (bottom < arr[i].y)
			bottom = arr[i].y;
		if (right < arr[i].x)
			right = arr[i].x;
		if (top > arr[i].y)
			top = arr[i].y;
	}

	// Width / height
	x = (int)left;
	y = (int)top;
	w = (int)(right - left);
	h = (int)(bottom - top);

	return true;
}

void DrawPentaBox(Color color, C_BasePlayer* pPlayer) {
	int x, y, w, h;
	if (!GetBox(pPlayer, x, y, w, h))
		return;

	int middle = x + w / 2;

	float halfw = w / 2;

	float a = sqrt(halfw * halfw + h * h);

	a *= 0.8f;

	Draw::Line(middle, y, x, y + h, color);
	Draw::Line(middle, y, x + w, y + h, color);
	Draw::Line(middle - a / 2, y + h / 3, middle + a / 2, y + h / 3, color);
	Draw::Line(x, y + h, middle + a / 2, y + h / 3, color);
	Draw::Line(x + w, y + h, middle - a / 2, y + h / 3, color);
}

void DrawBox(Color color, int x, int y, int w, int h) {
	// color
	Draw::Rectangle(x, y, x + w, y + h, color);
	// outer outline
	Draw::Rectangle(x + 1, y + 1, x + w - 1, y + h - 1, Color(10, 10, 10, 50));
	// inner outline
	Draw::Rectangle(x - 1, y - 1, x + w + 1, y + h + 1, Color(10, 10, 10, 50));
}

void DrawName(int x, int y, int w, int h, std::string name) {
	Vector2D nameSize = Draw::GetTextSize(name.c_str(), esp_name_font);
	Draw::Text(x + w / 2 - nameSize.x / 2, y - nameSize.y, name.c_str(), esp_name_font, Color(255, 255, 255, 255));
}

void DrawEntity(C_BaseEntity* pEntity, std::string name) {
	int x, y, w, h;
	if (!GetBox(pEntity, x, y, w, h))
		return;

	Vector2D nameSize = Draw::GetTextSize(name.c_str(), esp_name_font);
	Draw::Text(x + w / 2 - nameSize.x / 2, y + h / 2, name.c_str(), esp_name_font, Color(255, 255, 255, 255));
}

void DrawEntityFont(C_BaseEntity* pEntity, std::string name, HFont font) {
	int x, y, w, h;
	if (!GetBox(pEntity, x, y, w, h))
		return;

	Vector2D nameSize = Draw::GetTextSize(name.c_str(), font);
	Draw::Text(x + w / 2 - nameSize.x / 2, y + h / 2, name.c_str(), font, Color(255, 255, 255, 255));
}

void DrawPlantedBomb(C_PlantedC4* pBomb) {
	int x, y, w, h;
	if (!GetBox(pBomb, x, y, w, h))
		return;

	float bombTime = pBomb->GetBombTime() - pGlobalVars->curtime;

	std::stringstream ss;
	ss << std::fixed << std::setprecision(1) << bombTime;

	std::string bombTimeFixed = ss.str();

	if (pBomb->IsBombDefused() || !pBomb->IsBombTicking() || bombTime <= 0.f)
		DrawEntity(pBomb, "Bomb");
	else
		DrawEntity(pBomb, "Bomb: " + bombTimeFixed);
}

void DrawSkeleton(C_BasePlayer* pPlayer) {
	studiohdr_t* pStudioModel = pModelInfo->GetStudioModel(pPlayer->GetModel());
	if (!pStudioModel)
		return;

	static matrix3x4_t pBoneToWorldOut[128];
	if (pPlayer->SetupBones(pBoneToWorldOut, 128, 256, 0))
	{
		for (int i = 0; i < pStudioModel->numbones; i++)
		{
			mstudiobone_t* pBone = pStudioModel->pBone(i);
			if (!pBone || !(pBone->flags & 256) || pBone->parent == -1)
				continue;

			Vector vBonePos1;
			if (pDebugOverlay->ScreenPosition(Vector(pBoneToWorldOut[i][0][3], pBoneToWorldOut[i][1][3], pBoneToWorldOut[i][2][3]), vBonePos1))
				continue;

			Vector vBonePos2;
			if (pDebugOverlay->ScreenPosition(Vector(pBoneToWorldOut[pBone->parent][0][3], pBoneToWorldOut[pBone->parent][1][3], pBoneToWorldOut[pBone->parent][2][3]), vBonePos2))
				continue;

			Draw::Line(Vector2D(vBonePos1.x, vBonePos1.y), Vector2D(vBonePos2.x, vBonePos2.y), Color(255, 255, 255));
		}
	}
}

void DrawDroppedWeapon(C_BaseCombatWeapon* pWeapon) {
	Vector vecOrigin = pWeapon->GetVecOrigin();

	int owner = pWeapon->GetOwner();

	if (owner > -1 || (vecOrigin.x == 0 && vecOrigin.y == 0 && vecOrigin.z == 0))
		return;

	switch (Settings::ESP::droppedWeaponsType) {
		case WeaponType::NAME: {
			std::string modelName = Util::Items::GetItemDisplayName(*pWeapon->GetItemDefinitionIndex());

			if (pWeapon->GetAmmo() != -1) {
				modelName += " (";
				modelName += std::to_string(pWeapon->GetAmmo());
				modelName += ")";
			}

			DrawEntity((C_BaseEntity*)pWeapon, modelName);
			break;
		}
		case WeaponType::CS16: {
			std::string gunIcon = pWeapon->GunIcon();

			DrawEntityFont((C_BaseEntity*)pWeapon, gunIcon, weapon_icon);
			break;
		}
		case WeaponType::CSGO: {
			std::string gunIcon = pWeapon->GunIconCustom();

			DrawEntityFont((C_BaseEntity*)pWeapon, gunIcon, weapon_icon_custom);
			break;
		}
	}
}

void DrawHealthBar(int x, int y, int w, int h, int hp) {
	int health = hp;

	if (health > 100)
		health = 100;

	int g = int(health * 2.55f); // p calculations for color pasted from csgosimple :jew:
	int r = 255 - g;

	int boxSpacing = 3;

	float healthPerc = health / 100.f;

	int barx = x;
	int bary = y;
	int barw = w;
	int barh = h;

	barw = 4;
	barx -= barw + boxSpacing;

	Draw::Rectangle(barx, bary, barx + barw, bary + barh, Color(10, 10, 10, 200));

	if (healthPerc > 0)
		Draw::FilledRectangle(barx + 1, bary + (barh * (1.f - healthPerc)) + 1, barx + barw - 1, bary + barh - 1, Color(r, g, 0));
}
void DrawHealth(int x, int y, int w, int h, int hp) {
	std::string health = std::to_string(hp);
	Vector2D textSize = Draw::GetTextSize(health.c_str(), esp_font);

	int healthLock = hp;

	if (healthLock > 100)
		healthLock = 100;

	float healthPerc = healthLock / 100.f;

	int barx = x;
	int bary = y;
	int barw = w;
	int barh = h;
	if (healthLock < 100)
		Draw::Text(barx - textSize.x / 2 - 4, bary + (barh * (1.f - healthPerc)) + 1, health.c_str(), esp_font, Color(255, 255, 255));
}

void ResolverAct(int x, int y, int w, int h, C_BasePlayer* pPlayer) {
	AnimationLayer curLayer;
	curLayer = pPlayer->GetAnimationOverlay(3);

	const int curLayerAct = Resolver::GetSeqActivity(pPlayer, pModelInfo->GetStudioModel(pPlayer->GetModel()), curLayer.m_nSequence);

	Vector2D textSize = Draw::GetTextSize(std::to_string(curLayerAct).c_str(), esp_font);

	std::string weight = "Weight ";
	weight += std::to_string(curLayer.m_flWeight);

	std::string cycle = "Cycle ";
	cycle += std::to_string(curLayer.m_flCycle);

	Draw::Text(x + w, y - textSize.y, std::to_string(curLayerAct).c_str(), esp_font, Color(255, 255, 255));
	Draw::Text(x + w, y, cycle.c_str(), esp_font, Color(255, 255, 255));
	Draw::Text(x + w, y + textSize.y, weight.c_str(), esp_font, Color(255, 255, 255));
}

void LBYTimer(int x, int y, int w, int h, C_BasePlayer* pPlayer) {
	if (pPlayer->GetVelocity().Length2D() != 0)
		return;

	float nextUpdate = Resolver::nextUpdate[pPlayer->GetIndex()];
	float updateTime = Resolver::updateTime[pPlayer->GetIndex()];
	float simTime = pPlayer->GetSimulationTime();

	float lbyTime = nextUpdate - simTime;

	float perc = lbyTime / updateTime; // % of the box

	Draw::Rectangle(Vector2D(x, y + h + 4), Vector2D(x + w, y + h + 8), Color(10, 10, 10, 200));
	Draw::FilledRectangle(Vector2D(x + 1, y + h + 5), Vector2D(x + w * perc - 1, y + h + 7), Color(138, 34, 111, 225));
}

void DrawFlags(int x, int y, int w, int h, C_BasePlayer* pPlayer) {
	Vector2D textSize = Draw::GetTextWSize(L"K", esp_font);

	if (pPlayer->HasHelmet() || pPlayer->GetArmor() > 0) {
		if (pPlayer->HasHelmet())
			Draw::TextW(x + w + 2, y + textSize.y, L"HK", esp_font, Color(255, 255, 255));
		else if (pPlayer->GetArmor() > 0)
			Draw::TextW(x + w + 2, y + textSize.y, L"K", esp_font, Color(255, 255, 255));

		if (pPlayer->IsScoped()) {
			Draw::TextW(x + w + 2, y + textSize.y * 2, L"ZOOM", esp_font, Color(255, 255, 255));

			if (pPlayer->GetIndex() == (*csPlayerResource)->GetPlayerC4())
				Draw::TextW(x + w + 2, y + textSize.y * 3, L"B", esp_font, Color(255, 255, 255));
		} else 	if (pPlayer->GetIndex() == (*csPlayerResource)->GetPlayerC4())
			Draw::TextW(x + w + 2, y + textSize.y * 2, L"B", esp_font, Color(255, 255, 255));
	}
	else if (pPlayer->IsScoped()) {
		Draw::TextW(x + w + 2, y + textSize.y, L"ZOOM", esp_font, Color(255, 255, 255));

		if (pPlayer->GetIndex() == (*csPlayerResource)->GetPlayerC4())
			Draw::TextW(x + w + 2, y + textSize.y * 2, L"B", esp_font, Color(255, 255, 255));
	} else if (pPlayer->GetIndex() == (*csPlayerResource)->GetPlayerC4())
		Draw::TextW(x + w + 2, y + textSize.y, L"B", esp_font, Color(255, 255, 255));
}

void ResolverInfo(int x, int y, int w, int h, C_BasePlayer* pPlayer) {
	/*float nextUpdate = Resolver::nextUpdate[pPlayer->GetIndex()] - pPlayer->GetSimulationTime();
	//std::string nextUpdateStr = std::to_string(roundf(nextUpdate * 10) / 10);
	std::stringstream ss;
	ss << std::fixed << std::setprecision(1) << nextUpdate;
	std::string nextUpdateStr = ss.str();
	Vector2D textSize = Draw::GetTextSize(nextUpdateStr.c_str(), esp_font);

	std::string shotsMissedStr = std::to_string(Resolver::shotsMissed[pPlayer->GetIndex()]);

	std::string lbyDelta = "LBY Delta ";
	lbyDelta += std::to_string(Resolver::lbyDelta[pPlayer->GetIndex()]);

	//std::string fakeAA;

	//switch (Resolver::fakeAa[pPlayer->GetIndex()]) {
	//	case AAType::CANTDETECT: fakeAA = "CAN'T DETECT"; break;
	//	case AAType::FAKESPIN: fakeAA = "FAKE SPIN"; break;
	//	case AAType::LBY: fakeAA = "FAKE LBY"; break;
	//	case AAType::STATIC: fakeAA = "FAKE STATIC"; break;
	//	case AAType::JITTER2VALUES: fakeAA = "JITTER BETWEEN 2 VALUES"; break;
	//}

	//Draw::Text(x + w, y, fakeAA.c_str(), esp_font, Color(255, 255, 255));

	Draw::Text(x + w, y - textSize.y, nextUpdateStr.c_str(), esp_font, Color(255, 255, 255));
	Draw::Text(x + w, y, shotsMissedStr.c_str(), esp_font, Color(255, 255, 255));
	Draw::Text(x + w, y + textSize.y, lbyDelta.c_str(), esp_font, Color(255, 255, 255));*/

	int id = pPlayer->GetIndex();

	std::string simTime = "CHOKED " + std::to_string(TIME_TO_TICKS(Resolver::simTimeDelta[id] - pGlobalVars->interval_per_tick));

	const wchar_t* resType = L"UNDEFINED";

	switch (Resolver::resolveType[id]) {
	case ResolverType::LEGIT:
		resType = L"LEGIT"; break;
	case ResolverType::SHOOTING:
		resType = L"SHOOTING"; break;
	case ResolverType::BRUTE:
		resType = L"BRUTE"; break;
	case ResolverType::FAKEWALK:
		resType = L"FAKE WALK"; break;
	case ResolverType::LASTMOVE:
		resType = L"LAST MOVE"; break;
	case ResolverType::LBYAFTERMOVING:
		resType = L"LBY AFTER MOVING"; break;
	case ResolverType::LASTMOVEFW:
		resType = L"LAST MOVE FAKEWALK"; break;
	case ResolverType::LBY:
		resType = L"LBY"; break;
	case ResolverType::SPIN:
		resType = L"SPIN"; break;
	case ResolverType::LBYBT:
		resType = L"LBY BT"; break;
	case ResolverType::LBYDELTA:
		resType = L"LBY DELTA"; break;
	case ResolverType::LBYINVERSE:
		resType = L"LBY INVERSE"; break;
	case ResolverType::FREESTANDINGB:
		resType = L"FREESTANDING BACK"; break;
	case ResolverType::FREESTANDINGL:
		resType = L"FREESTANDING LEFT"; break;
	case ResolverType::FREESTANDINGR:
		resType = L"FREESTANDING RIGHT"; break;
	case ResolverType::FREESTANDINGBRUTE:
		resType = L"FREESTANDING BRUTE"; break;
	case ResolverType::FREESTANDINGAUTO:
		resType = L"FREESTANDING AUTO"; break;
	case ResolverType::LASTHIT:
		resType = L"LAST HIT"; break;
	case ResolverType::OVERRIDE:
		resType = L"OVERRIDE"; break;
	case ResolverType::PLIST:
		resType = L"PLIST"; break;
	}

	Vector2D textSize = Draw::GetTextSize("A", esp_font);

	//std::string shots = "MISSED ";
	//shots += std::to_string(Resolver::shotsMissed[id]);

	Draw::TextW(x + w + 2, y, resType, esp_font, Color(255, 255, 255));
	//Draw::Text(x + w + 2, y + textSize.y, shots.c_str(), esp_font, Color(255, 255, 255));
	//Draw::Text(x + w + 2, y + textSize.y * 2, std::to_string(Resolver::wasBreakingOver120[id]).c_str(), esp_font, Color(255, 255, 255));
	//std::stringstream ss;
	//ss << std::fixed << std::setprecision(1) << Resolver::paramDiff[id];
	//Draw::Text(x + w + 2, y + textSize.y * 2, ss.str().c_str(), esp_font, Color(255, 255, 255));

	//LBYTimer(x, y, w, h, pPlayer);
}

void DrawSkeletonBt(C_BasePlayer* pPlayer, std::vector<CTickRecord>::iterator record) {
	studiohdr_t* pStudioModel = pModelInfo->GetStudioModel(pPlayer->GetModel());
	if (!pStudioModel)
		return;

	if (record->matrixBuilt)
	{
		for (int i = 0; i < pStudioModel->numbones; i++)
		{
			mstudiobone_t* pBone = pStudioModel->pBone(i);
			if (!pBone || !(pBone->flags & 256) || pBone->parent == -1)
				continue;

			Vector vBonePos1;
			if (pDebugOverlay->ScreenPosition(Vector(record->matrix[i][0][3], record->matrix[i][1][3], record->matrix[i][2][3]), vBonePos1))
				continue;

			Vector vBonePos2;
			if (pDebugOverlay->ScreenPosition(Vector(record->matrix[pBone->parent][0][3], record->matrix[pBone->parent][1][3], record->matrix[pBone->parent][2][3]), vBonePos2))
				continue;

			Draw::Line(Vector2D(vBonePos1.x, vBonePos1.y), Vector2D(vBonePos2.x, vBonePos2.y), Color(255, 255, 255));
		}
	}
}

void DrawSkeletonBt(C_BasePlayer* pPlayer, CTickRecord record) {
	studiohdr_t* pStudioModel = pModelInfo->GetStudioModel(pPlayer->GetModel());
	if (!pStudioModel)
		return;

	if (record.matrixBuilt)
	{
		for (int i = 0; i < pStudioModel->numbones; i++)
		{
			mstudiobone_t* pBone = pStudioModel->pBone(i);
			if (!pBone || !(pBone->flags & 256) || pBone->parent == -1)
				continue;

			Vector vBonePos1;
			if (pDebugOverlay->ScreenPosition(Vector(record.matrix[i][0][3], record.matrix[i][1][3], record.matrix[i][2][3]), vBonePos1))
				continue;

			Vector vBonePos2;
			if (pDebugOverlay->ScreenPosition(Vector(record.matrix[pBone->parent][0][3], record.matrix[pBone->parent][1][3], record.matrix[pBone->parent][2][3]), vBonePos2))
				continue;

			Draw::Line(Vector2D(vBonePos1.x, vBonePos1.y), Vector2D(vBonePos2.x, vBonePos2.y), Color(255, 255, 255));
		}
	}
}

void DrawBacktrack(C_BasePlayer* pPlayer) {
	int id = pPlayer->GetIndex();

	if (BackTrack::backtrackRecords[id].empty())
		return;

	//C_BasePlayer* pLocal = (C_BasePlayer*)pEntityList->GetClientEntity(pEngine->GetLocalPlayer());
	//Vector headprints2D[BackTrack::MAX_QUEUE_SIZE + 1];
	//(pPlayer->GetTeam() == pLocal->GetTeam() ? Color(15, 100, 225) : Color(225, 100, 15))
	Color finalColor = Color(225, 100, 15);

	/*static Vector prevOrig = pPlayer->GetVecOrigin();

	static float oldLby = *pPlayer->GetLowerBodyYawTarget();

	for (auto record = BackTrack::backtrackRecords[id].begin(); record != BackTrack::backtrackRecords[id].end(); record++) {
	/*Vector output = Vector(0, 0, 0);
	pDebugOverlay->ScreenPosition(
	record->headPos,
	output);// headprints2D[shit]
	if(!output.IsZero())
	Draw::Rectangle(output.x - 2, output.y - 2, output.x + 2, output.y + 2, finalColor);*/
	//	Draw::FilledCircle(Vector2D(output.x,output.y), 10, 2, finalColor);

	/*if (record->m_vecOrigin == prevOrig)
	continue;

	oldLby = record->m_flLowerBodyYawTarget;

	prevOrig = record->m_vecOrigin;

	DrawSkeletonBt(pPlayer, record);
	}*/

	Vector origin = pPlayer->GetVecOrigin();

	auto record = BackTrack::backtrackRecords[id].front();
	if (record.m_vecOrigin == origin)
		return;

	DrawSkeletonBt(pPlayer, record);
}

void ESP::DrawHitboxes(int player, int r, int g, int b, int a, float duration)
{
	C_BasePlayer* pPlayer = (C_BasePlayer*)pEntityList->GetClientEntity(player);
	
	if (!pPlayer)
		return;

	matrix3x4_t boneMatrix_actual[MAXSTUDIOBONES];
	if (!pPlayer->SetupBones(boneMatrix_actual, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, pEngine->GetLastTimeStamp()))
		return;

	int id = pPlayer->GetIndex();

	if (Settings::Aimbot::backtrack && BackTrack::backtrackRecords[id].empty())
		return;

	studiohdr_t *studioHdr = pModelInfo->GetStudioModel(pPlayer->GetModel());
	if (studioHdr)
	{
		matrix3x4_t boneMatrix[MAXSTUDIOBONES];
		std::memcpy(boneMatrix, BackTrack::backtrackRecords[id].back().matrix, sizeof(BackTrack::backtrackRecords[id].back().matrix));

		mstudiohitboxset_t *set = studioHdr->pHitboxSet(*pPlayer->GetHitboxSet());
		if (set)
		{
			for (int i = 0; i < set->numhitboxes; i++)
			{
				mstudiobbox_t *hitbox = set->pHitbox(i);
				if (hitbox)
				{
					if (hitbox->radius == -1.0f)
					{
						Vector position, position_actual;
						QAngle angles, angles_actual;
						Math::MatrixAngles(boneMatrix[hitbox->bone], angles, position);
						Math::MatrixAngles(boneMatrix_actual[hitbox->bone], angles_actual, position_actual);

						pDebugOverlay->AddBoxOverlay(position, hitbox->bbmin, hitbox->bbmax, angles, r, g, b, a, duration);

						//if (g_Options.esp_lagcompensated_hitboxes_type == 1)
						//	g_DebugOverlay->AddBoxOverlay(position_actual, hitbox->bbmin, hitbox->bbmax, angles_actual, 0, 0, 255, 150, duration);
					}
					else
					{
						Vector min, max,
							min_actual, max_actual;

						Math::VectorTransform(hitbox->bbmin, boneMatrix[hitbox->bone], min);
						Math::VectorTransform(hitbox->bbmax, boneMatrix[hitbox->bone], max);

						Math::VectorTransform(hitbox->bbmin, boneMatrix_actual[hitbox->bone], min_actual);
						Math::VectorTransform(hitbox->bbmax, boneMatrix_actual[hitbox->bone], max_actual);

						pDebugOverlay->AddCapsuleOverlay(min, max, hitbox->radius, r, g, b, a, duration);

						//if (g_Options.esp_lagcompensated_hitboxes_type == 1)
						//	g_DebugOverlay->AddCapsuleOverlay(min_actual, max_actual, hitbox->m_flRadius, 0, 0, 255, 150, duration);
					}
				}
			}
		}
	}
}

Vector Extrplt(C_BasePlayer* pTarget, Vector point, float value) {
	point += pTarget->GetVelocity() * (pGlobalVars->interval_per_tick * (float)value);

	return point;
}

void DrawForwardtrack(C_BasePlayer* pPlayer) {
	INetChannelInfo *nci = pEngine->GetNetChannelInfo();
	auto ping = nci->GetLatency(FLOW_OUTGOING);
	if (ping > 0.199f) // check to not go outside of backtrack window (+200/-200ms)
	{
		ping = 0.199f - pGlobalVars->interval_per_tick;
	}

	Vector head = pPlayer->GetBonePosition((int)Bone::BONE_HEAD);
	Vector forwardTrack = Extrplt(pPlayer, head, TIME_TO_TICKS(ping));

	Vector output = Vector(0, 0, 0);
	pDebugOverlay->ScreenPosition(
		forwardTrack,
		output);// headprints2D[shit]
	if (!output.IsZero())
		Draw::FilledCircle(Vector2D(output.x, output.y), 10, 2, Color(0, 255, 0));
	//Draw::Rectangle(forwardTrack.x - 2, forwardTrack.y - 2, forwardTrack.x + 2, forwardTrack.y + 2, Color(0, 255, 0));
}

void DrawSpreadCrosshair() {
	//C_BasePlayer* pLocal = (C_BasePlayer*)pEntityList->GetClientEntity(pEngine->GetLocalPlayer());

	if (!Globals::pLocal || !Globals::pLocal->GetAlive())
		return;

	C_BaseCombatWeapon* pActiveWeapon = (C_BaseCombatWeapon*)pEntityList->GetClientEntityFromHandle(Globals::pLocal->GetActiveWeapon());

	if (!pActiveWeapon)
		return;

	int w, h;
	pEngine->GetScreenSize(w, h);

	float cone = pActiveWeapon->GetSpread() + pActiveWeapon->GetInaccuracy();
	if (cone > 0.0f) {
		float r = (cone * h) / 1.5f;
		Draw::FilledCircle(Vector2D(w / 2, h / 2),
			30,
			r,
			Color(253, 156, 71, 75));
	}
}

void DrawZeusRange() {
	if (!pEngine->IsInGame() || !pEngine->IsConnected())
		return;

	if (!Globals::pLocal || !Globals::pLocal->GetAlive())
		return;

	// looks trash
	if (!pInput->ThirdPerson)
		return;

	C_BaseCombatWeapon* pActiveWeapon = (C_BaseCombatWeapon*)pEntityList->GetClientEntityFromHandle(Globals::pLocal->GetActiveWeapon());

	if (!pActiveWeapon)
		return;

	if (*pActiveWeapon->GetItemDefinitionIndex() != ItemDefinitionIndex::WEAPON_TASER)
		return;

	CCSWeaponInfo* weaponData = pActiveWeapon->GetCSWpnData();

	float step = M_PI * 2.0 / 1023; //adjust if you need 1-5 extra fps lol
	float rad = weaponData->GetRange();
	Vector origin = Vector(Globals::eyePos.x, Globals::eyePos.y - 25, Globals::eyePos.z);

	static double rainbow;

	Vector screenPos;
	static Vector prevScreenPos;

	for (float rotation = 0; rotation < (M_PI * 2.0); rotation += step)
	{
		Vector pos(rad * cos(rotation) + origin.x, rad * sin(rotation) + origin.y, origin.z);

		Ray_t ray;
		trace_t trace;
		CTraceFilter filter;

		filter.pSkip = Globals::pLocal;
		ray.Init(origin, pos);

		pTrace->TraceRay(ray, MASK_SHOT_BRUSHONLY, &filter, &trace);

		if (pDebugOverlay->ScreenPosition(trace.endpos, screenPos))
			continue;

		if (!prevScreenPos.IsZero() && !screenPos.IsZero() && screenPos.DistTo(Vector(-107374176, -107374176, -107374176)) > 3.f&& prevScreenPos.DistTo(Vector(-107374176, -107374176, -107374176)) > 3.f) {
			rainbow += 0.00001;
			if (rainbow > 1.f)
				rainbow = 0;

			Color color = Color::FromHSB(rainbow, 1.f, 1.f);

			Draw::Line(Vector2D(prevScreenPos.x, prevScreenPos.y), Vector2D(screenPos.x, screenPos.y), color);
			Draw::Line(Vector2D(prevScreenPos.x, prevScreenPos.y + 1), Vector2D(screenPos.x, screenPos.y + 1), color);
			//Draw::Line(Vector2D(prevScreenPos.x, prevScreenPos.y - 1), Vector2D(screenPos.x, screenPos.y - 1), color);
		}
		prevScreenPos = screenPos;
	}
}

float lastChange[64];
int ammoSave[64];

void DrawAmmoBar(int x, int y, int w, int h, C_BasePlayer* pPlayer) {
	C_BaseCombatWeapon* pActiveWeapon = (C_BaseCombatWeapon*)pEntityList->GetClientEntityFromHandle(pPlayer->GetActiveWeapon());
	if (!pActiveWeapon)
		return;
	
	int ammo = pActiveWeapon->GetAmmo();
	CCSWeaponInfo* weaponData = pActiveWeapon->GetCSWpnData();
	int maxClip = weaponData->GetMaxClip();
	
	int boxSpacing = 3;

	int id = pPlayer->GetIndex();

	if (ammoSave[id] != ammo) {
		lastChange[id] = pGlobalVars->curtime;
		ammoSave[id] = ammo;
	}

	float ammoPerc = (float)(ammo + 1) / (float)maxClip;
	float timeDiff = (pGlobalVars->curtime - lastChange[id]) * 10.f;

	if (timeDiff > 1.f)
		timeDiff = 1.f;

	ammoPerc -= ((1.f / (float)maxClip) * timeDiff) - (1.f / (float)w); // 1px behind idk why

	// limit
	if (ammoPerc > 1.f)
		ammoPerc = 1.f;

	int barx = x;
	int bary = y;
	int barw = w;
	int barh = h;

	bary += barh + boxSpacing; // player box(?px) + spacing(1px) + outline(1px) + bar(2px) + outline (1px) = 5 px
	barh = 4; // outline(1px) + bar(2px) + outline(1px) = 4px;

	barw *= ammoPerc;
	
	if (maxClip > 0) {
		Draw::Rectangle(barx, bary, barx + w, bary + barh, Color(10, 10, 10, 200));

		if (ammoPerc > 0)
			Draw::FilledRectangle(barx + 1, bary + 1, barx + barw - 1, bary + barh - 1, Color(40, 140, 215));
	}
}

void DrawPlayer(C_BasePlayer* pPlayer, IEngineClient::player_info_t playerInfo) {
	int x, y, w, h;
	if (!GetBox(pPlayer, x, y, w, h))
		return;

	//C_BasePlayer* pLocal = (C_BasePlayer*)pEntityList->GetClientEntity(pEngine->GetLocalPlayer());

	if (!pEngine->IsConnected() || !pEngine->IsInGame())
		return;

	Color finalColor = Color(255, 255, 255, 100)/*((Settings::ESP::drawLocal && pPlayer == Globals::pLocal) ? Color(15, 225, 100) : (pPlayer->GetTeam() == Globals::pLocal->GetTeam()) ? Color(15, 100, 225) : Color(225, 100, 15))*/;

	if (Settings::ESP::boxes)
		DrawBox(finalColor, x, y, w, h);
	//todo: add option for penta box	
	//DrawPentaBox(finalColor, pPlayer);

	if (Settings::ESP::name)
		DrawName(x, y, w, h, playerInfo.name);

	if (Settings::ESP::healthBar)
		DrawHealthBar(x, y, w, h, pPlayer->GetHealth());
	if (Settings::ESP::flag)
		DrawFlags(x, y, w, h, pPlayer);

	if (Settings::ESP::ammobar)
		DrawAmmoBar(x, y, w, h, pPlayer);

	if (Settings::ESP::healthBar)
		DrawHealth(x, y, w, h, pPlayer->GetHealth());

	if (pPlayer != Globals::pLocal && Settings::ESP::resolverInfo && Settings::Resolver::enabled)
		ResolverInfo(x, y, w, h, pPlayer);

	if (Settings::ESP::skeleton)
		DrawSkeleton(pPlayer);

	if (pPlayer != Globals::pLocal && pPlayer->GetTeam() != Globals::pLocal->GetTeam()) {
		if (Settings::Aimbot::backtrack && Settings::ESP::backtrack)
			DrawBacktrack(pPlayer);

		if (Settings::ESP::forwardtrack && Settings::ESP::forwardtrack)
			DrawForwardtrack(pPlayer);
	}

	C_BaseCombatWeapon* pActiveWeapon = (C_BaseCombatWeapon*)pEntityList->GetClientEntityFromHandle(pPlayer->GetActiveWeapon());

	if (pActiveWeapon && Settings::ESP::weapons) {
		switch (Settings::ESP::weaponsType) {
			case WeaponType::NAME: {
				std::string weaponName = Util::Items::GetItemDisplayName(*pActiveWeapon->GetItemDefinitionIndex());

				if (pActiveWeapon->GetAmmo() != -1) {
					weaponName += " (";
					weaponName += std::to_string(pActiveWeapon->GetAmmo());
					weaponName += ")";
				}

				Vector2D textSize = Draw::GetTextSize(weaponName.c_str(), esp_font);

				Draw::Text(x + w / 2 - textSize.x / 2, y + h, weaponName.c_str(), esp_font, Color(255, 255, 255, 255));
				break;
			}
			case WeaponType::CS16: {
				std::string gunIcon = pActiveWeapon->GunIcon();

				Vector2D textSize = Draw::GetTextSize(gunIcon.c_str(), weapon_icon);

				Draw::Text(x + w / 2 - textSize.x / 2, y + h, gunIcon.c_str(), weapon_icon, Color(255, 255, 255, 255));
				break;
			}
			case WeaponType::CSGO: {
				std::string gunIcon = pActiveWeapon->GunIconCustom();

				Vector2D textSize = Draw::GetTextSize(gunIcon.c_str(), weapon_icon_custom);

				Draw::Text(x + w / 2 - textSize.x / 2, y + h, gunIcon.c_str(), weapon_icon_custom, Color(255, 255, 255, 255));
				break;
			}
		}
	}
}

void AAInfo() {
	if (!Settings::AntiAim::Yaw::enabled)
		return;

	// lol ok maybe display arrows based on freestanding/manual?
}

void DrawScope() {
	//C_BasePlayer* pLocal = (C_BasePlayer*)pEntityList->GetClientEntity(pEngine->GetLocalPlayer());
	if (!Globals::pLocal)
		return;

	C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*)pEntityList->GetClientEntityFromHandle(
		Globals::pLocal->GetActiveWeapon());
	if (!activeWeapon)
		return;

	if (*activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_SG556 ||
		*activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_AUG)
		return;

	int width, height;
	pEngine->GetScreenSize(width, height);

	Draw::Line(0, height * 0.5, width, height * 0.5, Color(0, 0, 0, 255));
	Draw::Line(width * 0.5, 0, width * 0.5, height, Color(0, 0, 0, 255));
}

bool ESP::PrePaintTraverse(VPANEL vgui_panel, bool force_repaint, bool allow_force) {
	if (Settings::ESP::enabled && Settings::NoScopeBorder::enabled &&
		strcmp("HudZoom", pPanel->GetName(vgui_panel)) == 0)
		return false;

	return true;
}

void DrawNade(C_BaseEntity* nade, ClientClass* client)
{
	model_t* nadeModel = nade->GetModel();

	if (!nadeModel)
		return;

	studiohdr_t* hdr = pModelInfo->GetStudioModel(nadeModel);

	if (!hdr)
		return;

	if (!strstr(hdr->name, "thrown") && !strstr(hdr->name, "dropped"))
		return;

	std::string nadeName = "Unknown Grenade";

	IMaterial* mats[32];
	pModelInfo->GetModelMaterials(nadeModel, hdr->numtextures, mats);

	for (int i = 0; i < hdr->numtextures; i++)
	{
		IMaterial* mat = mats[i];
		if (!mat)
			continue;

		if (strstr(mat->GetName(), "flashbang"))
		{
			nadeName = "Flash";
			break;
		}
		else if (strstr(mat->GetName(), "m67_grenade") || strstr(mat->GetName(), "hegrenade"))
		{
			nadeName = "HE";
			break;
		}
		else if (strstr(mat->GetName(), "smoke"))
		{
			nadeName = "Smoke";
			break;
		}
		else if (strstr(mat->GetName(), "decoy"))
		{
			nadeName = "Decoy";
			break;
		}
		else if (strstr(mat->GetName(), "incendiary") || strstr(mat->GetName(), "molotov"))
		{
			nadeName = "Molotov";
			break;
		}
	}

	DrawEntity(nade, nadeName.c_str());
}

void ESP::Paint() {
	if (!pEngine->IsInGame())
		return;

	if (!Settings::ESP::enabled)
		return;

	//C_BasePlayer* pLocal = (C_BasePlayer*)pEntityList->GetClientEntity(pEngine->GetLocalPlayer());

	if (!Globals::pLocal)
		return;

	if (Settings::ESP::aaArrows)
		AAInfo();

	if (Settings::ESP::spreadCrosshair)
		DrawSpreadCrosshair();

	DrawZeusRange();

	for (int i = 1; i < pEntityList->GetHighestEntityIndex(); ++i)
	{
		C_BaseEntity* pEntity = pEntityList->GetClientEntity(i);

		if (!pEntity)
			continue;

		ClientClass* pClient = pEntity->GetClientClass();

		if (pClient->m_ClassID == EClassIds::CCSPlayer && Settings::ESP::players)
		{
			C_BasePlayer* pPlayer = (C_BasePlayer*)pEntity;

			if (pPlayer->GetDormant() || !pPlayer->GetAlive())
				continue;

			if (!Settings::ESP::drawLocal && pPlayer == Globals::pLocal)
				continue;

			if (Settings::ESP::enemiesOnly && pPlayer->GetTeam() == Globals::pLocal->GetTeam() && pPlayer != Globals::pLocal)
				continue;

			IEngineClient::player_info_t playerInfo;
			if (pEngine->GetPlayerInfo(i, &playerInfo))
				DrawPlayer(pPlayer, playerInfo);
		}
		else if (Settings::ESP::droppedWeapons && (pClient->m_ClassID != EClassIds::CBaseWeaponWorldModel && (strstr(pClient->m_pNetworkName, "Weapon") || pClient->m_ClassID == EClassIds::CDEagle || pClient->m_ClassID == EClassIds::CAK47)))
			DrawDroppedWeapon((C_BaseCombatWeapon*)pEntity);
		else if (pClient->m_ClassID == EClassIds::CFish && Settings::ESP::other)
			DrawEntity(pEntity, "Fish");
		else if (pClient->m_ClassID == EClassIds::CChicken && Settings::ESP::other)
			DrawEntity(pEntity, "Chicken");
		else if (pClient->m_ClassID == EClassIds::CC4 && (*pGameRules)->IsBombDropped() && Settings::ESP::c4)
			DrawEntity(pEntity, "Dropped Bomb");
		else if (pClient->m_ClassID == EClassIds::CPlantedC4 && (*pGameRules)->IsBombPlanted() && Settings::ESP::plantedC4)
			DrawPlantedBomb((C_PlantedC4*)pEntity);
		else if (pClient->m_ClassID == EClassIds::CHostage && Settings::ESP::hostage)
			DrawEntity(pEntity, "Hostage");
		else if (pClient->m_ClassID == EClassIds::CBaseAnimating && Settings::ESP::defuseKit)
			DrawEntity(pEntity, "Defuse Kit");
		else if (strstr(pClient->m_pNetworkName, "Projectile") && Settings::ESP::nades)
			DrawNade(pEntity, pClient);
	}

	if (Settings::NoScopeBorder::enabled && Globals::pLocal->IsScoped())
		DrawScope();
}
