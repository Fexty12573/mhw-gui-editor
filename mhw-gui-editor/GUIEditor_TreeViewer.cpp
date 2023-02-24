#include "pch.h"
#include "GUIEditor.h"


void GUIEditor::render_tree_viewer() {
    ImGui::Begin("Tree Viewer");

    if (ImGui::TreeNodeEx("Animations", ImGuiTreeNodeFlags_SpanAvailWidth)) {
        for (auto& animation : m_file.m_animations) {
            render_animation(animation);
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("Objects", ImGuiTreeNodeFlags_SpanAvailWidth)) {
        for (auto& object : m_file.m_objects) {
            render_object(object);
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("Sequences", ImGuiTreeNodeFlags_SpanAvailWidth)) {
        for (auto& sequence : m_file.m_sequences) {
            render_sequence(sequence);
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("ObjectSequences", ImGuiTreeNodeFlags_SpanAvailWidth)) {
        for (auto& obj_sequence : m_file.m_obj_sequences) {
            render_obj_sequence(obj_sequence);
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("InitParams", ImGuiTreeNodeFlags_SpanAvailWidth)) {
        for (auto& init_param : m_file.m_init_params) {
            render_init_param(init_param);
        }

        ImGui::TreePop();
    }

    bool open = ImGui::TreeNodeEx("Params", ImGuiTreeNodeFlags_SpanAvailWidth);
    bool open_add_param_popup = false;

    if (ImGui::BeginPopupContextItem("Params")) {
        if (ImGui::MenuItem("Add New (WIP)")) {
            open_add_param_popup = true;
        }

        ImGui::EndPopup();
    }

    if (open) {
        for (auto& param : m_file.m_params) {
            render_param(param);
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("Instances", ImGuiTreeNodeFlags_SpanAvailWidth)) {
        for (auto& inst : m_file.m_instances) {
            render_instance(inst);
        }

        ImGui::TreePop();
    }

    ImGui::End();
}
