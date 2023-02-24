#include "pch.h"
#include "GUIEditor.h"
#include "RichText.h"
#include "CurveEditor.h"


void GUIEditor::render_key(GUIKey& key, ParamType type) const {
    constexpr u32 u32_step = 1;
    constexpr u32 u32_fast_step = 10;

    ImGui::PushID("Key");
    ImGui::PushID(key.Index);

    if (ImGui::RichTextTreeNode("Key", key.get_preview(key.Index))) {
        u32 frame = key.Data.Bitfield.Frame;
        u8 mode = key.Data.Bitfield.Mode_;

        ImGui::InputScalar("Frame", ImGuiDataType_U32, &frame, &u32_step, &u32_fast_step);
        ImGui::RichTextCombo("Interpolation Mode", &mode, KeyModeNames, 0xFFA3D7B8);
        if (type != ParamType::FLOAT && type != ParamType::VECTOR &&
            type != ParamType::INT && type != ParamType::UNKNOWN && key.Data.Named.Mode != KeyMode::CONSTANT_0) {
            ImGui::TextColored({ 1.0f, 1.0f, 0.2f, 1.0f }, "Note: Interpolation Mode only works for float, vector and int types.");
        } else {
            if (key.Data.Named.Mode == KeyMode::HERMITE2) {
                ImGui::HermiteCurve("Curve", &key.Curve, { 350, 350 });
            }
        }

        key.Data.Bitfield.Frame = frame;
        key.Data.Bitfield.Mode_ = mode;

        ImGui::TreePop();
    }

    ImGui::PopID();
    ImGui::PopID();
}
