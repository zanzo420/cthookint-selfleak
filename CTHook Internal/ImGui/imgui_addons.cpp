//
// Created by apostrophe on 24.10.17.
//
#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_PLACEMENT_NEW
#include "imgui_internal.h"

#include <ctype.h>      // toupper, isprint
#include <stdlib.h>     // NULL, malloc, free, qsort, atoi
#include <stdio.h>      // vsnprintf, sscanf, printf
#include <limits.h>     // INT_MIN, INT_MAX
#if defined(_MSC_VER) && _MSC_VER <= 1500 // MSVC 2008 or earlier
#include <stddef.h>     // intptr_t
#else
#include <stdint.h>     // intptr_t
#endif

#ifdef _MSC_VER
#pragma warning (disable: 4127) // condition expression is constant
#pragma warning (disable: 4505) // unreferenced local function has been removed (stb stuff)
#pragma warning (disable: 4996) // 'This function or variable may be unsafe': strcpy, strdup, sprintf, vsnprintf, sscanf, fopen
#endif

// Clang warnings with -Weverything
#ifdef __clang__
#pragma clang diagnostic ignored "-Wunknown-pragmas"        // warning : unknown warning group '-Wformat-pedantic *'        // not all warnings are known by all clang versions.. so ignoring warnings triggers new warnings on some configuration. great!
#pragma clang diagnostic ignored "-Wold-style-cast"         // warning : use of old-style cast                              // yes, they are more terse.
#pragma clang diagnostic ignored "-Wfloat-equal"            // warning : comparing floating point with == or != is unsafe   // storing and comparing against same constants (typically 0.0f) is ok.
#pragma clang diagnostic ignored "-Wformat-nonliteral"      // warning : format string is not a string literal              // passing non-literal to vsnformat(). yes, user passing incorrect format strings can crash the code.
#pragma clang diagnostic ignored "-Wexit-time-destructors"  // warning : declaration requires an exit-time destructor       // exit-time destruction order is undefined. if MemFree() leads to users code that has been disabled before exit it might cause problems. ImGui coding style welcomes static/globals.
#pragma clang diagnostic ignored "-Wglobal-constructors"    // warning : declaration requires a global destructor           // similar to above, not sure what the exact difference it.
#pragma clang diagnostic ignored "-Wsign-conversion"        // warning : implicit conversion changes signedness             //
#pragma clang diagnostic ignored "-Wformat-pedantic"        // warning : format specifies type 'void *' but the argument has type 'xxxx *' // unreasonable, would lead to casting every %p arg to void*. probably enabled by -pedantic.
#pragma clang diagnostic ignored "-Wint-to-void-pointer-cast" // warning : cast to 'void *' from smaller integer type 'int' //
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wunused-function"          // warning: 'xxxx' defined but not used
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"      // warning: cast to pointer from integer of different size
#pragma GCC diagnostic ignored "-Wformat"                   // warning: format '%p' expects argument of type 'void*', but argument 6 has type 'ImGuiWindow*'
#pragma GCC diagnostic ignored "-Wdouble-promotion"         // warning: implicit conversion from 'float' to 'double' when passing argument to function
#pragma GCC diagnostic ignored "-Wconversion"               // warning: conversion to 'xxxx' from 'xxxx' may alter its value
#pragma GCC diagnostic ignored "-Wcast-qual"                // warning: cast from type 'xxxx' to type 'xxxx' casts away qualifiers
#pragma GCC diagnostic ignored "-Wformat-nonliteral"        // warning: format not a string literal, format string not checked
#endif


