#include "pch.h"
#include "GUIEditor.h"
#include "RichText.h"
#include "crc32.h"

#include <imgui_stdlib.h>


void GUIEditor::render_obj_sequence(GUIObjectSequence& objseq, ObjectType source_object, u32 object_id, const GUISequence* seq) {
    struct AddParamInfo {
        u32 ObjSeqIndex;
        ObjectType SourceObjectType;
        u32 SourceObjectId;
        std::shared_ptr<ObjectInfo> ObjectInfo;
        bool AddKeyframes;

        GUIParam Param;
        bool Added = false;
    };

    constexpr u32 u32_step = 1;
    constexpr u32 u32_fast_step = 10;
    const auto add_param_func = [this, u32_step, u32_fast_step](std::any& user_data)  {
        auto& info = std::any_cast<AddParamInfo&>(user_data);

        if (info.SourceObjectType != ObjectType::None) {
            const auto& params = info.ObjectInfo->Params;
            if (ImGui::BeginCombo("Presets", params.contains(info.Param.Name) ? info.Param.Name.c_str() : "----")) {
                for (const auto& [name, type] : params) {
                    if (ImGui::Selectable(name.c_str(), name == info.Param.Name)) {
                        info.Param.Name = name;
                        info.Param.Type = type;

                        info.Param.perform_value_operation(
                            [](auto, auto& p) { p.Values = std::vector<u8>(p.ValueCount, 0); },
                            [](auto, auto& p) { p.Values = std::vector<u32>(p.ValueCount, 0); },
                            [](auto, auto& p) { p.Values = std::vector<f32>(p.ValueCount, 0.0f); },
                            [](auto, auto& p) { p.Values = std::vector<MtVector4>(p.ValueCount, MtVector4{}); },
                            [](auto, auto& p) { p.Values = std::vector<std::string>(p.ValueCount, ""); },
                            false
                        );
                    }
                }

                ImGui::EndCombo();
            }
        }

        if (ImGui::RichTextCombo("Type", reinterpret_cast<u8*>(&info.Param.Type), ParamTypeNames, 0xFFB0C94E)) {
            info.Param.perform_value_operation(
                [](auto, auto& p) { p.Values = std::vector<u8>(p.ValueCount, 0); },
                [](auto, auto& p) { p.Values = std::vector<u32>(p.ValueCount, 0); },
                [](auto, auto& p) { p.Values = std::vector<f32>(p.ValueCount, 0.0f); },
                [](auto, auto& p) { p.Values = std::vector<MtVector4>(p.ValueCount, MtVector4{}); },
                [](auto, auto& p) { p.Values = std::vector<std::string>(p.ValueCount, ""); },
                false
            );

            ImGui::EndCombo();
        }

        if (ImGui::InputScalar("Count", ImGuiDataType_U8, &info.Param.ValueCount, &u32_step, &u32_fast_step)) {
            info.Param.perform_value_operation(
                [](auto v, auto& p) { v->resize(p.ValueCount, 0); },
                [](auto v, auto& p) { v->resize(p.ValueCount, 0); },
                [](auto v, auto& p) { v->resize(p.ValueCount, 0.0f); },
                [](auto v, auto& p) { v->resize(p.ValueCount, MtVector4{}); },
                [](auto v, auto& p) { v->resize(p.ValueCount, ""); }
            );
        }

        ImGui::InputText("Name", &info.Param.Name);

        ImGui::NewLine();

        if (ImGui::Button("Add")) {
            GUIParam& p = info.Param;

            p.IsColorParam = p.Name.contains("Color") && p.Name != "ColorScale";
            p.IsColorParam |= p.Name == "RGB";
            p.IsColorParam &= p.Type == ParamType::VECTOR;

            p.NameCRC = crc::crc32(p.Name.c_str(), p.Name.size());
            p.KeyIndex = m_file.m_keys.size();
            p.ParentID = info.SourceObjectId != -1 ? info.SourceObjectId : 0;

            for (auto i = 0u; i < p.ValueCount; ++i) {
                m_file.insert_key({ .Data = {.Full = 0 } });
            }

            auto& seq = m_file.m_obj_sequences[info.ObjSeqIndex];

            p.Index = seq.ParamIndex + seq.ParamNum;
            m_file.insert_param(p, p.Index);

            update_indices();

            info.Added = true;
            seq.ParamNum += 1;
            seq.ParamIndex -= 1; // TODO: This gets incremented by the insert_param function, should probably be changed

            return true;
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel")) {
            return true;
        }

        ImGui::SameLine();

        ImGui::Checkbox("Add Keyframes", &info.AddKeyframes);

        return false;
    };

    ImGui::PushID("ObjectSequence");
    ImGui::PushID(objseq.Index);

    bool open = ImGui::RichTextTreeNode("ObjSeq", objseq.get_preview(objseq.Index, seq ? seq->Name : ""));

    if (ImGui::RT::BeginPopupContextItem("ObjSeq")) {
        if (ImGui::MenuItem("Add Param")) {
            m_generic_popup_queue.emplace("Add New Param", [this, add_param_func](std::any& user_data) {
                const bool result = add_param_func(user_data);
                const auto& info = std::any_cast<AddParamInfo&>(user_data);

                if (result && info.Added) {
                    auto& seq = m_file.m_obj_sequences[info.ObjSeqIndex];

                    if (seq.ParamNum == 0) {
                        seq.ParamIndex = info.Param.Index;
                    }
                }

                return result;
            }, AddParamInfo{
                .ObjSeqIndex = objseq.Index,
                .SourceObjectType = source_object,
                .SourceObjectId = object_id,
                .ObjectInfo = source_object != ObjectType::None ? m_owner->get_object_info2().at(source_object) : nullptr,
                .AddKeyframes = false,
                .Param = { .Type = ParamType::UNKNOWN }
            });
        }

        ImGui::EndPopup();
    }

    if (open) {
        ImGui::InputScalar("Attribute", ImGuiDataType_U16, &objseq.Attr, &u32_step, &u32_fast_step);
        ImGui::InputScalar("InitParam Count", ImGuiDataType_U8, &objseq.InitParamNum, &u32_step, &u32_fast_step);
        ImGui::InputScalar("Param Count", ImGuiDataType_U8, &objseq.ParamNum, &u32_step, &u32_fast_step);
        ImGui::InputScalar("Loop Start", ImGuiDataType_S16, &objseq.LoopStart, &u32_step, &u32_fast_step);
        ImGui::InputScalar("Frame Count", ImGuiDataType_S16, &objseq.FrameCount, &u32_step, &u32_fast_step);
        ImGui::InputScalar("InitParam Index", ImGuiDataType_U32, &objseq.InitParamIndex, &u32_step, &u32_fast_step);
        ImGui::InputScalar("Param Index", ImGuiDataType_U32, &objseq.ParamIndex, &u32_step, &u32_fast_step);

        if (objseq.InitParamNum > 0) {
            if (ImGui::TreeNodeEx("InitParams", ImGuiTreeNodeFlags_SpanAvailWidth)) {
                auto& initparams = m_file.m_init_params;
                const u64 max = std::min(objseq.InitParamIndex + static_cast<u64>(objseq.InitParamNum), initparams.size());

                for (auto i = objseq.InitParamIndex; i < max; ++i) {
                    render_init_param(initparams[i], source_object);
                }

                if (objseq.InitParamIndex + static_cast<u64>(objseq.InitParamNum) > m_file.m_init_params.size()) {
                    ImGui::TextColored({ 1.0f, 1.0f, 0.2f, 1.0f }, "Warning: InitParamNum goes out of bounds!");
                }

                ImGui::TreePop();
            }
        }

        if (objseq.ParamNum > 0) {
            open = ImGui::TreeNodeEx("Params", ImGuiTreeNodeFlags_SpanAvailWidth);

            if (ImGui::BeginPopupContextItem()) {
                if (ImGui::MenuItem("Add Param")) {
                    m_generic_popup_queue.emplace("Add New Param", add_param_func, AddParamInfo{
                        .ObjSeqIndex = objseq.Index,
                        .SourceObjectType = source_object,
                        .SourceObjectId = object_id,
                        .ObjectInfo = source_object != ObjectType::None ? m_owner->get_object_info2().at(source_object) : nullptr,
                        .AddKeyframes = false,
                        .Param = { .Type = ParamType::UNKNOWN }
                    });
                }

                ImGui::EndPopup();
            }

            if (open) {
                auto& params = m_file.m_params;
                const u64 max = std::min(objseq.ParamIndex + static_cast<u64>(objseq.ParamNum), params.size());

                for (auto i = objseq.ParamIndex; i < max; ++i) {
                    render_param(params[i], source_object);
                }

                if (objseq.ParamIndex + static_cast<u64>(objseq.ParamNum) > m_file.m_init_params.size()) {
                    ImGui::TextColored({ 1.0f, 1.0f, 0.2f, 1.0f }, "Warning: ParamNum goes out of bounds!");
                }

                ImGui::TreePop();
            }
        }

        ImGui::TreePop();
    }

    ImGui::PopID();
    ImGui::PopID();
}
