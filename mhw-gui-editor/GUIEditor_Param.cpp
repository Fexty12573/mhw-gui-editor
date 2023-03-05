#include "pch.h"
#include "GUIEditor.h"
#include "RichText.h"
#include "IconFontAwesome.h"
#include "crc32.h"

#include <fmt/format.h>
#include <imgui_stdlib.h>


void GUIEditor::render_param(GUIParam& param, ObjectType source_object) {
    using namespace crc::literals;
    constexpr u32 u32_step = 1;
    constexpr u32 u32_fast_step = 10;

    ImGui::PushID("Param");
    ImGui::PushID(param.Index);

    const auto pvr = is_valid_param(param.Type, param.Name, source_object);

    const bool open = ImGui::RichTextTreeNode("Param", param.get_preview(param.Index));
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

        if (ImGui::RichTextCombo("Type", reinterpret_cast<u8*>(&param.Type), ParamTypeNames, 0xFFB0C94E)) {
            param.perform_value_operation(
                [](auto, auto& p) { p.Values = std::vector<u8>(p.ValueCount, 0); },
                [](auto, auto& p) { p.Values = std::vector<u32>(p.ValueCount, 0); },
                [](auto, auto& p) { p.Values = std::vector<f32>(p.ValueCount, 0.0f); },
                [](auto, auto& p) { p.Values = std::vector<MtVector4>(p.ValueCount, MtVector4{}); },
                [](auto, auto& p) { p.Values = std::vector<std::string>(p.ValueCount, ""); },
                false
            );
        }

        const u8 prev_value_count = param.ValueCount;
        if (ImGui::InputScalar("Count", ImGuiDataType_U8, &param.ValueCount, &u32_step, &u32_fast_step)) {
            param.perform_value_operation(
                [](auto v, auto& p) { v->resize(p.ValueCount, 0); },
                [](auto v, auto& p) { v->resize(p.ValueCount, 0); },
                [](auto v, auto& p) { v->resize(p.ValueCount, 0.0f); },
                [](auto v, auto& p) { v->resize(p.ValueCount, MtVector4{}); },
                [](auto v, auto& p) { v->resize(p.ValueCount, ""); }
            );

            if (m_settings.AutoAdjustKeyFrames) {
                if (prev_value_count < param.ValueCount) {
                    for (u32 i = prev_value_count; i < param.ValueCount; ++i) {
                        m_file.insert_key({}, static_cast<s32>(param.KeyIndex + i));
                    }
                } else {
                    m_file.erase_keys(param.KeyIndex + param.ValueCount, prev_value_count - param.ValueCount);
                }
            }
        }
        ImGui::InputText("Name", &param.Name);

        for (auto i = 0u; i < param.ValueCount; ++i) {
            const std::string name = fmt::format("Value[{}]", i);
            switch (param.Type) {
            case ParamType::UNKNOWN:
                break;
            case ParamType::INT:
                ImGui::InputScalar(name.c_str(), ImGuiDataType_U32, &param.get_value<u32>(i), &u32_step, &u32_fast_step);
                break;
            case ParamType::FLOAT:
                ImGui::InputFloat(name.c_str(), &param.get_value<float>(i), 0.01f, 0.1f, "%.3f");
                break;
            case ParamType::BOOL:
                ImGui::Checkbox(name.c_str(), reinterpret_cast<bool*>(&param.get_value<u8>(i)));
                break;
            case ParamType::VECTOR:
                ImGui::ColorEdit4(name.c_str(), &param.get_value<vector4>(i).x, ImGuiColorEditFlags_Float);
                break;
            case ParamType::RESOURCE:
            {
                auto& v = param.get_value<u32>(i);
                ImGui::InputScalar(name.c_str(), ImGuiDataType_U32, &v, &u32_step, &u32_fast_step);
                for (const auto& res : m_file.m_resources) {
                    if (res.ID == v) {
                        ImGui::SameLine();
                        ImGui::Text("Resource: %s", res.Path.c_str());
                        break;
                    }
                }

                break;
            }
            case ParamType::STRING:
                ImGui::InputText(name.c_str(), &param.get_value<std::string>(i));
                break;
            case ParamType::TEXTURE:
            {
                auto& v = param.get_value<u32>(i);
                ImGui::InputScalar(name.c_str(), ImGuiDataType_U32, &v, &u32_step, &u32_fast_step);
                for (const auto& tex : m_file.m_textures) {
                    if (tex.ID == v) {
                        ImGui::SameLine();
                        ImGui::Text("Texture: %s | %s", tex.Name.c_str(), tex.Path.c_str());
                        break;
                    }
                }

                break;
            }
            case ParamType::GUIRESOURCE:
            {
                auto& v = param.get_value<u32>(i);
                ImGui::InputScalar(name.c_str(), ImGuiDataType_U32, &v, &u32_step, &u32_fast_step);
                for (const auto& tex : m_file.m_resources) {
                    if (tex.ID == v) {
                        ImGui::SameLine();
                        ImGui::Text("GUIResource: %s", tex.Path.c_str());
                        break;
                    }
                }

                break;
            }
            case ParamType::GENERALRESOURCE:
            {
                auto& v = param.get_value<u32>(i);
                ImGui::InputScalar(name.c_str(), ImGuiDataType_U32, &v, &u32_step, &u32_fast_step);
                for (const auto& tex : m_file.m_general_resources) {
                    if (tex.ID == v) {
                        ImGui::SameLine();
                        ImGui::Text("GeneralResource: %s", tex.Path.c_str());
                        break;
                    }
                }

                break;
            }
            case ParamType::FONT: [[fallthrough]];
            case ParamType::MESSAGE: [[fallthrough]];
            case ParamType::VARIABLE: [[fallthrough]];
            case ParamType::ANIMATION: [[fallthrough]];
            case ParamType::EVENT: [[fallthrough]];
            case ParamType::FONT_FILTER: [[fallthrough]];
            case ParamType::ANIMEVENT: [[fallthrough]];
            case ParamType::SEQUENCE:
                ImGui::InputScalar(name.c_str(), ImGuiDataType_U32, &param.get_value<u32>(i), &u32_step, &u32_fast_step);
                break;
            case ParamType::INIT_BOOL:
                ImGui::Checkbox(name.c_str(), reinterpret_cast<bool*>(&param.get_value<u8>(i)));
                break;
            case ParamType::INIT_INT:
                switch (param.NameCRC) {
                case "SamplerState"_crc:
                    ImGui::RichTextCombo(name.c_str(), &param.get_value<u8>(i), SamplerStateNames, 0xFFA3D7B8);
                    break;
                case "BlendState"_crc:
                    ImGui::RichTextCombo(name.c_str(), &param.get_value<u8>(i), BlendStateNames, 0xFFA3D7B8);
                    break;
                case "Alignment"_crc:
                    ImGui::RichTextCombo(name.c_str(), &param.get_value<u8>(i), AlignmentNames, 0xFFA3D7B8);
                    break;
                case "ResolutionAdjust"_crc:
                    ImGui::RichTextCombo(name.c_str(), &param.get_value<u8>(i), ResolutionAdjustNames, 0xFFA3D7B8);
                    break;
                case "AutoWrap"_crc:
                    ImGui::RichTextCombo(name.c_str(), &param.get_value<u8>(i), AutoWrapNames, 0xFFA3D7B8);
                    break;
                case "ColorControl"_crc:
                    ImGui::RichTextCombo(name.c_str(), &param.get_value<u8>(i), ColorControlNames, 0xFFA3D7B8);
                    break;
                case "LetterHAlign"_crc:
                    ImGui::RichTextCombo(name.c_str(), &param.get_value<u8>(i), LetterHAlignNames, 0xFFA3D7B8);
                    break;
                case "LetterVAlign"_crc:
                    ImGui::RichTextCombo(name.c_str(), &param.get_value<u8>(i), LetterVAlignNames, 0xFFA3D7B8);
                    break;
                case "DepthState"_crc:
                    ImGui::RichTextCombo(name.c_str(), &param.get_value<u8>(i), DepthStateNames, 0xFFA3D7B8);
                    break;
                case "Billboard"_crc:
                    ImGui::RichTextCombo(name.c_str(), &param.get_value<u8>(i), BillboardNames, 0xFFA3D7B8);
                    break;
                case "DrawPass"_crc:
                    ImGui::RichTextCombo(name.c_str(), &param.get_value<u8>(i), DrawPassNames, 0xFFA3D7B8);
                    break;
                case "Mask"_crc:
                    ImGui::RichTextCombo(name.c_str(), &param.get_value<u8>(i), MaskTypeNames, 0xFFA3D7B8);
                    break;
                default:
                    ImGui::InputScalar(name.c_str(), ImGuiDataType_U8, &param.get_value<u8>(i), &u32_step, &u32_fast_step);
                    break;
                }

                break;

            case ParamType::INIT_INT32:
                switch (param.NameCRC) {
                case "FontStyleId"_crc:
                    ImGui::RichTextCombo(name.c_str(), &param.get_value<u32>(i), FontStyleNames, 0xFFA3D7B8);
                    break;
                default:
                    ImGui::InputScalar(name.c_str(), ImGuiDataType_U32, &param.get_value<u32>(i), &u32_step, &u32_fast_step);
                    break;
                }
                break;
            }


        }

        if (ImGui::TreeNode("Keyframes")) {
            const u64 max = std::min(static_cast<u64>(param.KeyIndex + param.ValueCount), m_file.m_keys.size());

            for (auto i = param.KeyIndex; i < max; i++) {
                render_key(m_file.m_keys[i], param.Type);
            }

            ImGui::TreePop();
        }

        ImGui::TreePop();
    }

    ImGui::PopID();
    ImGui::PopID();
}
