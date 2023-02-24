#include "pch.h"
#include "GUIEditor.h"
#include "RichText.h"

#include <imgui_stdlib.h>


void GUIEditor::render_sequence(GUISequence& seq) const {
    constexpr u32 u32_step = 1;
    constexpr u32 u32_fast_step = 10;

    ImGui::PushID("Sequence");
    ImGui::PushID(seq.Index);

    if (ImGui::RichTextTreeNode("Seq", seq.get_preview(seq.Index))) {
        ImGui::InputScalar("ID", ImGuiDataType_U32, &seq.ID, &u32_step, &u32_fast_step);
        ImGui::InputText("Name", &seq.Name);
        ImGui::InputScalar("Frame Count", ImGuiDataType_U32, &seq.FrameCount, &u32_step, &u32_fast_step);

        ImGui::TreePop();
    }

    ImGui::PopID();
    ImGui::PopID();
}