bool ImGui::SliderCheckbox(const char* label, bool* v)
{
// ©leaked russian hack
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;
    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    const ImRect check_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(label_size.y + style.FramePadding.x * 6, label_size.y + style.FramePadding.y / 2));
    ItemSize(check_bb, style.FramePadding.y);
    ImRect total_bb = check_bb;
    if (label_size.x > 0)
        SameLine(0, style.ItemInnerSpacing.x);
    const ImRect text_bb(window->DC.CursorPos + ImVec2(0, style.FramePadding.y), window->DC.CursorPos + ImVec2(0, style.FramePadding.y) + label_size);
    if (label_size.x > 0)
    {
        ItemSize(ImVec2(text_bb.GetWidth(), check_bb.GetHeight()), style.FramePadding.y);
        total_bb = ImRect(ImMin(check_bb.Min, text_bb.Min), ImMax(check_bb.Max, text_bb.Max));
    }
    if (!ItemAdd(total_bb, id))
        return false;
    bool hovered, held;
    bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);
    if (pressed)
        *v = !(*v);
    const float check_sz = ImMin(check_bb.GetWidth(), check_bb.GetHeight());
    window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 + 6, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(1.0f, 0.0f, 0.0f, 1.0f)), 12);
    window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 + 5, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(1.0f, 0.0f, 0.0f, 1.0f)), 12);
    window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 + 4, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(1.0f, 0.0f, 0.0f, 1.0f)), 12);
    window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 + 3, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(1.0f, 0.0f, 0.0f, 1.0f)), 12);
    window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 + 2, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(1.0f, 0.0f, 0.0f, 1.0f)), 12);
    window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 + 1, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(1.0f, 0.0f, 0.0f, 1.0f)), 12);
    window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(1.0f, 0.0f, 0.0f, 1.0f)), 12);
    window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 - 1, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(1.0f, 0.0f, 0.0f, 1.0f)), 12);
    window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 - 2, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(1.0f, 0.0f, 0.0f, 1.0f)), 12);
    window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 - 3, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(1.0f, 0.0f, 0.0f, 1.0f)), 12);
    window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 - 4, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(1.0f, 0.0f, 0.0f, 1.0f)), 12);
    window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 - 5, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(1.0f, 0.0f, 0.0f, 1.0f)), 12);
    window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 - 6, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)), 12);
    if (*v)
    {


        window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 + 6, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(0.0f, 1.0f, 0.0f, 1.0f)), 12);
        window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 + 5, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(0.0f, 1.0f, 0.0f, 1.0f)), 12);
        window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 + 4, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(0.0f, 1.0f, 0.0f, 1.0f)), 12);
        window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 + 3, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(0.0f, 1.0f, 0.0f, 1.0f)), 12);
        window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 + 2, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(0.0f, 1.0f, 0.0f, 1.0f)), 12);
        window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 + 1, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(0.0f, 1.0f, 0.0f, 1.0f)), 12);
        window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(0.0f, 1.0f, 0.0f, 1.0f)), 12);
        window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 - 1, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(0.0f, 1.0f, 0.0f, 1.0f)), 12);
        window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 - 2, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(0.0f, 1.0f, 0.0f, 1.0f)), 12);
        window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 - 3, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(0.0f, 1.0f, 0.0f, 1.0f)), 12);
        window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 - 4, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(0.0f, 1.0f, 0.0f, 1.0f)), 12);
        window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 - 5, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(0.0f, 1.0f, 0.0f, 1.0f)), 12);
        window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 - 6, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(0.0f, 1.0f, 0.0f, 1.0f)), 12);
        window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 + 6, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)), 12);
    }
    else
    {
        window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 - 6, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)), 12);
    }
    if (label_size.x > 0.0f)
        RenderText(text_bb.GetTL(), label);
    return pressed;
}

