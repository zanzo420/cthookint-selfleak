//
// Created by apostrophe on 01.11.17.
//

#include "hvhtab.h"

void hvhtab::RenderTab() {

    const char* yTypes[] = {
            "BACKWARDS", "LBY", "FREESTANDING", "RANDOM SPIN", "MASTERLOOSER", "AUTISM"
    };

    const char* xTypes[] = {
            "DOWN", "FAKE UP"
    };

	const char* lbyTypes[] = {
		"REAL", "FAKE", "BACKWARDS"
	};

	const char* fakeLagTypes[] = {
			"LUNICO", "KMETH", "REACTIVE"
	};

    ImGui::Columns(2 , NULL, false);
    {
        ImGui::BeginChild("hvh1");
        {
            ImGui::Text("Yaw-AntiAim");
            ImGui::Separator();
            ImGui::Checkbox("Active##YAW", &Settings::AntiAim::Yaw::enabled);
            ImGui::Combo("FakeYaw", (int*)&Settings::AntiAim::Yaw::fake, yTypes , IM_ARRAYSIZE (yTypes) );
			ImGui::SliderFloat(("##FAKEADD"), &Settings::AntiAim::Yaw::fakeAdd, -180, 180, ("FAKE ADD %0.1f"));
            ImGui::Combo("RealYaw", (int*)&Settings::AntiAim::Yaw::real, yTypes , IM_ARRAYSIZE (yTypes) );
			ImGui::SliderFloat(("##REALADD"), &Settings::AntiAim::Yaw::realAdd, -180, 180, ("REAL ADD %0.1f"));

			if (Settings::AntiAim::Yaw::real == AntiAimYType::RANDOMSPIN || Settings::AntiAim::Yaw::fake == AntiAimYType::RANDOMSPIN) {
				ImGui::Separator();
				ImGui::SliderFloat("Spin Speed", (float*)&Settings::AntiAim::Yaw::spinSpeed, 0.1f, 100.0f);
			}

            ImGui::Separator();
            ImGui::Text("Pitch");
            ImGui::Checkbox("Active##PITCH", &Settings::AntiAim::Pitch::enabled);
            ImGui::Combo("type", (int*)&Settings::AntiAim::Pitch::pitch, xTypes, IM_ARRAYSIZE(xTypes));

			ImGui::Separator();
			ImGui::Text("Roll");
			ImGui::Checkbox("Active##ROLL", &Settings::AntiAim::Roll::enabled);
			ImGui::SliderFloat(("##ROLL"), &Settings::AntiAim::Roll::angle, -180, 180, ("ROLL %0.1f"));

			ImGui::Separator();
			ImGui::Text("LBY");
			ImGui::Checkbox("antiResolver", &Settings::AntiAim::Yaw::antiResolver);
			if (ImGui::IsItemActive() || ImGui::IsItemHovered())
				ImGui::SetTooltip("LBY Breaker, breaks ayyware resolvers");
			ImGui::Combo("Type", (int*)&Settings::AntiAim::LBY::type, lbyTypes, IM_ARRAYSIZE(lbyTypes));
			ImGui::SliderFloat(("##LBYADD"), &Settings::AntiAim::LBY::add, -180, 180, ("LBY ADD %0.1f"));
			ImGui::Checkbox("PreBreak Change", &Settings::AntiAim::LBY::preBreak);
			if (ImGui::IsItemActive() || ImGui::IsItemHovered())
				ImGui::SetTooltip("Changes yaw 1 tick before lby update, breaks resolvers");
			ImGui::Combo("PreBreak Type", (int*)&Settings::AntiAim::LBY::preBreakType, lbyTypes, IM_ARRAYSIZE(lbyTypes));
			ImGui::SliderFloat(("##LBYPREBREAKADD"), &Settings::AntiAim::LBY::preBreakAdd, -180, 180, ("LBY PREBREAK ADD %0.1f"));
			ImGui::Checkbox("Anti LastMove", &Settings::AntiAim::AntiLastMove::enabled);
			if (ImGui::IsItemActive() || ImGui::IsItemHovered())
				ImGui::SetTooltip("Changes yaw before stopped moving, breaks resolvers");
			ImGui::Combo("Anti LastMove Type", (int*)&Settings::AntiAim::AntiLastMove::type, lbyTypes, IM_ARRAYSIZE(lbyTypes));
			ImGui::SliderFloat(("##LASTMOVEADD"), &Settings::AntiAim::AntiLastMove::add, -180, 180, ("ANTI LASTMOVE ADD %0.1f"));

           /* ImGui::Separator();
            ImGui::Text("Head-Edge");
            ImGui::Checkbox("Active##HEADEDGE", &Settings::AntiAim::HeadEdge::enabled);
            ImGui::SliderFloat("value", (float*) &Settings::AntiAim::HeadEdge::distance, 20, 30 );*/

            /*ImGui::Separator();
            ImGui::Text("Disable AA");
            ImGui::Checkbox("on Knife", &Settings::AntiAim::AutoDisable::knifeHeld);
            ImGui::Checkbox("without Enemy", &Settings::AntiAim::AutoDisable::noEnemy);*/
            ImGui::EndChild();
        }
    }
    ImGui::NextColumn();
    {
        ImGui::BeginChild("hvh2");
        {
			ImGui::Text("Anti-UT");
			ImGui::Checkbox("Active##ANTIUT", &Settings::AntiAim::antiUt);
			ImGui::Separator();
			ImGui::Text("Resolver");
			{
				ImGui::Separator();
				ImGui::Checkbox("Active##RESOLVER", &Settings::Resolver::enabled);
				ImGui::SliderInt(("##BAIMAFTERXSHOTS"), &Settings::Resolver::baimAfterXShots, 0, 15, ("BAIM AFTER %0.f SHOTS"));
				ImGui::Columns(2, NULL, true);
				{
					//ImGui::Checkbox(XORSTR("Fakewalk - AIMWARE"), &Settings::FakewalkAW::enabled);
					ImGui::Checkbox("Override", &Settings::Resolver::override);
					//ImGui::Checkbox(XORSTR("Choke Packets"), &Settings::ChokePackets::enabled);
					//ImGui::Checkbox(XORSTR("Circle Strafer"), &Settings::CircleStrafe::enabled);
				}
				ImGui::NextColumn();
				{
					//UI::KeyBindButton(&Settings::FakewalkAW::key);
					UI::KeyBindButton(&Settings::Resolver::overrideKey);
					//UI::KeyBindButton(&Settings::ChokePackets::key);
					//UI::KeyBindButton(&Settings::CircleStrafe::key);
				}
				ImGui::Columns(1);
			}
            /*ImGui::Checkbox("Backtrack", &Settings::Resolver::backtrack);
            ImGui::Checkbox("BaimLby", &Settings::Resolver::baimLby);
            ImGui::Checkbox("Lbyonly", &Settings::Resolver::shootLbyOnly);*/

			ImGui::Separator();
			ImGui::Text("Movement");
			{
				ImGui::Separator();

				ImGui::Columns(2, NULL, true);
				{
					//ImGui::Checkbox(XORSTR("Fakewalk - AIMWARE"), &Settings::FakewalkAW::enabled);
					ImGui::Checkbox("Fakewalk", &Settings::FakeWalk::enabled);
					//ImGui::Checkbox(XORSTR("Choke Packets"), &Settings::ChokePackets::enabled);
					//ImGui::Checkbox(XORSTR("Circle Strafer"), &Settings::CircleStrafe::enabled);
				}
				ImGui::NextColumn();
				{
					//UI::KeyBindButton(&Settings::FakewalkAW::key);
					UI::KeyBindButton(&Settings::FakeWalk::key);
					//UI::KeyBindButton(&Settings::ChokePackets::key);
					//UI::KeyBindButton(&Settings::CircleStrafe::key);
				}
				ImGui::Columns(1);
			}
			ImGui::Separator();
			ImGui::Text(("Fake Lag"));
			ImGui::Checkbox("Active##FAKELAG", &Settings::FakeLag::enabled);
			ImGui::Combo("Type", (int*)&Settings::FakeLag::type, fakeLagTypes, IM_ARRAYSIZE(fakeLagTypes));
			if (Settings::FakeLag::type == FakeLagType::KMETH)
				ImGui::SliderInt(("##FAKELAGAMOUNT"), &Settings::FakeLag::value, 0, 16, ("Amount: %0.f"));
			ImGui::Separator();
			ImGui::Text("Info");
			ImGui::Checkbox("LBY Broken", &Settings::Info::lbyBroken);
			ImGui::Checkbox("LC Broken", &Settings::Info::lcBroken);
			ImGui::EndChild();
            // ... TODO more stuff here
        }
    }
    ImGui::Columns(1, NULL , false);
}
