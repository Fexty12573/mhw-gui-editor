#include "pch.h"
#include "GUIEditor.h"
#include "RichText.h"

#include <imgui_stdlib.h>
#include <fmt/format.h>

void GUIEditor::render_font_filter(const std::shared_ptr<GUIFontFilter>& filter) const {
    constexpr u32 u32_step = 1;
    constexpr u32 u32_fast_step = 10;

    ImGui::PushID("FontFilter");
    ImGui::PushID((int)filter->ID);

    const bool open = ImGui::RichTextTreeNode("filter", filter->get_preview());

    if (open) {
        ImGui::InputScalar("ID", ImGuiDataType_U32, &filter->ID, &u32_step, &u32_fast_step);

        switch (filter->Type) {
        case ObjectType::cGUIFontFilterShadow:
            render_font_filter_shadow(std::static_pointer_cast<GUIFontFilterShadow>(filter));
            break;
        case ObjectType::cGUIFontFilterBorder:
            render_font_filter_border(std::static_pointer_cast<GUIFontFilterBorder>(filter));
            break;
        case ObjectType::cGUIFontFilterGradationOverlay:
            render_font_filter_gradation_overlay(std::static_pointer_cast<GUIFontFilterGradationOverlay>(filter));
            break;
        case ObjectType::cGUIFontFilterDistanceField:
            render_font_filter_distance_field(std::static_pointer_cast<GUIFontFilterDistanceField>(filter));
            break;
        default:
            break;
        }

        ImGui::TreePop();
    }

    ImGui::PopID();
    ImGui::PopID();
}

void GUIEditor::render_font_filter_shadow(const std::shared_ptr<GUIFontFilterShadow>& filter) const {
    ImGui::InputInt("Distance", &filter->Distance);
    ImGui::InputInt("Rotation", &filter->Rotation);

    vector4 color = filter->color_to_vector4(filter->Color);
    ImGui::ColorEdit4("Color", &color.x);
    filter->Color = filter->vector4_to_color(color);
}

void GUIEditor::render_font_filter_border(const std::shared_ptr<GUIFontFilterBorder>& filter) const {
    constexpr u32 u32_step = 1;
    constexpr u32 u32_fast_step = 10;

    ImGui::InputScalar("Border Type", ImGuiDataType_U32, &filter->BorderType, &u32_step, &u32_fast_step);
    ImGui::InputInt("Distance", &filter->Distance);

    vector4 color = filter->color_to_vector4(filter->Color);
    ImGui::ColorEdit4("Color", &color.x);
    filter->Color = filter->vector4_to_color(color);
}

void GUIEditor::render_font_filter_gradation_overlay(const std::shared_ptr<GUIFontFilterGradationOverlay>& filter) const {
    constexpr u32 u32_step = 1;
    constexpr u32 u32_fast_step = 10;

    ImGui::InputScalar("Texture Index", ImGuiDataType_U32, &filter->TextureIndex, &u32_step, &u32_fast_step);
    ImGui::InputText("Name", &filter->Name);
}

void GUIEditor::render_font_filter_distance_field(const std::shared_ptr<GUIFontFilterDistanceField>& filter) const {
    constexpr u32 u32_step = 1;
    constexpr u32 u32_fast_step = 10;

    ImGui::InputScalar("Param Count", ImGuiDataType_U32, &filter->ParamNum, &u32_step, &u32_fast_step);
    ImGui::InputScalar("Offset", ImGuiDataType_U32, &filter->Offset, &u32_step, &u32_fast_step);
    ImGui::InputScalar("Offset Angle", ImGuiDataType_U32, &filter->OffsetAngle, &u32_step, &u32_fast_step);

    if (ImGui::BeginCombo("Blending", enum_to_string(filter->Blending))) {
        for (u32 i = 0; i < BlendStateNames.size(); ++i) {
            const auto state = (BlendState)i;
            if (ImGui::Selectable(enum_to_string(state), filter->Blending == state)) {
                filter->Blending = state;
            }
        }

        ImGui::EndCombo();
    }

    constexpr auto render_param = [u32_step, u32_fast_step](const std::shared_ptr<GUIFontFilterDistanceField>& filter, const u32 index) {
        auto& param = filter->Params[index];

        ImGui::PushID(index);

        vector4 color = filter->color_to_vector4(param.Color);
        ImGui::ColorEdit4("Color", &color.x);
        param.Color = filter->vector4_to_color(color);

        ImGui::InputInt("Size", &param.Size);
        ImGui::InputScalar("Softness", ImGuiDataType_U32, &param.Softness, &u32_step, &u32_fast_step);
        ImGui::InputScalar("Range", ImGuiDataType_U32, &param.Range, &u32_step, &u32_fast_step);
        ImGui::InputInt("Strength", &param.Strength);

        ImGui::PopID();
    };

    if (ImGui::RichTextTreeNode("Param0", "Params[<C FFA3D7B8>0</C>]")) {
        render_param(filter, 0);
        ImGui::TreePop();
    }

    if (ImGui::RichTextTreeNode("Param1", "Params[<C FFA3D7B8>1</C>]")) {
        render_param(filter, 1);
        ImGui::TreePop();
    }
}
