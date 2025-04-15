#include "pch.h"
#include "GUIEditor.h"
#include "RichText.h"
#include "imgui_neo_sequencer.h"

#include <imgui_stdlib.h>
#include <algorithm>


void GUIEditor::render_object(GUIObject& obj, u32 seq_count, GUIAnimation* parent_anim) {
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
            render_object(*object, seq_count, parent_anim);

            while (object->NextIndex != -1) {
                object = &m_file.m_objects[object->NextIndex];
                render_object(*object, seq_count, parent_anim);
            }
        }

        if (obj.Type == ObjectType::cGUIObjTexture) {
            const GUIInitParam* texture = nullptr, * rect = nullptr, * size = nullptr;
            const u64 max = std::min(static_cast<u64>(obj.InitParamIndex + obj.InitParamNum), m_file.m_init_params.size());
            for (auto i = obj.InitParamIndex; i < max; ++i) {
                if (m_file.m_init_params[i].Name == "Texture" && m_file.m_init_params[i].Type == ParamType::TEXTURE) {
                    texture = &m_file.m_init_params[i];
                } else if (m_file.m_init_params[i].Name == "mTextureRect") {
                    rect = &m_file.m_init_params[i];
                } else if (m_file.m_init_params[i].Name == "Size") {
                    size = &m_file.m_init_params[i];
                }
            }

            if (texture && rect && size) {
                if (ImGui::TreeNodeEx("Texture Preview", ImGuiTreeNodeFlags_SpanAvailWidth)) {
                    const auto& tex = std::ranges::find_if(
                        m_file.m_textures,
                        [&texture](const GUITexture& tex0) {
                            return tex0.ID == texture->Value32;
                        }
                    );

                    if (tex->RenderTexture.is_valid()) {
                        ImGui::Image(
                            tex->RenderTexture.get_view().Get(),
                            { size->ValueVector.x, size->ValueVector.y },
                            { rect->ValueVector.x, rect->ValueVector.y },
                            { rect->ValueVector.z, rect->ValueVector.w }
                        );
                    } else {
                        ImGui::TextColored({ 1.0f, 0.2f, 0.2f, 1.0f }, "Error: Couldn't load texture!");
                        ImGui::Text("Make sure the file exists and you have the correct chunk path set under Tools -> Options");
                    }

                    ImGui::TreePop();
                }
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

        if (parent_anim) {
            static int current_frame = 0;
            static int start_frame = 0;
            static int end_frame = 100;
            auto& params = m_file.m_params;

            if (ImGui::TreeNodeEx("ObjectSequences (Timeline)", ImGuiTreeNodeFlags_SpanAvailWidth)) {
                const u64 max = std::min(static_cast<u64>(obj.ObjectSequenceIndex + seq_count), m_file.m_obj_sequences.size());
                for (auto i = obj.ObjectSequenceIndex; i < max; ++i) {
                    const int sequence_index = i - obj.ObjectSequenceIndex + parent_anim->SequenceIndex;
                    GUIObjectSequence& objseq = m_file.m_obj_sequences[i];
                    GUISequence& seq = m_file.m_sequences[sequence_index];

                    if (ImGui::BeginNeoSequencer(m_file.m_sequences[sequence_index].Name.c_str(), &current_frame, &start_frame, (int*)&seq.FrameCount)) {
                        const u64 max_param = std::min(objseq.ParamIndex + static_cast<u64>(objseq.ParamNum), params.size());

                        for (auto i = objseq.ParamIndex; i < max_param; ++i) {
                            GUIParam& param = params[i];
                            std::span keys(m_file.m_keys.begin() + param.KeyIndex, param.ValueCount);
                            std::vector<int> key_frames;
                            std::ranges::transform(
                                keys,
                                std::back_inserter(key_frames),
                                [](const GUIKey& key) {
                                    return key.Data.Bitfield.Frame;
                                }
                            );

                            if (ImGui::BeginNeoTimeline(param.Name.c_str(), key_frames)) {
                                ImGui::EndNeoTimeLine();
                            }

                            for (auto i = 0u; i < keys.size(); ++i) {
                                keys[i].Data.Bitfield.Frame = key_frames[i];
                            }
                        }

                        ImGui::EndNeoSequencer();
                    }
                }

                ImGui::TreePop();
            }
        }

        ImGui::TreePop();
    }

    ImGui::PopID();
    ImGui::PopID();
}
