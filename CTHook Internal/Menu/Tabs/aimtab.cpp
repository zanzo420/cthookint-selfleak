//
// Created by apostrophe on 30.10.17.
//

#include "aimtab.h"

void aimtab::RenderTab() {


    const char* targets[] = { "PELVIS", "", "", "HIP", "LOWER SPINE", "MIDDLE SPINE", "UPPER SPINE", "NECK", "HEAD" };
    const char* smoothTypes[] = { "Slow Near End", "Constant Speed", "Fast Near End" };

    ImGui::Columns(2,NULL,false);

    {
        ImGui::BeginChild("Aim1", ImVec2( 0, 0 ), false);
        {
            ImGui::Text("Aimbot");
            ImGui::Separator();

            ImGui::Checkbox("Active##AIMBOT", &Settings::Aimbot::enabled);

            ImGui::Checkbox("Friendlyfire", &Settings::Aimbot::friendly);

            ImGui::Checkbox("RCS" , &Settings::Aimbot::rcs);
            /*if ( ImGui::Button( ( "RCS Settings" ), ImVec2( 0, 0 ) ) )
                ImGui::OpenPopup( ( "optionRCSAmount" ) );
            ImGui::SetNextWindowSize( ImVec2( 200, 100 ), ImGuiSetCond_Always );
            if ( ImGui::BeginPopup( ( "optionRCSAmount" ) ) )
            {
                ImGui::PushItemWidth( -1 );
                ImGui::Checkbox( ( "RCS Always on" ), &rcsAlwaysOn );
                ImGui::SliderFloat( ( "##RCSX" ), &rcsAmountX, 0, 2, ( "X: %0.3f" ) );
                ImGui::SliderFloat( ( "##RCSY" ), &rcsAmountY, 0, 2, ( "Y: %0.3f" ) );


                ImGui::PopItemWidth();

                ImGui::EndPopup();
            }*/

            ImGui::Separator();
            ImGui::Text("AutoShoot");
            ImGui::Checkbox("Active##AUTOSHOOT", &Settings::Aimbot::autoShoot );
            ImGui::Checkbox("Autoscope" , &Settings::Aimbot::autoScope);

			ImGui::Separator();
			ImGui::Text("HitChance");
			ImGui::Checkbox("Active##HITCHANCE", &Settings::Aimbot::HitChance::enabled);
			ImGui::SliderFloat(("##AUTOSNIPER"), &Settings::Aimbot::HitChance::value::autoSniper, 0, 1, ("AUTOSNIPER %0.3f"));
			ImGui::SliderFloat(("##AWP"), &Settings::Aimbot::HitChance::value::awp, 0, 1, ("AWP %0.3f"));
			ImGui::SliderFloat(("##SCOUT"), &Settings::Aimbot::HitChance::value::scout, 0, 1, ("SCOUT %0.3f"));
			ImGui::SliderFloat(("##R8"), &Settings::Aimbot::HitChance::value::r8, 0, 1, ("R8/DEAGLE %0.3f"));
			ImGui::SliderFloat(("##OTHER"), &Settings::Aimbot::HitChance::value::other, 0, 1, ("OTHER %0.3f"));
			ImGui::SliderInt(("##HITCHANCERAYS"), &Settings::Aimbot::HitChance::hitRays, 1, 250, ("HIT RAYS %0.f"));

            ImGui::EndChild();

        }

    }
    ImGui::NextColumn();
    {
        ImGui::BeginChild( "Aim2", ImVec2( 0, 0 ), false );
        {

            ImGui::Text("Enhancements");
            ImGui::Separator();
            ImGui::Checkbox("Auto Cock Revolver" , &Settings::Aimbot::autoRevolver);
            //ImGui::Checkbox("Prediction" ,&Settings::Aimbot::Prediction::enabled);
            ImGui::Checkbox("Auto Pistol" , &Settings::Aimbot::autoPistol);
            ImGui::Checkbox("Silent Aim" , &Settings::Aimbot::silent);
			ImGui::Checkbox("Multipoint", &Settings::Aimbot::multipoint);
			ImGui::SliderFloat(("##POINTSCALE"), &Settings::Aimbot::pointscale, 0, 1, ("POINTSCALE %0.2f"));
			ImGui::SliderFloat(("##POINTSCALEBAIM"), &Settings::Aimbot::pointscaleBaim, 0, 1, ("POINTSCALE BAIM %0.2f"));
            ImGui::Separator();
            ImGui::SliderFloat(("##AUTOWALLDMG"), &Settings::Aimbot::mindmg , 0 ,100, ("MIN DMG %0.1f"));

            if ( ImGui::Button( "Hitboxes", ImVec2( -1, 0 ) ) )
                ImGui::OpenPopup( "optionHitboxes" );
            ImGui::SetNextWindowSize( ImVec2( ( ImGui::GetWindowWidth() / 1.25f ), ImGui::GetWindowHeight() ),
                                      ImGuiSetCond_Always );
            if ( ImGui::BeginPopup( "optionHitboxes" ) ) {
                ImGui::PushItemWidth( -1 );
                ImGui::Columns(2, NULL, false);
                {

					ImGui::Checkbox("Head", &Settings::Aimbot::hitboxes[0]);
					ImGui::Checkbox("Neck", &Settings::Aimbot::hitboxes[1]);
					ImGui::Checkbox("Pelvis", &Settings::Aimbot::hitboxes[2]);
					ImGui::Checkbox("Belly", &Settings::Aimbot::hitboxes[3]);
					ImGui::Checkbox("Thorax", &Settings::Aimbot::hitboxes[4]);
                }
                ImGui::NextColumn();
                {
					ImGui::Checkbox("Lower chest", &Settings::Aimbot::hitboxes[5]);
					ImGui::Checkbox("Upper chest", &Settings::Aimbot::hitboxes[6]);
					ImGui::Checkbox("Left thigh", &Settings::Aimbot::hitboxes[7]);
					ImGui::Checkbox("Right thigh", &Settings::Aimbot::hitboxes[8]);
					ImGui::Checkbox("Left upper arm", &Settings::Aimbot::hitboxes[9]);
					ImGui::Checkbox("Right upper arm", &Settings::Aimbot::hitboxes[10]);
                }
                ImGui::PopItemWidth();
                ImGui::EndPopup();
            }
			ImGui::Separator();
			ImGui::Checkbox("Backtrack", &Settings::Aimbot::backtrack);
			ImGui::Checkbox("Forwardtrack", &Settings::Aimbot::forwardtrack);
            ImGui::EndChild();

			//ImGui::Separator();
			//ImGui::Text("BackTrack");
			//ImGui::Checkbox("Enabled", &Settings::Aimbot::backtrack);
			//ImGui::Checkbox("Hitscan", &Settings::Aimbot::backtrackawall);

			/*if (ImGui::Button("Backtrack Bones", ImVec2(-1, 0)))
				ImGui::OpenPopup("optionBonesBt");
			ImGui::SetNextWindowSize(ImVec2((ImGui::GetWindowWidth() / 1.25f), ImGui::GetWindowHeight()),
				ImGuiSetCond_Always);
			if (ImGui::BeginPopup("optionBonesBt")) {
				ImGui::PushItemWidth(-1);
				ImGui::Columns(2, NULL, false);
				{

					(ImGui::Checkbox("Head", &Settings::Aimbot::bones[(int)Hitbox::HITBOX_HEAD]));
					(ImGui::Checkbox("Neck", &Settings::Aimbot::bones[(int)Hitbox::HITBOX_NECK]));
					(ImGui::Checkbox("Pelvis", &Settings::Aimbot::bones[(int)Hitbox::HITBOX_PELVIS]));
				}
				ImGui::NextColumn();
				{
					(ImGui::Checkbox("Spine", &Settings::Aimbot::bones[(int)Hitbox::HITBOX_SPINE]));
					(ImGui::Checkbox("Legs", &Settings::Aimbot::bones[(int)Hitbox::HITBOX_LEGS]));
					(ImGui::Checkbox("Arms", &Settings::Aimbot::bones[(int)Hitbox::HITBOX_ARMS]));
				}
				ImGui::PopItemWidth();
				ImGui::EndPopup();
			}
			ImGui::EndChild();*/
        }
    }


    ImGui::Columns(1,NULL,false);
}
