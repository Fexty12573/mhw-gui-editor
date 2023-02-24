#include "pch.h"
#include "GUIEditor.h"
#include "RichText.h"

#include <imgui_stdlib.h>


void GUIEditor::render_animation(GUIAnimation& anim) {
    constexpr u32 u32_step = 1;
    constexpr u32 u32_fast_step = 10;

    ImGui::PushID("Animation");
    ImGui::PushID(anim.Index);

    if (ImGui::RichTextTreeNode("Anim", anim.get_preview(anim.Index))) {
        ImGui::InputScalar("ID", ImGuiDataType_U32, &anim.ID, &u32_step, &u32_fast_step);
        ImGui::InputText("Name", &anim.Name);
        ImGui::InputScalar("Root Object Index", ImGuiDataType_U32, &anim.RootObjectIndex, &u32_step, &u32_fast_step);
        ImGui::InputScalar("Object Count", ImGuiDataType_U16, &anim.ObjectNum, &u32_step, &u32_fast_step);
        ImGui::InputScalar("Sequence Start Index", ImGuiDataType_U32, &anim.SequenceIndex, &u32_step, &u32_fast_step);
        ImGui::InputScalar("Sequence Count", ImGuiDataType_U16, &anim.SequenceNum, &u32_step, &u32_fast_step);

        if (ImGui::TreeNodeEx("Objects", ImGuiTreeNodeFlags_SpanAvailWidth)) {
            auto object = &m_file.m_objects[anim.RootObjectIndex];
            render_object(*object, anim.SequenceNum);

            while (object->NextIndex != -1) {
                object = &m_file.m_objects[object->NextIndex];
                render_object(*object, anim.SequenceNum);
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNodeEx("Sequences", ImGuiTreeNodeFlags_SpanAvailWidth)) {
            auto& sequences = m_file.m_sequences;
            const u64 max = std::min(static_cast<u64>(anim.SequenceIndex + anim.SequenceNum), sequences.size());

            for (auto i = anim.SequenceIndex; i < max; ++i) {
                render_sequence(sequences[i]);
            }

            ImGui::TreePop();
        }

        ImGui::TreePop();
    }

    ImGui::PopID();
    ImGui::PopID();
}
