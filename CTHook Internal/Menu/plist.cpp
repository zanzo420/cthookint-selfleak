#include "plist.h"

bool pList::showWindow = false;

void pList::RenderWindow() {

	const char* yTypes[] = {
		"OFF", "BACKWARDS", "LBY", "FREESTANDING"
	};

	const char* xTypes[] = {
		"OFF", "DOWN", "UP", "ZERO"
	};

	if (!pList::showWindow)
		return;

	static bool setWidth = false;

	ImGui::SetNextWindowSize(ImVec2(500, 330), ImGuiSetCond_FirstUseEver);
	if (ImGui::Begin("Player list", &pList::showWindow,
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar)) {

		static int currentPlayer = -1;

		if (!pEngine->IsInGame() || (*csPlayerResource && !(*csPlayerResource)->GetConnected(currentPlayer)))
			currentPlayer = -1;

		ImGui::ListBoxHeader("##PLAYERS", ImVec2(-1, (ImGui::GetWindowSize().y - 95)));

		ImGui::Columns(4);

		if (!setWidth)
			ImGui::SetColumnWidth(0, 35);
		ImGui::Text(("ID"));
		ImGui::NextColumn();

		if (!setWidth)
			ImGui::SetColumnWidth(1, 200);
		ImGui::Text(("Name"));
		ImGui::NextColumn();

		if (!setWidth)
			ImGui::SetColumnWidth(2, 150);
		ImGui::Text(("Team"));
		ImGui::NextColumn();

		if (!setWidth)
			ImGui::SetColumnWidth(3, 200);
		ImGui::Text(("SteamID"));
		ImGui::NextColumn();

		if (!setWidth)
			setWidth = true;

		//ImGui::Text(("Stats"));
		//ImGui::NextColumn();

		std::unordered_map<TeamID, std::vector<int>, Util::IntHash<TeamID>> players = {
			{ TeamID::TEAM_UNASSIGNED,{} },
		{ TeamID::TEAM_SPECTATOR,{} },
		{ TeamID::TEAM_TERRORIST,{} },
		{ TeamID::TEAM_COUNTER_TERRORIST,{} },
		};

		for (int i = 1; i < pEngine->GetMaxClients(); i++) {
			if (i == pEngine->GetLocalPlayer())
				continue;

			if (!(*csPlayerResource)->GetConnected(i))
				continue;

			players[(*csPlayerResource)->GetTeam(i)].push_back(i);
		}

		for (int team = (int)TeamID::TEAM_UNASSIGNED; team <= (int)TeamID::TEAM_COUNTER_TERRORIST; team++) {
			char* teamName = strdup("");
			switch ((TeamID)team) {
			case TeamID::TEAM_UNASSIGNED:
				teamName = strdup("Unassigned");
				break;
			case TeamID::TEAM_SPECTATOR:
				teamName = strdup("Spectator");
				break;
			case TeamID::TEAM_TERRORIST:
				teamName = strdup("Terrorist");
				break;
			case TeamID::TEAM_COUNTER_TERRORIST:
				teamName = strdup("Counter Terrorist");
				break;
			}

			for (auto it : players[(TeamID)team]) {
				std::string id = std::to_string(it);

				IEngineClient::player_info_t entityInformation;
				pEngine->GetPlayerInfo(it, &entityInformation);

				//if (entityInformation.ishltv)
				//	continue;

				ImGui::Separator();

				if (ImGui::Selectable(id.c_str(), it == currentPlayer, ImGuiSelectableFlags_SpanAllColumns))
					currentPlayer = it;
				//ImGui::NextColumn();

				//ImGui::Text("%i", it);
				ImGui::NextColumn();

				ImGui::Text("%s", entityInformation.name);
				ImGui::NextColumn();

				ImGui::Text("%s", teamName);
				ImGui::NextColumn();

				ImGui::Text("%s", entityInformation.guid);
				ImGui::NextColumn();
				
			}
		}

		ImGui::ListBoxFooter();

		if (currentPlayer != -1) {
			ImGui::Columns(2);
			{
				ImGui::Combo("Pitch Correction", &Resolver::pitchCorrection[currentPlayer], xTypes, IM_ARRAYSIZE(xTypes));
				ImGui::Combo("Yaw Correction", &Resolver::yawCorrection[currentPlayer], yTypes, IM_ARRAYSIZE(yTypes));
			}
			ImGui::NextColumn();
			{
				ImGui::SliderFloat(("##PITCHCORRECTIONADD"), &Resolver::pitchCorrectionAdd[currentPlayer], -90, 90, ("ADD %0.1f"));
				ImGui::SliderFloat(("##YAWCORRECTIONADD"), &Resolver::pitchCorrectionAdd[currentPlayer], -180, 180, ("ADD %0.1f"));
			}

			//TODO:
			//static bool friendly; // add friend sys
			//ImGui::Checkbox("Friend", &friendly);

			//static bool mustkill; // add prioritization
			//ImGui::Checkbox("Priority", &mustkill);

			//static bool gayaim; // add that
			//ImGui::Checkbox("BodyAim", &gayaim);
		}


		ImGui::End();
	}
}