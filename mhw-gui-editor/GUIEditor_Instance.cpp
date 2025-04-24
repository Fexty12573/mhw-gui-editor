#include "pch.h"
#include "GUIEditor.h"
#include "RichText.h"

#include <imgui_stdlib.h>


void GUIEditor::render_instance(GUIInstance& inst) {
    constexpr u32 u32_step = 1;
    constexpr u32 u32_fast_step = 10;

    ImGui::PushID("Instance");
    ImGui::PushID(inst.Index);

    if (ImGui::RichTextTreeNode("Inst", inst.get_preview(inst.Index))) {
        ImGui::InputScalar("ID", ImGuiDataType_U32, &inst.ID, &u32_step, &u32_fast_step);
        ImGui::InputInt("Child Index", &inst.ChildIndex);
        ImGui::InputInt("Next Index", &inst.NextIndex);
        ImGui::InputText("Name", &inst.Name);
        if (ImGui::BeginCombo("Type", ObjectTypeNames.at(inst.Type))) {
            for (const auto& [type, name] : ObjectTypeNames) {
                if (ImGui::Selectable(name, inst.Type == type)) {
                    inst.Type = type;
                }
            }

            ImGui::EndCombo();
        }

        if (inst.ChildIndex != -1) {
            auto instance = &m_file.m_instances[inst.ChildIndex];
            render_instance(*instance);

            while (instance->NextIndex != -1) {
                instance = &m_file.m_instances[instance->NextIndex];
                render_instance(*instance);
            }
        }

        if (inst.InitParamNum > 0) {
            if (ImGui::TreeNodeEx("InitParams", ImGuiTreeNodeFlags_SpanAvailWidth)) {
                const u64 max = std::min(static_cast<u64>(inst.InitParamIndex + inst.InitParamNum), m_file.m_init_params.size());
                for (auto i = inst.InitParamIndex; i < max; ++i) {
                    render_init_param(m_file.m_init_params[i]);
                }

                if (inst.InitParamIndex + static_cast<u64>(inst.InitParamNum) > m_file.m_init_params.size()) {
                    ImGui::TextColored({ 1.0f, 1.0f, 0.2f, 1.0f }, "Warning: InitParamNum goes out of bounds!");
                }

                ImGui::TreePop();
            }
        }

        const auto param_idx = m_file.is_mhgu()
            ? m_file.m_header_mhgu.instExeParamOffset + inst.Index
            : m_file.m_header.instanceParamEntryStartIndex + inst.Index;
        if (param_idx < m_file.m_params.size()) {
            render_param(m_file.m_params[param_idx]);
        }

        ImGui::TreePop();
    }

    ImGui::PopID();
    ImGui::PopID();
}