int ImGui::PiePopupSelectMenu( const ImVec2& center, const char* popup_id, const char** items, int items_count , ImColor col1 , ImColor col2 , ImColor col3 ) {
    int ret = -1;

    // FIXME: Missing a call to query if Popup is open so we can move the PushStyleColor inside the BeginPopupBlock (e.g. IsPopupOpen() in imgui.cpp)
    // FIXME: Our PathFill function only handle convex polygons, so we can't have items spanning an arc too large else inner concave edge artifact is too visible, hence the ImMax(7,items_count)
    ImGui::PushStyleColor( ImGuiCol_WindowBg, ImVec4( 0, 0, 0, 0 ) );
    ImGui::PushStyleColor( ImGuiCol_Border, ImVec4( 0, 0, 0, 0 ) );
    ImGui::SetNextWindowPos( { -100, -100 } );
    if ( ImGui::BeginPopup( popup_id ) ) {
        const ImVec2 drag_delta = ImVec2( ImGui::GetIO().MousePos.x - center.x, ImGui::GetIO().MousePos.y - center.y );
        const float drag_dist2 = drag_delta.x * drag_delta.x + drag_delta.y * drag_delta.y;

        const ImGuiStyle& style = ImGui::GetStyle();
        const float RADIUS_MIN = 30.0f;
        const float RADIUS_MAX = 120.0f;
        const float RADIUS_INTERACT_MIN = 20.0f;
        const int ITEMS_MIN = 6;

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        //ImGuiWindow* window = ImGui::GetCurrentWindow();
        draw_list->PushClipRectFullScreen();
        draw_list->PathArcTo( center, ( RADIUS_MIN + RADIUS_MAX ) * 0.5f, 0.0f, IM_PI * 2.0f * 0.99f,
                              32 ); // FIXME: 0.99f look like full arc with closed thick stroke has a bug now
        draw_list->PathStroke( ImColor( 0, 0, 0 ), true, RADIUS_MAX - RADIUS_MIN );

        const float item_arc_span = 2 * IM_PI / ImMax( ITEMS_MIN, items_count );
        float drag_angle = atan2f( drag_delta.y, drag_delta.x );
        if ( drag_angle < -0.5f * item_arc_span )
            drag_angle += 2.0f * IM_PI;
        //ImGui::Text("%f", drag_angle);    // [Debug]

        int item_hovered = -1;
        for ( int item_n = 0; item_n < items_count; item_n++ ) {
            const char* item_label = items[item_n];
            const float item_ang_min = item_arc_span * ( item_n + 0.02f ) - item_arc_span *
                                                                            0.5f; // FIXME: Could calculate padding angle based on how many pixels they'll take
            const float item_ang_max = item_arc_span * ( item_n + 0.98f ) - item_arc_span * 0.5f;

            bool hovered = false;
            if ( drag_dist2 >= RADIUS_INTERACT_MIN * RADIUS_INTERACT_MIN ) {
                if ( drag_angle >= item_ang_min && drag_angle < item_ang_max )
                    hovered = true;
            }
            // bool selected = p_selected && (*p_selected == item_n);

            int arc_segments = ( int ) ( 32 * item_arc_span / ( 2 * IM_PI ) ) + 1;
            draw_list->PathArcTo( center, RADIUS_MAX - style.ItemInnerSpacing.x, item_ang_min, item_ang_max,
                                  arc_segments );
            draw_list->PathArcTo( center, RADIUS_MIN + style.ItemInnerSpacing.x, item_ang_max, item_ang_min,
                                  arc_segments );
            //draw_list->PathFill(window->Color(hovered ? ImGuiCol_HeaderHovered : ImGuiCol_FrameBg));
            draw_list->PathFillConvex(
                    hovered ?  col2: /* selected ? ImColor(170,170,215) :*/ col1 );

            ImVec2 text_size = ImGui::GetWindowFont()->CalcTextSizeA( ImGui::GetWindowFontSize(), FLT_MAX, 0.0f,
                                                                      item_label );
            ImVec2 text_pos = ImVec2(
                    center.x + cosf( ( item_ang_min + item_ang_max ) * 0.5f ) * ( RADIUS_MIN + RADIUS_MAX ) * 0.5f -
                    text_size.x * 0.5f,
                    center.y + sinf( ( item_ang_min + item_ang_max ) * 0.5f ) * ( RADIUS_MIN + RADIUS_MAX ) * 0.5f -
                    text_size.y * 0.5f );
            draw_list->AddText( text_pos, col3, item_label );

            if ( hovered )
                item_hovered = item_n;
        }
        draw_list->PopClipRect();

        if ( ImGui::IsMouseReleased( 1 ) || ImGui::IsMouseReleased( 2 ) ) { //middle click == 2 ? idk fix if not
            ImGui::CloseCurrentPopup();
            ret = item_hovered;
            //   if (p_selected)
            //    *p_selected = item_hovered;
        }
        ImGui::EndPopup();
    }
    ImGui::PopStyleColor( 2 );
    return ret;
}