#include "pch.h"
#include "App.h"
#include "GUIEditor.h"
#include "IconFontAwesome.h"

#include <imgui_stdlib.h>


void GUIEditor::render_resource_manager() {
    ImGui::Begin("Resource Manager");

    {
        static constexpr std::array ResourceTypes = {
            "Texture",
            "Message",
            "Resource",
            "GeneralResource"
        };

        static std::string name;
        static u32 id = 0;
        static int type = 0;
        static ObjectType obj_type = ObjectType::None;

        if (ImGui::Button("Add New Resource")) {
            ImGui::OpenPopup("Add New Resource##popup");

            name = "";
            id = m_file.get_unused_id(m_file.m_textures);
            type = 0;
        }

        if (ImGui::BeginPopupModal("Add New Resource##popup")) {
            ImGui::Combo("Type", &type, ResourceTypes.data(), ResourceTypes.size());
            if (type == 0) {
                ImGui::InputText("Name", &name);
            } else if (type == 3) {
                if (ImGui::BeginCombo("Object Type", enum_to_string(obj_type))) {
                    for (const auto& [key, value] : ObjectTypeNames) {
                        if (ImGui::Selectable(value, obj_type == key)) {
                            obj_type = key;
                        }
                    }

                    ImGui::EndCombo();
                }
            }

            ImGui::InputScalar("ID", ImGuiDataType_U32, &id, nullptr, nullptr, "%u");
            ImGui::Separator();

            if (ImGui::Button("Add")) {
                switch (type) {
                case 0: m_file.insert_texture({ .ID = id, .Name = name }); break;
                case 1: m_file.m_messages.push_back({ .ID = id }); break;
                case 2: m_file.m_resources.push_back({ .ID = id }); break;
                case 3: m_file.m_general_resources.push_back({ .ID = id }); break;
                default: break;
                }

                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

    constexpr ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
    constexpr u32 u32_step = 1;
    constexpr u32 u32_fast_step = 10;

    for (auto i = 0; i < m_file.m_textures.size(); ++i) {
        ImGuiTreeNodeFlags node_flags = flags;

        auto& tex = m_file.m_textures[i];

        if (m_selected_texture == i) {
            node_flags |= ImGuiTreeNodeFlags_Selected;
        }

        const bool open = ImGui::TreeNodeEx(std::format("{} | {}", tex.Name, tex.Path).c_str(), node_flags);
        if (ImGui::IsItemClicked()) {
            m_selected_texture = i;
        }

        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Delete")) {
                m_popup_queue.emplace("Are you sure?", "Are you sure you want to delete this texture?", PopupType::YesCancel,
                    [this](auto result, const std::any& data) {
                    if (result == YesNoCancelPopupResult::Yes) {
                        const int index = std::any_cast<int>(data);
                        m_file.m_textures.erase(m_file.m_textures.begin() + index);
                    }
                }, i);
            }

            ImGui::EndPopup();
        }

        if (open) {
            ImGui::InputScalar("ID", ImGuiDataType_U32, &tex.ID, &u32_step, &u32_fast_step);

            ImGui::InputScalarN("Texture Rect (XYWH)", ImGuiDataType_U16, &tex.Left, 4, &u32_step, &u32_fast_step);

            ImGui::InputFloat4("Clamp", &tex.Clamp[0], "%.3f");
            ImGui::InputFloat2("InvSize", &tex.InvSize.x, "%.8f");

            ImGui::InputText("Name", &tex.Name);
            ImGui::InputText("Path", &tex.Path);
            ImGui::SameLine();
            if (ImGui::Button("...")) {
                if (m_settings.ChunkPath.empty()) {
                    m_error_popup_select_file_open = true;
                } else {
                    const auto path = open_file_dialog(L"Select Texture", { {L"TEX Files", L"*.tex"} }, L"tex");
                    if (!path.empty()) {

                        auto relpath = std::filesystem::relative(path, m_settings.ChunkPath);
                        if (relpath.empty()) {
                            relpath = std::filesystem::relative(path, m_settings.NativePath);
                        }

                        tex.Path = relpath.replace_extension().generic_string();

                        BinaryReader reader(path);
                        tex.RenderTexture.load_from(reader, m_owner->get_device().Get(), m_owner->get_context().Get());
                    }
                }
            }

            if (ImGui::Button(ICON_FA_ARROW_ROTATE_RIGHT " Reload")) {
                if (m_settings.ChunkPath.empty() && m_settings.NativePath.empty()) {
                    m_error_popup_select_file_open = true;
                } else {
                    auto path = std::filesystem::path(m_settings.NativePath) / tex.Path;
                    path.replace_extension("tex");

                    if (!exists(path)) {
                        path = std::filesystem::path(m_settings.ChunkPath) / tex.Path;
                        path.replace_extension("tex");
                    }

                    try {
                        BinaryReader reader(path);
                        tex.RenderTexture.load_from(reader, m_owner->get_device().Get(), m_owner->get_context().Get());
                    } catch (...) {
                        m_error_popup_message = "Failed to load texture";
                        m_error_popup_open = true;
                    }
                }
            }

            ImGui::SameLine();
            if (ImGui::Button("Populate dimensions from Texture") && tex.RenderTexture.is_valid()) {
                tex.Left = 0;
                tex.Top = 0;
                tex.Width = static_cast<u16>(tex.RenderTexture.get_width());
                tex.Height = static_cast<u16>(tex.RenderTexture.get_height());
                tex.InvSize.x = 1.0f / tex.Width;
                tex.InvSize.y = 1.0f / tex.Height;
            }

            ImGui::TreePop();
        }
    }

    ImGui::End();
}
