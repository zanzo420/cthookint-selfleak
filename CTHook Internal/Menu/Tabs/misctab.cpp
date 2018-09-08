//
// Created by apostrophe on 01.11.17.
//

#include "misctab.h"

void misctab::RenderTab() {
    const char* clanTagTypes[] = {
			"CTHook", "CTHook Animated"
	};

    ImGui::Columns(2 , NULL, false);
    {
        ImGui::BeginChild("misc1");
        {
            ImGui::Text("Movement");
            ImGui::Separator();
            ImGui::Checkbox("Bunnyhop", &Settings::BHop::enabled);
            ImGui::Checkbox("Autostrafer", &Settings::BHop::autoStrafe);
            //ImGui::Checkbox("Strafe silent", &Settings::AutoStrafe::silent);
            /*ImGui::Separator();
            ImGui::Text("Spammer");
            ImGui::Checkbox("onKill", &Settings::Spammer::KillSpammer::enabled);
            ImGui::SameLine();
            ImGui::Checkbox("teamChatOnly", &Settings::Spammer::KillSpammer::sayTeam);
            if (ImGui::Button(("Options###KILL")))
                ImGui::OpenPopup(("options_kill"));

            ImGui::SetNextWindowSize(ImVec2(565, 268), ImGuiSetCond_Always);
            if (ImGui::BeginPopup(("options_kill"))) {
                static int killSpammerMessageCurrent = -1;
                static char killSpammerMessageBuf[126];

                ImGui::PushItemWidth(445);
                ImGui::InputText(("###SPAMMERMESSAGE"), killSpammerMessageBuf,
                                 IM_ARRAYSIZE(killSpammerMessageBuf));
                ImGui::PopItemWidth();
                ImGui::SameLine();

                if (ImGui::Button(("Add"))) {
                    if (strlen(killSpammerMessageBuf) > 0)
                        Settings::Spammer::KillSpammer::messages.push_back(std::string(killSpammerMessageBuf));

                    strcpy(killSpammerMessageBuf, "");
                }
                ImGui::SameLine();

                if (ImGui::Button(("Remove")))
                    if (killSpammerMessageCurrent > -1 &&
                        (int) Settings::Spammer::KillSpammer::messages.size() > killSpammerMessageCurrent)
                        Settings::Spammer::KillSpammer::messages.erase(
                                Settings::Spammer::KillSpammer::messages.begin() + killSpammerMessageCurrent);

                ImGui::PushItemWidth(550);
                //ImGui::ListBox("", &killSpammerMessageCurrent, Settings::Spammer::KillSpammer::messages, 10);
                ImGui::PopItemWidth();

                ImGui::EndPopup();
            }

            ImGui::Checkbox("Radio Commands", &Settings::Spammer::RadioSpammer::enabled);

            ImGui::Combo(("###SPAMMERYPE"), (int *) &Settings::Spammer::type, spammerTypes,
                         IM_ARRAYSIZE(spammerTypes));

            ImGui::Checkbox(("Team Chat only###SAY_TEAM2"), &Settings::Spammer::say_team);

            if (Settings::Spammer::type != SpammerType::SPAMMER_NONE &&
                ImGui::Button(("Options###SPAMMER")))
                ImGui::OpenPopup(("options_spammer"));

            if (Settings::Spammer::type == SpammerType::SPAMMER_NORMAL)
                ImGui::SetNextWindowSize(ImVec2(565, 268), ImGuiSetCond_Always);
            else if (Settings::Spammer::type == SpammerType::SPAMMER_POSITIONS)
                ImGui::SetNextWindowSize(ImVec2(200, 240), ImGuiSetCond_Always);

            if (Settings::Spammer::type != SpammerType::SPAMMER_NONE &&
                ImGui::BeginPopup(("options_spammer"))) {
                if (Settings::Spammer::type == SpammerType::SPAMMER_NORMAL) {
                    static int spammerMessageCurrent = -1;
                    static char spammerMessageBuf[126];

                    ImGui::PushItemWidth(445);
                    ImGui::InputText(("###SPAMMERMESSAGE"), spammerMessageBuf,
                                     IM_ARRAYSIZE(spammerMessageBuf));
                    ImGui::PopItemWidth();
                    ImGui::SameLine();

                    if (ImGui::Button(("Add"))) {
                        if (strlen(spammerMessageBuf) > 0)
                            Settings::Spammer::NormalSpammer::messages.push_back(
                                    std::string(spammerMessageBuf));

                        strcpy(spammerMessageBuf, "");
                    }
                    ImGui::SameLine();

                    if (ImGui::Button(("Remove")))
                        if (spammerMessageCurrent > -1 &&
                            (int) Settings::Spammer::NormalSpammer::messages.size() > spammerMessageCurrent)
                            Settings::Spammer::NormalSpammer::messages.erase(
                                    Settings::Spammer::NormalSpammer::messages.begin() + spammerMessageCurrent);

                    ImGui::PushItemWidth(550);
                    //ImGui::ListBox("", &spammerMessageCurrent, Settings::Spammer::NormalSpammer::messages, 10);
                    ImGui::PopItemWidth();
                } else if (Settings::Spammer::type == SpammerType::SPAMMER_POSITIONS) {
                    ImGui::PushItemWidth(185);
                    ImGui::Combo(("###POSITIONSTEAM"), (int*) &Settings::Spammer::PositionSpammer::team, teams,
                                 IM_ARRAYSIZE(teams));
                    ImGui::PopItemWidth();
                    ImGui::Separator();
                    ImGui::Checkbox(("Show Name"), &Settings::Spammer::PositionSpammer::showName);
                    ImGui::Checkbox(("Show Weapon"), &Settings::Spammer::PositionSpammer::showWeapon);
                    ImGui::Checkbox(("Show Rank"), &Settings::Spammer::PositionSpammer::showRank);
                    ImGui::Checkbox(("Show Wins"), &Settings::Spammer::PositionSpammer::showWins);
                    ImGui::Checkbox(("Show Health"), &Settings::Spammer::PositionSpammer::showHealth);
                    ImGui::Checkbox(("Show Money"), &Settings::Spammer::PositionSpammer::showMoney);
                    ImGui::Checkbox(("Show Last Place"),
                                    &Settings::Spammer::PositionSpammer::showLastplace);
                }

                ImGui::EndPopup();
                }*/
                ImGui::Separator();
                /*ImGui::Text("FOV");
                ImGui::Checkbox(("Active##FOVCHANGER"), &Settings::FOVChanger::enabled);
                ImGui::SliderFloat(("##FOVAMOUNT"), &Settings::FOVChanger::value, 0, 180);
                ImGui::Checkbox(("Viewmodel FOV"), &Settings::FOVChanger::viewmodelEnabled);
                ImGui::SliderFloat(("##MODELFOVAMOUNT"), &Settings::FOVChanger::viewmodelValue, 0, 360);
                ImGui::Checkbox(("Ignore Scope"), &Settings::FOVChanger::ignoreScope);*/

                ImGui::Checkbox("Third Person", &Settings::ThirdPerson::enabled);
                ImGui::SliderFloat(("##TPCAMOFFSET"), &Settings::ThirdPerson::distance, 0.0f, 500.0f, ("DISTANCE %0.2f"));
                ImGui::Checkbox("Show Real Angles", &Settings::ThirdPerson::realAngles);
				ImGui::Checkbox("Fake Angle Chams", &Settings::ThirdPerson::fakeChams);
                ImGui::EndChild();
            }
        }
        ImGui::NextColumn();
        {
            ImGui::BeginChild("misc2");
            {
                ImGui::Text( ( "Clantag" ) );
                ImGui::Separator();
                ImGui::Checkbox( ( "Enabled" ), &Settings::ClanTag::enabled );
				ImGui::Combo("type", (int*)&Settings::ClanTag::type, clanTagTypes, IM_ARRAYSIZE(clanTagTypes));
                /*if ( ImGui::Combo( ( "##ANIMATIONTYPE" ), ( int* ) &Settings::ClanTagChanger::type,
                                   animationTypes, IM_ARRAYSIZE( animationTypes ) ) )
                    ClanTagChanger::UpdateClanTagCallback();
                if ( ImGui::SliderInt( ( "##ANIMATIONSPEED" ), &Settings::ClanTagChanger::animationSpeed, 0,
                                       2000 ) )
                    ClanTagChanger::UpdateClanTagCallback();
                if ( ImGui::InputText( ( "##CLANTAG" ), Settings::ClanTagChanger::value, 30 ) ) {
                    if (Settings::ClanTagChanger::value[1] != 0)
                        ClanTagChanger::UpdateClanTagCallback();
                }
					const char* nicknameBuff = NameChanger::origName.c_str();
					static char* nickname = (char*)nicknameBuff;
						
                    ImGui::Separator();
                    ImGui::Text( ( "Nickname" ) );
                    ImGui::InputText( ( "##NICKNAME" ), nickname, 127 );

                    ImGui::SameLine();
                    if ( ImGui::Button( ( "Set Nickname" ), ImVec2( -1, 0 ) ) )
                        NameChanger::SetName( std::string( nickname ).c_str() );

                    if ( ImGui::Button( ( "Glitch Name" ) ) )
                        NameChanger::SetName( "\n\xAD\xAD\xAD" );
                    ImGui::SameLine();
                    if ( ImGui::Button( ( "No Name" ) ) )
                        ImGui::OpenPopup( ( "optionNoName" ) );
                    ImGui::SetNextWindowSize( ImVec2( 150, 100 ), ImGuiCond_Always );
                    /*if ( ImGui::BeginPopup(  ( "optionNoName" ) ) ) {
                        ImGui::PushItemWidth( -1 );
                        for ( auto& it : NameChanger::nntypes ) {
                            if ( ImGui::Button( it.second, ImVec2( -1, 0 ) ) )
                                NameChanger::InitNoName( NameChanger::NC_Type::NC_NORMAL, it.first );

                        }
                        ImGui::PopItemWidth();

                        ImGui::EndPopup();
                    }*/

                    //if ( ImGui::Checkbox( ( "Name Stealer" ), &Settings::NameStealer::enabled ) )
                   //     NameStealer::entityId = -1;
					
				//ImGui::Combo( "", (int*) &Settings::NameStealer::team, teams, IM_ARRAYSIZE( teams ) );
				//ImGui::Separator();*/
				ImGui::Separator();
				ImGui::Checkbox("Auto Accept", &Settings::AutoAccept::enabled);
				ImGui::Separator();
				ImGui::Text("Event Logger");
				ImGui::Checkbox("Enabled", &Settings::UI::EventLogger::enabled);
				ImGui::Text("Filters");
				ImGui::Checkbox("Item Purchase", &Settings::UI::EventLogger::filters[1]);
				ImGui::Checkbox("Objective", &Settings::UI::EventLogger::filters[2]);
				ImGui::Checkbox("Player Hurt", &Settings::UI::EventLogger::filters[3]);
				ImGui::Checkbox("Resolver", &Settings::UI::EventLogger::filters[4]);
				ImGui::Checkbox("Other", &Settings::UI::EventLogger::filters[0]);
				ImGui::SliderFloat(("##ELOGGERTIMEOUT"), &Settings::UI::EventLogger::fadeOut, 1.f, 12.f, ("FADE OUT TIME %0.1f"));

                //TODO finish

                ImGui::EndChild();
            }
        }

    ImGui::Columns(1, NULL , false);
}