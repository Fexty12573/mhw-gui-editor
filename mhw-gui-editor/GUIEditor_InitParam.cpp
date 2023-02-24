#include "pch.h"
#include "GUIEditor.h"
#include "RichText.h"
#include "IconFontAwesome.h"
#include "crc32.h"

#include <imgui_stdlib.h>


void GUIEditor::render_init_param(GUIInitParam& param, ObjectType source_object) const {
    using namespace crc::literals;
    constexpr u32 u32_step = 1;
    constexpr u32 u32_fast_step = 10;

    ImGui::PushID("InitParam");
    ImGui::PushID(param.Index);

    const auto pvr = is_valid_param(param.Type, param.Name, source_object);

    const bool open = ImGui::RichTextTreeNode("InitParam", param.get_preview(param.Index));
    constexpr ImVec4 warning_color = { 1.0f, 1.0f, 0.2f, 1.0f };

    if (pvr.InvalidName || pvr.InvalidType) {
        ImGui::SameLine();
        ImGui::TextColored(warning_color, " " ICON_FA_TRIANGLE_EXCLAMATION);
    }

    if (open) {
        if (pvr.InvalidName) {
            ImGui::TextColored(warning_color, "Warning: Name does not exist in the original object!");
        } else if (pvr.InvalidType) {
            ImGui::TextColored(warning_color, "Warning: Type does not match the original type. Should be %s!", enum_to_string(pvr.CorrectType));
        }

        ImGui::Checkbox("Use", &param.Use);
        ImGui::RichTextCombo("Type", reinterpret_cast<u8*>(&param.Type), ParamTypeNames, 0xFFB0C94E);
        ImGui::InputText("Name", &param.Name);


        switch (param.Type) {
        case ParamType::UNKNOWN:
            break;
        case ParamType::INT:
            ImGui::InputScalar("Value", ImGuiDataType_U32, &param.Value32, &u32_step, &u32_fast_step);
            break;
        case ParamType::FLOAT:
            ImGui::InputFloat("Value", &param.ValueFloat, 0.01f, 0.1f, "%.3f");
            break;
        case ParamType::BOOL:
            ImGui::Checkbox("Value", &param.ValueBool);
            break;
        case ParamType::VECTOR:
            ImGui::ColorEdit4("Value", &param.ValueVector.x, ImGuiColorEditFlags_Float);
            break;
        case ParamType::RESOURCE:
            ImGui::InputScalar("Value", ImGuiDataType_U32, &param.Value32, &u32_step, &u32_fast_step);
            for (const auto& res : m_file.m_resources) {
                if (res.ID == param.Value32) {
                    ImGui::SameLine();
                    ImGui::Text("Resource: %s", res.Path.c_str());
                    break;
                }
            }
            break;
        case ParamType::STRING:
            ImGui::InputText("Value", &param.ValueString);
            break;
        case ParamType::TEXTURE:
            ImGui::InputScalar("Value", ImGuiDataType_U32, &param.Value32, &u32_step, &u32_fast_step);
            for (const auto& tex : m_file.m_textures) {
                if (tex.ID == param.Value32) {
                    ImGui::SameLine();
                    ImGui::Text("Texture: %s", tex.Path.c_str());
                    break;
                }
            }
            break;
        case ParamType::GUIRESOURCE:
            ImGui::InputScalar("Value", ImGuiDataType_U32, &param.Value32, &u32_step, &u32_fast_step);
            for (const auto& res : m_file.m_resources) {
                if (res.ID == param.Value32) {
                    ImGui::SameLine();
                    ImGui::Text("GUIResource: %s", res.Path.c_str());
                    break;
                }
            }
            break;
        case ParamType::GENERALRESOURCE:
            ImGui::InputScalar("Value", ImGuiDataType_U32, &param.Value32, &u32_step, &u32_fast_step);
            for (const auto& res : m_file.m_general_resources) {
                if (res.ID == param.Value32) {
                    ImGui::SameLine();
                    ImGui::Text("GeneralResource: %s", res.Path.c_str());
                    break;
                }
            }
            break;
        case ParamType::FONT: [[fallthrough]];
        case ParamType::MESSAGE: [[fallthrough]];
        case ParamType::VARIABLE: [[fallthrough]];
        case ParamType::ANIMATION: [[fallthrough]];
        case ParamType::EVENT: [[fallthrough]];
        case ParamType::FONT_FILTER: [[fallthrough]];
        case ParamType::ANIMEVENT: [[fallthrough]];
        case ParamType::SEQUENCE:
            ImGui::InputScalar("Value", ImGuiDataType_U32, &param.Value32, &u32_step, &u32_fast_step);
            break;
        case ParamType::INIT_BOOL:
            ImGui::Checkbox("Value", &param.ValueBool);
            break;
        case ParamType::INIT_INT:
            switch (param.NameCRC) {
            case "SamplerState"_crc:
                ImGui::RichTextCombo("Value", &param.Value32, SamplerStateNames, 0xFFA3D7B8);
                break;
            case "BlendState"_crc:
                ImGui::RichTextCombo("Value", &param.Value32, BlendStateNames, 0xFFA3D7B8);
                break;
            case "Alignment"_crc:
                ImGui::RichTextCombo("Value", &param.Value32, AlignmentNames, 0xFFA3D7B8);
                break;
            case "ResolutionAdjust"_crc:
                ImGui::RichTextCombo("Value", &param.Value32, ResolutionAdjustNames, 0xFFA3D7B8);
                break;
            case "AutoWrap"_crc:
                ImGui::RichTextCombo("Value", &param.Value32, AutoWrapNames, 0xFFA3D7B8);
                break;
            case "ColorControl"_crc:
                ImGui::RichTextCombo("Value", &param.Value32, ColorControlNames, 0xFFA3D7B8);
                break;
            case "LetterHAlign"_crc:
                ImGui::RichTextCombo("Value", &param.Value32, LetterHAlignNames, 0xFFA3D7B8);
                break;
            case "LetterVAlign"_crc:
                ImGui::RichTextCombo("Value", &param.Value32, LetterVAlignNames, 0xFFA3D7B8);
                break;
            case "DepthState"_crc:
                ImGui::RichTextCombo("Value", &param.Value32, DepthStateNames, 0xFFA3D7B8);
                break;
            case "Billboard"_crc:
                ImGui::RichTextCombo("Value", &param.Value32, BillboardNames, 0xFFA3D7B8);
                break;
            case "DrawPass"_crc:
                ImGui::RichTextCombo("Value", &param.Value32, DrawPassNames, 0xFFA3D7B8);
                break;
            case "Mask"_crc:
                ImGui::RichTextCombo("Value", &param.Value32, MaskTypeNames, 0xFFA3D7B8);
                break;
            default:
                ImGui::InputScalar("Value", ImGuiDataType_S32, &param.Value32, &u32_step, &u32_fast_step);
                break;
            }

            break;

        case ParamType::INIT_INT32:
            switch (param.NameCRC) {
            case "FontStyleId"_crc:
                ImGui::RichTextCombo("Value", &param.Value32, FontStyleNames, 0xFFA3D7B8);
                break;
            default:
                ImGui::InputScalar("Value", ImGuiDataType_S32, &param.Value32, &u32_step, &u32_fast_step);
                break;
            }
        }

        ImGui::TreePop();
    }

    ImGui::PopID();
    ImGui::PopID();
}
