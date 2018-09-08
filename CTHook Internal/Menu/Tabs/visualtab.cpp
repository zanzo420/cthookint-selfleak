//
// Created by apostrophe on 01.11.17.
//

#include "visualtab.h"

void visualtab::RenderTab() {

	const char* BoxTypes[] = { "Flat 2D", "Frame 2D", "Box 3D" };
	const char* TracerTypes[] = { "Bottom", "Cursor" };
	const char* BarTypes[] = { "Vertical Left", "Vertical Right", "Horizontal Below", "Horizontal Above", "Interwebz" };
	const char* BarColorTypes[] = { "Static", "Health Based" };
	const char* TeamColorTypes[] = { "Absolute", "Relative" };
	const char* ChamsTypes[] = { "Normal", "Normal - XQZ", "Flat", "Flat - XQZ" };
	const char* ArmsTypes[] = { "Default", "Wireframe", "None" };
	const char* Sounds[] = { "None", "kickmetaldoor2", "default_doorstop", "metal", "dont_leave!", "thunder", "beep", "bass" , "sk00ter" };

	const char* WeaponTypes[] = { "Name", "CS 1.6", "CSGO" };

	ImGui::Columns(2, NULL, false);
	{
		ImGui::BeginChild("Vis1");
		{
			ImGui::Text("Visuals");
			ImGui::Separator();
			ImGui::Checkbox("Active##ESPON", &Settings::ESP::enabled);
			ImGui::Checkbox("ESP Box", &Settings::ESP::boxes);
			//ImGui::Combo("", (int*)&Settings::ESP::Boxes::type, BoxTypes, IM_ARRAYSIZE (BoxTypes));
			ImGui::Checkbox("Health Bar", &Settings::ESP::healthBar);
			ImGui::Checkbox("Ammo Bar", &Settings::ESP::ammobar);
			ImGui::Checkbox("LBY Bar", &Settings::ESP::armorbar2);

			//ImGui::Combo("", (int*)&Settings::ESP::Bars::type, BarTypes , IM_ARRAYSIZE (BarTypes));
			//ImGui::Combo("", (int*)&Settings::ESP::Bars::colorType , BarColorTypes , IM_ARRAYSIZE (BarColorTypes));
			ImGui::Checkbox("Draw Skeleton", &Settings::ESP::skeleton);
			ImGui::Checkbox("Draw Backtrack", &Settings::ESP::backtrack);
			//ImGui::Checkbox("offscreen", &Settings::ESP::offscreen);
			//ImGui::Checkbox("BulletTracers" ,&Settings::ESP::BulletTracers::enabled);
			ImGui::Checkbox("Show Backtrack Hitboxes", &Settings::ESP::backtrackHitboxes);
			//	ImGui::Checkbox("bullet tracer", &Settings::ESP::bullettracer);

			ImGui::Separator();
			ImGui::Text("Filters");
			ImGui::Checkbox("Enemies Only", &Settings::ESP::enemiesOnly);
			ImGui::Checkbox("Local Player", &Settings::ESP::drawLocal);
			ImGui::Checkbox("Other Entites", &Settings::ESP::other);
			//ImGui::Checkbox("Chickens" , &Settings::ESP::Filters::chickens);
			// ... TODO more filters if you need

			ImGui::Separator();
			ImGui::Text("Info");
			ImGui::Checkbox("Name", &Settings::ESP::name);
			ImGui::Checkbox("Weapon", &Settings::ESP::weapons);
			ImGui::Combo("Weapon Draw Type", (int*)&Settings::ESP::weaponsType, WeaponTypes, IM_ARRAYSIZE(WeaponTypes));

			//ImGui::Checkbox("Health", &Settings::ESP::health);
			ImGui::Checkbox("Resolver Info", &Settings::ESP::resolverInfo);
			ImGui::Checkbox("Flag", &Settings::ESP::flag);

			//ImGui::Checkbox("Health", &Settings::ESP::healthBar);
			//ImGui::Checkbox("Flashed", &Settings::ESP::Info::flashed);
			//ImGui::Checkbox("Clan" , &Settings::ESP::Info::clan);
			//ImGui::Checkbox("Defusing", &Settings::ESP::Info::defusing);
			//ImGui::Checkbox("Has Defuser", &Settings::ESP::Info::hasDefuser);
			//ImGui::Checkbox("Resolver Info", &Settings::ESP::Info::resolverInfo);
			// ... TODO complete info list
			ImGui::Separator();

			ImGui::Text("Hitmarker");
			ImGui::Checkbox("Active##HITMARKER", &Settings::HitMarker::enabled);
			//if(ImGui::Checkbox("Active##HITMARKER", &Settings::HitMarker::enabled))
			/*ImGui::SetTooltip("Notify when you hit another player");
			ImGui::SliderInt("##HITMARKERDUR", &Settings::ESP::Hitmarker::duration, 250, 3000, "Timeout: %0.f");
			ImGui::SliderInt("##HITMARKERSIZE", &Settings::ESP::Hitmarker::size, 1, 32, "Size: %0.f");
			ImGui::SliderInt("##HITMARKERGAP", &Settings::ESP::Hitmarker::innerGap, 1, 16, "Gap: %0.f");
			ImGui::Checkbox("Enemies##HITMARKERS", &Settings::ESP::Hitmarker::enemies);
			ImGui::Checkbox("Allies##HITMARKERS", &Settings::ESP::Hitmarker::allies);
			if(ImGui::Checkbox("Damage##HITMARKERS", &Settings::ESP::Hitmarker::Damage::enabled))
			ImGui::SetTooltip("Show dealt damage next to the hitmarker");
			ImGui::Checkbox("Sounds##HITMARKERS", &Settings::ESP::Hitmarker::Sounds::enabled);
			ImGui::Combo("Sounds", (int*)&Settings::ESP::Hitmarker::Sounds::sound, Sounds , IM_ARRAYSIZE(Sounds));*/


			ImGui::EndChild();
		}

	}
	ImGui::NextColumn();
	{
		ImGui::BeginChild("Vis2");
		{
			ImGui::Text("Chams");
			ImGui::Separator();

			ImGui::Checkbox("Active##CHAMSON", &Settings::ESP::chams);
			//ImGui::Combo("style##STYLECHAMS", (int*) &Settings::ESP::Chams::type , ChamsTypes , IM_ARRAYSIZE (ChamsTypes));

			//ImGui::Checkbox("Arms", &Settings::ESP::Chams::Arms::enabled);

			//ImGui::Combo("style##ARMTYPES", (int*) &Settings::ESP::Chams::Arms::type , ArmsTypes , IM_ARRAYSIZE (ArmsTypes));

			//ImGui::Checkbox("Weapons##WEAPONCHAMS", &Settings::ESP::Chams::Weapon::enabled);
			ImGui::Separator();

			ImGui::Text("World");
			if (ImGui::Checkbox("Weapons##WEAPONESP", &Settings::ESP::droppedWeapons))
				ImGui::SetTooltip("Show weapons on floor");
			ImGui::Combo("Dropped Weapon Draw Type", (int*)&Settings::ESP::droppedWeaponsType, WeaponTypes, IM_ARRAYSIZE(WeaponTypes));
			//if(ImGui::Checkbox("Throwables", &Settings::ESP::Filters::throwables))
			//ImGui::SetTooltip("Show throwables");
			//if(ImGui::Checkbox("Entity Glow", &Settings::ESP::Glow::enabled))
			//ImGui::SetTooltip("Show glow around entities");
			if (ImGui::Checkbox("Bomb", &Settings::ESP::plantedC4))
				ImGui::SetTooltip("Show bomb when planted");
			if (ImGui::Checkbox("Dropped Bomb", &Settings::ESP::c4))
				ImGui::SetTooltip("Show bomb when dropped");
			if (ImGui::Checkbox("Defuse Kits", &Settings::ESP::defuseKit))
				ImGui::SetTooltip("Show defuse kits on floor");
			if (ImGui::Checkbox("Hostages", &Settings::ESP::hostage))
				ImGui::SetTooltip("Show hostages");
			if (ImGui::Checkbox("Nades", &Settings::ESP::nades))
				ImGui::SetTooltip("Show nades");
			ImGui::Separator();

			ImGui::Text("Misc");
			//ImGui::Checkbox("Night Mode", &Settings::NightMode::enabled);
			ImGui::Checkbox("No Smoke", &Settings::NoSmoke::enabled);
			ImGui::Checkbox("FoV Changer", &Settings::FOVChanger::enabled);
			ImGui::Checkbox("Disable on scope", &Settings::FOVChanger::ignoreScope);
			ImGui::SliderFloat("##VALUEFOV", &Settings::FOVChanger::value, 0, 360, "FOV %0.1f");
			ImGui::Checkbox("ViewModel FoV Changer", &Settings::FOVChanger::viewmodelEnabled);
			ImGui::SliderFloat("##VALUEVIEWMODELFOV", &Settings::FOVChanger::viewmodelValue, 0, 360, "VIEWMODEL FOV %0.1f");
			ImGui::Checkbox("No Scope Border", &Settings::NoScopeBorder::enabled);
			/*if(ImGui::Checkbox("Dlights", &Settings::Dlights::enabled))
			ImGui::SetTooltip("Adds a light source to players");
			ImGui::SliderFloat("##DLIGHTRADIUS", &Settings::Dlights::radius, 0, 1000, "Radius: %0.f");
			if(ImGui::Checkbox("No Flash", &Settings::Noflash::enabled))
			ImGui::SetTooltip("Hide flashbang effect");
			ImGui::SliderFloat("##NOFLASHAMOUNT", &Settings::Noflash::value, 0, 255, "Amount: %0.f");
			if(ImGui::Checkbox("Show Footsteps", &Settings::ESP::Sounds::enabled))
			ImGui::SetTooltip("Shows you footsteps in 3D space");
			ImGui::SliderInt("##SOUNDSTIME", &Settings::ESP::Sounds::time, 250, 5000, "Timeout: %0.f");
			if (ImGui::Checkbox("No Sky", &Settings::NoSky::enabled))
			ImGui::SetTooltip("Allows for the skybox to be colored or disabled");
			if (ImGui::Checkbox("No Smoke", &Settings::NoSmoke::enabled))
			ImGui::SetTooltip("Disables smoke rendering");
			if (ImGui::Checkbox("No Aim Punch", &Settings::View::NoAimPunch::enabled))
			ImGui::SetTooltip("Disables aim punch when shooting");
			if (ImGui::Checkbox("ASUS Walls", &Settings::ASUSWalls::enabled))
			ImGui::SetTooltip("Makes wall textures transparent");
			if (ImGui::Checkbox("No Scope Border", &Settings::NoScopeBorder::enabled))
			ImGui::SetTooltip("Disables black scope silhouette");
			ImGui::Separator();*/

			/*ImGui::Text("Radar");
			ImGui::Checkbox("Active##RADARON", &Settings::Radar::enabled);
			ImGui::SliderFloat("##RADARZOOM", &Settings::Radar::zoom, 0.f, 100.f, "Zoom: %0.f");
			ImGui::Combo("##RADARTEAMCOLTYPE", (int*)& Settings::Radar::teamColorType, TeamColorTypes, IM_ARRAYSIZE(TeamColorTypes));
			ImGui::SliderFloat("##RADARICONSSCALE", &Settings::Radar::iconsScale, 2, 16, "Icons Scale: %0.1f");
			ImGui::Checkbox("Enemies", &Settings::Radar::enemies);
			ImGui::Checkbox("Bomb", &Settings::Radar::bomb);
			ImGui::Checkbox("Legit", &Settings::Radar::legit);
			ImGui::Checkbox("In-game Radar", &Settings::Radar::InGame::enabled);
			ImGui::Checkbox("Allies", &Settings::Radar::allies);
			ImGui::Checkbox("Defuser", &Settings::Radar::defuser);
			ImGui::Checkbox("Visibility Check", &Settings::Radar::visibilityCheck);
			ImGui::Checkbox("Smoke Check", &Settings::Radar::smokeCheck);*/

			ImGui::EndChild();
		}
	}
	ImGui::Columns(1, NULL, false);
}
