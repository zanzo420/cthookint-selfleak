//
// Created by apostrophe on 30.10.17.
//

#include "gui.h"

ColorVar Settings::UI::mainColor = ImColor(50, 50, 50);
ColorVar Settings::UI::bodyColor = ImColor(0, 0, 0);
ColorVar Settings::UI::fontColor = ImColor(255, 255, 0);

bool Main::showWindow = true;
void gui::RenderWindow(){

	UI::SetupColors();
    static int page = 0;
    ImGui::SetNextWindowSize( ImVec2( 800, 400 ), ImGuiSetCond_FirstUseEver );
    if ( ImGui::Begin( ( "cth00k re::c0de" ), &Main::showWindow,
                       ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar ) ) {

        const char* tabs[] = {
                "Aimbot",
                "Visuals",
                "HvH",
                "Misc"
        };


        ImGui::BeginChild( "Main1", ImVec2( 0, 30 ), true, ImGuiWindowFlags_NoResize );
        {
            for ( int i = 0; i < IM_ARRAYSIZE( tabs ); i++ ) {
                int distance = i == page ? 0 : i > page ? i - page : page - i;

                ImGui::GetStyle().Colors[ImGuiCol_Button] = ImVec4(
                        Settings::UI::mainColor.Color().Value.x - ( distance * 0.035f ),
                        Settings::UI::mainColor.Color().Value.y - ( distance * 0.035f ),
                        Settings::UI::mainColor.Color().Value.z - ( distance * 0.035f ),
                        Settings::UI::mainColor.Color().Value.w
                );

                ImGui::GetStyle().Colors[ImGuiCol_Button] = Settings::UI::mainColor.Color();
                ImGui::SameLine();
                if ( ImGui::Button( tabs[i], ImVec2( ImGui::GetWindowSize().x / IM_ARRAYSIZE( tabs ) - 9,
                                                     ImGui::GetWindowSize().y - 9  ) ) )
                    page = i;


                i < IM_ARRAYSIZE( tabs ) - 1;

            }
            ImGui::EndChild();
        }


        ImGui::BeginChild( "Main2", ImVec2( 0, 0 ), true, ImGuiWindowFlags_NoResize );
        {


            switch ( page ) {
                case 0:
                    aimtab::RenderTab();
                    break;
                case 1:
                    visualtab::RenderTab();
                    break;
                case 2:
                    hvhtab::RenderTab();
                    break;
                case 3:
                    misctab::RenderTab();
                    break;
            }


            ImGui::EndChild();
        }

        ImGui::End();
    }


}
