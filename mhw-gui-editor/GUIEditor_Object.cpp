#include "pch.h"
#include "GUIEditor.h"
#include "RichText.h"

#include <imgui_stdlib.h>


void GUIEditor::render_object(GUIObject& obj, u32 seq_count) {
    constexpr u32 u32_step = 1;
    constexpr u32 u32_fast_step = 10;

    ImGui::PushID("Object");
    ImGui::PushID(obj.Index);

    const bool open = ImGui::RichTextTreeNode("Obj", obj.get_preview(obj.Index));

    if (ImGui::BeginPopupContextItem("Object Context Menu")) {
        if (ImGui::MenuItem("Add Default InitParams")) {

        }

        if (ImGui::MenuItem("Delete")) {
            // TODO: Delete object
        }

        ImGui::EndPopup();
    }

    if (open) {
        ImGui::InputScalar("ID", ImGuiDataType_U32, &obj.ID, &u32_step, &u32_fast_step);
        ImGui::InputInt("Child Index", &obj.ChildIndex);
        ImGui::InputInt("Next Index", &obj.NextIndex);
        ImGui::InputText("Name", &obj.Name);
        if (ImGui::BeginCombo("Type", ObjectTypeNames.at(obj.Type))) {
            for (const auto& [type, name] : ObjectTypeNames) {
                if (ImGui::Selectable(name, obj.Type == type)) {
                    obj.Type = type;
                }
            }

            ImGui::EndCombo();
        }

        if (obj.ChildIndex != -1) {
            auto object = &m_file.m_objects[obj.ChildIndex];
            render_object(*object, seq_count);

            while (object->NextIndex != -1) {
                object = &m_file.m_objects[object->NextIndex];
                render_object(*object, seq_count);
            }
        }

        if (obj.InitParamNum > 0) {
            if (ImGui::TreeNodeEx("InitParams", ImGuiTreeNodeFlags_SpanAvailWidth)) {
                const u64 max = std::min(static_cast<u64>(obj.InitParamIndex + obj.InitParamNum), m_file.m_init_params.size());
                for (auto i = obj.InitParamIndex; i < max; ++i) {
                    render_init_param(m_file.m_init_params[i], obj.Type);
                }

                if (obj.InitParamIndex + static_cast<u64>(obj.InitParamNum) > m_file.m_init_params.size()) {
                    ImGui::TextColored({ 1.0f, 1.0f, 0.2f, 1.0f }, "Warning: InitParamNum goes out of bounds!");
                }

                ImGui::TreePop();
            }
        }

        if (seq_count > 0) {
            if (ImGui::TreeNodeEx("ObjectSequences", ImGuiTreeNodeFlags_SpanAvailWidth)) {
                const u64 max = std::min(static_cast<u64>(obj.ObjectSequenceIndex + seq_count), m_file.m_obj_sequences.size());
                for (auto i = obj.ObjectSequenceIndex; i < max; ++i) {
                    render_obj_sequence(m_file.m_obj_sequences[i], obj.Type);
                }

                ImGui::TreePop();
            }
        }

        ImGui::TreePop();
    }

    ImGui::PopID();
    ImGui::PopID();
}
