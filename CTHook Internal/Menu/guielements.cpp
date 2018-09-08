//
// Created by apostrophe on 30.10.17.
//

#include "gui.h"

const char* ButtonNames[] =
{

	"KEY_NONE",
	"KEY_0",
	"KEY_1",
	"KEY_2",
	"KEY_3",
	"KEY_4",
	"KEY_5",
	"KEY_6",
	"KEY_7",
	"KEY_8",
	"KEY_9",
	"KEY_A",
	"KEY_B",
	"KEY_C",
	"KEY_D",
	"KEY_E",
	"KEY_F",
	"KEY_G",
	"KEY_H",
	"KEY_I",
	"KEY_J",
	"KEY_K",
	"KEY_L",
	"KEY_M",
	"KEY_N",
	"KEY_O",
	"KEY_P",
	"KEY_Q",
	"KEY_R",
	"KEY_S",
	"KEY_T",
	"KEY_U",
	"KEY_V",
	"KEY_W",
	"KEY_X",
	"KEY_Y",
	"KEY_Z",
	"KEY_PAD_0",
	"KEY_PAD_1",
	"KEY_PAD_2",
	"KEY_PAD_3",
	"KEY_PAD_4",
	"KEY_PAD_5",
	"KEY_PAD_6",
	"KEY_PAD_7",
	"KEY_PAD_8",
	"KEY_PAD_9",
	"KEY_PAD_DIVIDE",
	"KEY_PAD_MULTIPLY",
	"KEY_PAD_MINUS",
	"KEY_PAD_PLUS",
	"KEY_PAD_ENTER",
	"KEY_PAD_DECIMAL",
	"KEY_LBRACKET",
	"KEY_RBRACKET",
	"KEY_SEMICOLON",
	"KEY_APOSTROPHE",
	"KEY_BACKQUOTE",
	"KEY_COMMA",
	"KEY_PERIOD",
	"KEY_SLASH",
	"KEY_BACKSLASH",
	"KEY_MINUS",
	"KEY_EQUAL",
	"KEY_ENTER",
	"KEY_SPACE",
	"KEY_BACKSPACE",
	"KEY_TAB",
	"KEY_CAPSLOCK",
	"KEY_NUMLOCK",
	"KEY_ESCAPE",
	"KEY_SCROLLLOCK",
	"KEY_INSERT",
	"KEY_DELETE",
	"KEY_HOME",
	"KEY_END",
	"KEY_PAGEUP",
	"KEY_PAGEDOWN",
	"KEY_BREAK",
	"KEY_LSHIFT",
	"KEY_RSHIFT",
	"KEY_LALT",
	"KEY_RALT",
	"KEY_LCONTROL",
	"KEY_RCONTROL",
	"KEY_LWIN",
	"KEY_RWIN",
	"KEY_APP",
	"KEY_UP",
	"KEY_LEFT",
	"KEY_DOWN",
	"KEY_RIGHT",
	"KEY_F1",
	"KEY_F2",
	"KEY_F3",
	"KEY_F4",
	"KEY_F5",
	"KEY_F6",
	"KEY_F7",
	"KEY_F8",
	"KEY_F9",
	"KEY_F10",
	"KEY_F11",
	"KEY_F12",
	"KEY_CAPSLOCKTOGGLE",
	"KEY_NUMLOCKTOGGLE",
	"KEY_SCROLLLOCKTOGGLE",



	"MOUSE_LEFT",
	"MOUSE_RIGHT",
	"MOUSE_MIDDLE",
	"MOUSE_4",
	"MOUSE_5",
	"MOUSE_WHEEL_UP",		// A fake button which is 'pressed' and 'released' when the wheel is moved up
	"MOUSE_WHEEL_DOWN",	// A fake button which is 'pressed' and 'released' when the wheel is moved down

						// VAXIS NEGATIVE
};

void UI::KeyBindButton( ButtonCode_t* key ) {
    /*const char* text = pInputSystem->ButtonCodeToString( *key );

	// Apo needs to fix this
	if ( SetKeyCodeState::shouldListen && SetKeyCodeState::keyOutput == key )
        text = "-- press a key --";
    else
        text = Util::ToUpper( std::string( text ) ).c_str();

    if ( ImGui::Button( text, ImVec2( -1, 0 ) ) ) {
        SetKeyCodeState::shouldListen = true;
        SetKeyCodeState::keyOutput = key;
    }*/

	//fix until apo fixes
	ImGui::Combo("##bindbuttonkey", (int*)&key, ButtonNames, IM_ARRAYSIZE(ButtonNames));
}

