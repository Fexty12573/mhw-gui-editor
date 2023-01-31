#pragma once

#include "pch.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>
#include <algorithm>


namespace ImGui {

bool HermiteCurve(std::string_view label, MtHermiteCurve* curve, const ImVec2& size) {
    constexpr int SMOOTHNESS = 256;
    constexpr float F_SMOOTHNESS = 256.0f;
    constexpr float POINT_SIZE = 6.0f;
    constexpr float POINT_SIZE_SELECTED = 8.0f;
    constexpr float POINT_BORDER_SIZE = 1.0f;
    constexpr float CURVE_WIDTH = 3.0f;

    const ImGuiWindow* window = GetCurrentWindow();
    ImDrawList* draw_list = window->DrawList;
    const ImGuiStyle& style = GetStyle();
    if (window->SkipItems) {
        return false;
    }

    bool changed = false;
    bool hovered = false;

    const ImVec2 avail = GetContentRegionAvail();
    const ImVec2 bb_size = size.x > 0.0f ? size : ImVec2(avail.x, 200.0f); // TODO: make this configurable
    const ImRect bb{ window->DC.CursorPos, window->DC.CursorPos + bb_size };

    ItemSize(bb);
    if (!ItemAdd(bb, 0)) {
        return false;
    }

    const auto id = GetID(label.data());
    ImGui::PushID(id);
    RenderFrame(bb.Min, bb.Max, GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);

    // Draw grid
    for (int i = 0; i <= bb_size.x; i += static_cast<int>(bb_size.x / 4)) {
        draw_list->AddLine(
            { bb.Min.x + i, bb.Min.y },
            { bb.Min.x + i, bb.Max.y },
            GetColorU32(ImGuiCol_TextDisabled)
        );
    }

    for (int i = 0; i <= bb_size.y; i += static_cast<int>(bb_size.y / 4)) {
        draw_list->AddLine(
            { bb.Min.x, bb.Min.y + i },
            { bb.Max.x, bb.Min.y + i },
            GetColorU32(ImGuiCol_TextDisabled)
        );
    }

    // Draw curve
    for (int i = 0; i < SMOOTHNESS; ++i) {
        const float x1 = i / (F_SMOOTHNESS - 1);
        const float x2 = (i + 1) / (F_SMOOTHNESS - 1);

        const float y1 = 1.0f - curve->get(x1); // flip y axis
        const float y2 = 1.0f - curve->get(x2); 

        draw_list->AddLine(
            { bb.Min.x + x1 * bb_size.x, bb.Min.y + y1 * bb_size.y },
            { bb.Min.x + x2 * bb_size.x, bb.Min.y + y2 * bb_size.y },
            GetColorU32(ImGuiCol_PlotLines),
            CURVE_WIDTH
        );
    }

    // Handle points
    constexpr int point_count = MtHermiteCurve::point_count();

    const ImVec2 mouse_pos = GetIO().MousePos;
    float point_dist[point_count]{};

    for (int i = 0; i < point_count; ++i) {
        const ImVec2 pos = {
            bb.Min.x + curve->x[i] * bb_size.x,
            bb.Min.y + (1.0f - curve->y[i]) * bb_size.y
        };

        const float dx = mouse_pos.x - pos.x;
        const float dy = mouse_pos.y - pos.y;

        point_dist[i] = dx * dx + dy * dy;
    }

    int closest = 0;
    for (int i = 1; i < point_count; ++i) {
        if (point_dist[i] < point_dist[closest]) {
            closest = i;
        }
    }

    if (point_dist[closest] < POINT_SIZE * POINT_SIZE * 8.0f) {
        hovered = true;

        SetTooltip("(%.3f, %.3f)", curve->x[closest], curve->y[closest]);

        if (IsMouseClicked(ImGuiMouseButton_Left) || IsMouseDragging(ImGuiMouseButton_Left)) {
            float& x = curve->x[closest];
            float& y = curve->y[closest];
            const auto& io = GetIO();

            x = std::clamp((io.MousePos.x - bb.Min.x) / bb_size.x, 0.0f, 1.0f);
            y = 1.0f - std::clamp((io.MousePos.y - bb.Min.y) / bb_size.y, 0.0f, 1.0f);
            /*x = std::clamp(x + io.MouseDelta.x / bb_size.x, 0.0f, 1.0f);
            y = std::clamp(y - io.MouseDelta.y / bb_size.y, 0.0f, 1.0f);*/

            if (closest > 0) {
                x = std::max(x, curve->x[closest - 1] + 0.001f);
            }

            if (closest < point_count - 1) {
                x = std::min(x, curve->x[closest + 1] - 0.001f);
            }

            changed = true;
        }
    }

    // Draw points
    for (int i = 0; i < point_count; ++i) {
        const float point_size = (i == closest) && hovered ? POINT_SIZE_SELECTED : POINT_SIZE;
        const float x = curve->x[i];
        const float y = 1.0f - curve->y[i]; // flip y

        // Border
        draw_list->AddCircleFilled(
            { bb.Min.x + x * bb_size.x, bb.Min.y + y * bb_size.y },
            point_size + POINT_BORDER_SIZE,
            GetColorU32(ImGuiCol_Border)
        );

        // Center
        draw_list->AddCircleFilled(
            { bb.Min.x + x * bb_size.x, bb.Min.y + y * bb_size.y },
            point_size,
            GetColorU32(ImGuiCol_PlotLines)
        );
    }

    ImGui::PopID();

    return changed;
}

};
