#include "pch.h"
#include "GUIEditor.h"
#include "HrException.h"
#include "RichText.h"
#include "GroupPanel.h"
#include "CurveEditor.h"
#include "App.h"
#include "IconFontAwesome.h"
#include "crc32.h"
#include "Console.h"

#include "imgui-notify/tahoma.h"

#include <fmt/format.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include <imgui.h>
#include <ShObjIdl.h>
#include <wrl.h>
#include <ranges>
#include <nlohmann/json.hpp>

GUIEditor::GUIEditor(Editor* owner, const std::filesystem::path& file) : m_owner(owner) {
    m_app = m_owner->get_app();

    BinaryReader reader(file);
    m_file.load_from(reader);
    m_file_path = file;

    const auto& settings = owner->get_settings();
    if (!settings.ChunkPath.empty() || !settings.NativePath.empty()) {
        m_file.load_resources(
            settings.ChunkPath,
            settings.NativePath,
            settings.ArcfsPath,
            m_app->get_device().Get(),
            m_app->get_context().Get()
        );
    }

    m_file.run_data_usage_analysis(true);

    update_indices();
}

void GUIEditor::render(u32 dockspace_id) {
    if (!m_generic_popup_queue.empty()) {
        ImGui::PushID("GenericPopupQueue");

        auto& popup = m_generic_popup_queue.front();
        if (!ImGui::IsPopupOpen(popup.Title.c_str())) {
            ImGui::OpenPopup(popup.Title.c_str());
        }

        if (ImGui::BeginPopupModal(popup.Title.c_str(), nullptr, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize)) {
            if (popup.DrawCallback(popup.UserData)) {
                m_generic_popup_queue.pop();
            }

            ImGui::EndPopup();
        }

        ImGui::PopID();
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(43.f / 255.f, 43.f / 255.f, 43.f / 255.f, 100.f / 255.f));
    ImGui::RenderNotifications();
    ImGui::PopStyleVar(1);
    ImGui::PopStyleColor(1);
}

void GUIEditor::save_file() {
    if (m_file_path.empty()) {
        return;
    }

    BinaryWriter writer(m_file_path);
    m_file.save_to(writer, m_owner->get_settings());

    ImGuiToast toast(ImGuiToastType_Success);
    toast.set_title("Saved");
    toast.set_type(ImGuiToastType_Success);
    toast.set_content("Successfully saved file to %s", m_file_path.string().c_str());
    ImGui::InsertNotification(toast);
}

void GUIEditor::save_file_as(const std::filesystem::path& path) {
    BinaryWriter writer(path);

    m_file.save_to(writer, m_owner->get_settings());
    m_file_path = path;

    ImGuiToast toast(ImGuiToastType_Success);
    toast.set_title("Saved");
    toast.set_type(ImGuiToastType_Success);
    toast.set_content("Successfully saved file to %s", m_file_path.string().c_str());
    ImGui::InsertNotification(toast);
}

void GUIEditor::render_object_editor() {
    if (m_selected_object == -1) {
        return;
    }

    if (!ImGui::Begin("Object Editor", &m_object_editor_visible)) {
        return;
    }

    ImGui::End();
}

ParamValidationResult GUIEditor::is_valid_param(ParamType type, const std::string& name, ObjectType source_object) const {
    if (source_object == ObjectType::None) {
        return { .CorrectType = type };
    }

    bool invalid_name = false;
    bool invalid_type = false;
    ParamType correct_type = type;

    const auto& object_info_map = m_owner->get_object_info();
    const std::string source_object_name = enum_to_string(source_object);
    if (source_object != ObjectType::None) {
        if (object_info_map.contains(source_object_name)) {
            const auto& object_info = object_info_map.at(source_object_name);
            if (object_info->Params.contains(name)) {
                if (object_info->Params.at(name) != type) {
                    invalid_type = true;
                    correct_type = object_info->Params.at(name);
                }
            } else {
                invalid_name = true;
            }
        }
    }

    return { invalid_name, invalid_type, correct_type };
}

void GUIEditor::update_indices() {
    auto update_indices = [this](auto& list) {
        for (auto i = 0u; i < list.size(); ++i) {
            list[i].Index = i;
        }
    };

    update_indices(m_file.m_animations);
    update_indices(m_file.m_objects);
    update_indices(m_file.m_sequences);
    update_indices(m_file.m_obj_sequences);
    update_indices(m_file.m_init_params);
    update_indices(m_file.m_params);
    update_indices(m_file.m_instances);
    update_indices(m_file.m_keys);
}

void GUIEditor::open_animation_editor() {
    if (m_animation_editor_first) {
        m_animation_editor_first = false;
    }
    
    m_animation_editor_visible = !m_animation_editor_visible;
}