bool UI::ColorPicker( float* col, bool alphabar ) {
    const int EDGE_SIZE = 200; // = int(ImGui::GetWindowWidth() * 0.75f);
    const ImVec2 SV_PICKER_SIZE = ImVec2( EDGE_SIZE, EDGE_SIZE );
    const float SPACING = ImGui::GetStyle().ItemInnerSpacing.x;
    const float HUE_PICKER_WIDTH = 20.f;
    const float CROSSHAIR_SIZE = 7.0f;

    ImColor color( col[0], col[1], col[2] );
    bool value_changed = false;

    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    ImVec2 picker_pos = ImGui::GetCursorScreenPos();

    float hue, saturation, value;
    ImGui::ColorConvertRGBtoHSV( color.Value.x, color.Value.y, color.Value.z, hue, saturation, value );

    ImColor colors[] = {
            ImColor( 255, 0, 0 ),
            ImColor( 255, 255, 0 ),
            ImColor( 0, 255, 0 ),
            ImColor( 0, 255, 255 ),
            ImColor( 0, 0, 255 ),
            ImColor( 255, 0, 255 ),
            ImColor( 255, 0, 0 )
    };

    for ( int i = 0; i < 6; i++ ) {
        draw_list->AddRectFilledMultiColor(
                ImVec2( picker_pos.x + SV_PICKER_SIZE.x + SPACING, picker_pos.y + i * ( SV_PICKER_SIZE.y / 6 ) ),
                ImVec2( picker_pos.x + SV_PICKER_SIZE.x + SPACING + HUE_PICKER_WIDTH,
                        picker_pos.y + ( i + 1 ) * ( SV_PICKER_SIZE.y / 6 ) ),
                colors[i],
                colors[i],
                colors[i + 1],
                colors[i + 1]
        );
    }

    draw_list->AddLine(
            ImVec2( picker_pos.x + SV_PICKER_SIZE.x + SPACING - 2, picker_pos.y + hue * SV_PICKER_SIZE.y ),
            ImVec2( picker_pos.x + SV_PICKER_SIZE.x + SPACING + 2 + HUE_PICKER_WIDTH,
                    picker_pos.y + hue * SV_PICKER_SIZE.y ),
            ImColor( 255, 255, 255 )
    );

    if ( alphabar ) {
        float alpha = col[3];

        draw_list->AddRectFilledMultiColor(
                ImVec2( picker_pos.x + SV_PICKER_SIZE.x + 2 * SPACING + HUE_PICKER_WIDTH, picker_pos.y ),
                ImVec2( picker_pos.x + SV_PICKER_SIZE.x + 2 * SPACING + 2 * HUE_PICKER_WIDTH,
                        picker_pos.y + SV_PICKER_SIZE.y ),
                ImColor( 0, 0, 0 ), ImColor( 0, 0, 0 ), ImColor( 255, 255, 255 ), ImColor( 255, 255, 255 )
        );

        draw_list->AddLine(
                ImVec2( picker_pos.x + SV_PICKER_SIZE.x + 2 * ( SPACING - 2 ) + HUE_PICKER_WIDTH,
                        picker_pos.y + alpha * SV_PICKER_SIZE.y ),
                ImVec2( picker_pos.x + SV_PICKER_SIZE.x + 2 * ( SPACING + 2 ) + 2 * HUE_PICKER_WIDTH,
                        picker_pos.y + alpha * SV_PICKER_SIZE.y ),
                ImColor( 255.f - alpha, 255.f, 255.f )
        );
    }

    const ImU32 c_oColorBlack = ImGui::ColorConvertFloat4ToU32( ImVec4( 0.f, 0.f, 0.f, 1.f ) );
    const ImU32 c_oColorBlackTransparent = ImGui::ColorConvertFloat4ToU32( ImVec4( 0.f, 0.f, 0.f, 0.f ) );
    const ImU32 c_oColorWhite = ImGui::ColorConvertFloat4ToU32( ImVec4( 1.f, 1.f, 1.f, 1.f ) );

    ImVec4 cHueValue( 1, 1, 1, 1 );
    ImGui::ColorConvertHSVtoRGB( hue, 1, 1, cHueValue.x, cHueValue.y, cHueValue.z );
    ImU32 oHueColor = ImGui::ColorConvertFloat4ToU32( cHueValue );

    draw_list->AddRectFilledMultiColor(
            ImVec2( picker_pos.x, picker_pos.y ),
            ImVec2( picker_pos.x + SV_PICKER_SIZE.x, picker_pos.y + SV_PICKER_SIZE.y ),
            c_oColorWhite,
            oHueColor,
            oHueColor,
            c_oColorWhite
    );

    draw_list->AddRectFilledMultiColor(
            ImVec2( picker_pos.x, picker_pos.y ),
            ImVec2( picker_pos.x + SV_PICKER_SIZE.x, picker_pos.y + SV_PICKER_SIZE.y ),
            c_oColorBlackTransparent,
            c_oColorBlackTransparent,
            c_oColorBlack,
            c_oColorBlack
    );

    float x = saturation * SV_PICKER_SIZE.x;
    float y = ( 1 - value ) * SV_PICKER_SIZE.y;
    ImVec2 p( picker_pos.x + x, picker_pos.y + y );
    draw_list->AddLine( ImVec2( p.x - CROSSHAIR_SIZE, p.y ), ImVec2( p.x - 2, p.y ), ImColor( 255, 255, 255 ) );
    draw_list->AddLine( ImVec2( p.x + CROSSHAIR_SIZE, p.y ), ImVec2( p.x + 2, p.y ), ImColor( 255, 255, 255 ) );
    draw_list->AddLine( ImVec2( p.x, p.y + CROSSHAIR_SIZE ), ImVec2( p.x, p.y + 2 ), ImColor( 255, 255, 255 ) );
    draw_list->AddLine( ImVec2( p.x, p.y - CROSSHAIR_SIZE ), ImVec2( p.x, p.y - 2 ), ImColor( 255, 255, 255 ) );

    ImGui::InvisibleButton( "saturation_value_selector", SV_PICKER_SIZE );

    if ( ImGui::IsItemActive() && ImGui::GetIO().MouseDown[0] ) {
        ImVec2 mouse_pos_in_canvas = ImVec2( ImGui::GetIO().MousePos.x - picker_pos.x,
                                             ImGui::GetIO().MousePos.y - picker_pos.y );

        if ( mouse_pos_in_canvas.x < 0 )
            mouse_pos_in_canvas.x = 0;
        else if ( mouse_pos_in_canvas.x >= SV_PICKER_SIZE.x - 1 )
            mouse_pos_in_canvas.x = SV_PICKER_SIZE.x - 1;

        if ( mouse_pos_in_canvas.y < 0 )
            mouse_pos_in_canvas.y = 0;
        else if ( mouse_pos_in_canvas.y >= SV_PICKER_SIZE.y - 1 )
            mouse_pos_in_canvas.y = SV_PICKER_SIZE.y - 1;

        value = 1 - ( mouse_pos_in_canvas.y / ( SV_PICKER_SIZE.y - 1 ) );
        saturation = mouse_pos_in_canvas.x / ( SV_PICKER_SIZE.x - 1 );
        value_changed = true;
    }

    // hue bar logic
    ImGui::SetCursorScreenPos( ImVec2( picker_pos.x + SPACING + SV_PICKER_SIZE.x, picker_pos.y ) );
    ImGui::InvisibleButton( "hue_selector", ImVec2( HUE_PICKER_WIDTH, SV_PICKER_SIZE.y ) );

    if ( ImGui::GetIO().MouseDown[0] && ( ImGui::IsItemHovered() || ImGui::IsItemActive() ) ) {
        ImVec2 mouse_pos_in_canvas = ImVec2( ImGui::GetIO().MousePos.x - picker_pos.x,
                                             ImGui::GetIO().MousePos.y - picker_pos.y );

        if ( mouse_pos_in_canvas.y < 0 )
            mouse_pos_in_canvas.y = 0;
        else if ( mouse_pos_in_canvas.y >= SV_PICKER_SIZE.y - 1 )
            mouse_pos_in_canvas.y = SV_PICKER_SIZE.y - 1;

        hue = mouse_pos_in_canvas.y / ( SV_PICKER_SIZE.y - 1 );
        value_changed = true;
    }

    if ( alphabar ) {
        ImGui::SetCursorScreenPos(
                ImVec2( picker_pos.x + SPACING * 2 + HUE_PICKER_WIDTH + SV_PICKER_SIZE.x, picker_pos.y ) );
        ImGui::InvisibleButton( "alpha_selector", ImVec2( HUE_PICKER_WIDTH, SV_PICKER_SIZE.y ) );

        if ( ImGui::GetIO().MouseDown[0] && ( ImGui::IsItemHovered() || ImGui::IsItemActive() ) ) {
            ImVec2 mouse_pos_in_canvas = ImVec2( ImGui::GetIO().MousePos.x - picker_pos.x,
                                                 ImGui::GetIO().MousePos.y - picker_pos.y );

            if ( mouse_pos_in_canvas.y < 0 )
                mouse_pos_in_canvas.y = 0;
            else if ( mouse_pos_in_canvas.y >= SV_PICKER_SIZE.y - 1 )
                mouse_pos_in_canvas.y = SV_PICKER_SIZE.y - 1;

            float alpha = mouse_pos_in_canvas.y / ( SV_PICKER_SIZE.y - 1 );
            col[3] = alpha;
            value_changed = true;
        }
    }

    color = ImColor::HSV( hue >= 1 ? hue - 10 * 1e-6 : hue, saturation > 0 ? saturation : 10 * 1e-6,
                          value > 0 ? value : 1e-6 );
    col[0] = color.Value.x;
    col[1] = color.Value.y;
    col[2] = color.Value.z;

    bool widget_used;
    ImGui::PushItemWidth(
            ( alphabar ? SPACING + HUE_PICKER_WIDTH : 0 ) + SV_PICKER_SIZE.x + SPACING + HUE_PICKER_WIDTH -
            2 * ImGui::GetStyle().FramePadding.x );
    widget_used = alphabar ? ImGui::ColorEdit4( "", col ) : ImGui::ColorEdit3( "", col );
    ImGui::PopItemWidth();

    float new_hue, new_sat, new_val;
    ImGui::ColorConvertRGBtoHSV( col[0], col[1], col[2], new_hue, new_sat, new_val );

    if ( new_hue <= 0 && hue > 0 ) {
        if ( new_val <= 0 && value != new_val ) {
            color = ImColor::HSV( hue, saturation, new_val <= 0 ? value * 0.5f : new_val );
            col[0] = color.Value.x;
            col[1] = color.Value.y;
            col[2] = color.Value.z;
        } else if ( new_sat <= 0 ) {
            color = ImColor::HSV( hue, new_sat <= 0 ? saturation * 0.5f : new_sat, new_val );
            col[0] = color.Value.x;
            col[1] = color.Value.y;
            col[2] = color.Value.z;
        }
    }

    return value_changed | widget_used;
}

bool UI::ColorPicker3( float col[3] ) {
    return UI::ColorPicker( col, false );
}

bool UI::ColorPicker4( float col[4] ) {
    return UI::ColorPicker( col, true );
}

void UI::SetupColors() {
    ImGuiStyle& style = ImGui::GetStyle();

    style.Alpha = 1.0f;
    style.WindowPadding = ImVec2(8, 8);
    style.WindowRounding = 0.0f;
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
    style.ChildWindowRounding = 3.0f;
    style.FramePadding = ImVec2(4, 3);
    style.FrameRounding = 0.0f;
    style.ItemSpacing = ImVec2(8, 4);
    style.ItemInnerSpacing = ImVec2(4, 4);
    style.TouchExtraPadding = ImVec2(0, 0);
    style.IndentSpacing = 21.0f;
    style.ColumnsMinSpacing = 6.0f;
    style.ScrollbarSize = 10.0f;
    style.ScrollbarRounding = 3.0f;
    style.GrabMinSize = 10.0f;
    style.GrabRounding = 0.0f;
    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
    style.DisplayWindowPadding = ImVec2(22, 22);
    style.DisplaySafeAreaPadding = ImVec2(4, 4);
    style.AntiAliasedLines = true;
    style.AntiAliasedShapes = true;
    style.CurveTessellationTol = 1.25f;


    style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.10f, 0.11f, 1.00f);
    style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.21f, 0.20f, 0.21f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.39f, 0.20f, 0.21f, 1.00f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.90f, 0.20f, 0.21f, 1.00f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.75f, 0.11f, 0.22f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.75f, 0.11f, 0.22f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.75f, 0.11f, 0.22f, 1.00f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.75f, 0.11f, 0.22f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.75f, 0.11f, 0.22f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.51f, 0.11f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.35f, 0.10f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_ComboBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.83f, 0.11f, 0.28f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.75f, 0.11f, 0.22f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.51f, 0.10f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.76f, 0.10f, 0.24f, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.63f, 0.10f, 0.24f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.55f, 0.10f, 0.24f, 1.00f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.75f, 0.11f, 0.22f, 1.00f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.51f, 0.11f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.40f, 0.11f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_Column] = ImVec4(0.75f, 0.10f, 0.24f, 1.00f);
    style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.51f, 0.11f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.40f, 0.11f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
    style.Colors[ImGuiCol_CloseButton] = ImVec4(1.00f, 0.10f, 0.24f, 1.00f);
    style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(1.00f, 0.10f, 0.24f, 1.00f);
    style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(1.00f, 0.10f, 0.24f, 1.00f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
}
