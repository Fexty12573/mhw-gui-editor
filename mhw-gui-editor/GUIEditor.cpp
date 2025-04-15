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

GUIEditor::GUIEditor(App* owner) : m_owner(owner) {
    HR_INIT(S_OK);
    HR_ASSERT(CoInitializeEx(nullptr, COINIT_MULTITHREADED));
    
    add_menu_item("File", { ICON_FA_FILE " Open", "Ctrl+O", [](GUIEditor* e) { e->open_file(); } });
    add_menu_item("File", { ICON_FA_FLOPPY_DISK " Save", "Ctrl+S", [](GUIEditor* e) { e->save_file(); } });
    add_menu_item("File", { "Save As...", "Ctrl+Shift+S", [](GUIEditor* e) { e->save_file_as(); }});
    add_menu_item("View", { "Animation Editor", "Ctrl+Shift+A", [this](GUIEditor* e) {
        m_animation_editor_visible = true;
        
        
    } });

    add_menu_item("Edit", { ICON_FA_ARROW_RIGHT " Go To...", "Ctrl+G", [](GUIEditor* e) {
        e->m_generic_popup_queue.emplace("Go To...", [e](const std::any& data) {
            auto& path = std::any_cast<std::string&>(data);
            ImGui::InputText("Path", &path);
            ImGui::NewLine();

            if (ImGui::Button("Go")) {
                ImGui::CloseCurrentPopup();
                //e->m_selected_object = e->m_file.find_object(path);
                return true;
            }

            if (ImGui::Button("Cancel")) {
                ImGui::CloseCurrentPopup();
                return true;
            }

            return false;
        }, std::string{});
    }});

    m_settings.load();
    m_theme_manager.set_theme_directory("./themes");
    m_theme_manager.refresh();

    m_theme_manager.apply_style(m_settings.Theme.empty() ? "Default" : m_settings.Theme);

    const auto fonts = ImGui::GetIO().Fonts;

    ImFontConfig config;
    config.MergeMode = true;
    config.GlyphMinAdvanceX = 16.0f;
    config.GlyphMaxAdvanceX = 16.0f;
    static constexpr ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };

    fonts->AddFontFromFileTTF("./fonts/CascadiaMono.ttf", 16.0f);
    fonts->AddFontFromFileTTF("./fonts/Font Awesome 6 Free-Solid-900.otf", 16.0f, &config, icon_ranges);
    fonts->Build();

    config.FontDataOwnedByAtlas = false;
    fonts->AddFontFromMemoryTTF((void*)tahoma, sizeof tahoma, 17.0f, &config);
    fonts->Build();
    ImGui::MergeIconsWithLatestFont(16.0f);

    nlohmann::json object_info;

    try {
        std::ifstream("./data/cGUIObject.json") >> object_info;
    } catch (const std::exception& e) {
        spdlog::error("Failed to load cGUIObject.json: {}", e.what());
    }

    for (const auto& [key, value] : object_info.items()) {
        ObjectInfo info{};
        info.Name = key;
        info.Type = ObjectTypeNamesReversed.at(info.Name);

        for (const auto& field : value["Fields"]) {
            info.Params[field.value("Name", "N/A")] = static_cast<ParamType>(field.value("Type", 0));
        }

        const auto p_info = std::make_shared<ObjectInfo>(std::move(info));
        m_object_info[key] = p_info;
        m_object_info2[p_info->Type] = p_info;
    }
}

void GUIEditor::add_menu_item(const std::string& menu, const MenuItem& item) {
    for (auto& m : m_menu_items) {
        if (m.Name == menu) {
            m.Items.push_back(item);
            return;
        }
    }

    m_menu_items.emplace_back(menu, std::vector{ item });
}

void GUIEditor::render(u32 dockspace_id) {
    if (m_first_render) {
        m_first_render = false;

        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);

        const ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(
            dockspace_id,
            ImGuiDir_Right,
            0.25f,
            nullptr, 
            &dockspace_id
        );
        ImGuiID dock_id_bottom_left = ImGui::DockBuilderSplitNode(
            dockspace_id,
            ImGuiDir_Down,
            0.4f,
            nullptr,
            &dockspace_id
        );
        const ImGuiID dock_id_bottom_right = ImGui::DockBuilderSplitNode(
            dock_id_bottom_left,
            ImGuiDir_Right,
            0.4f,
            nullptr,
            &dock_id_bottom_left
        );

        ImGui::DockBuilderDockWindow("Overview", dock_id_right);
        ImGui::DockBuilderDockWindow("Tree Viewer", dockspace_id);
        ImGui::DockBuilderDockWindow("Resource Manager", dock_id_bottom_left);
        ImGui::DockBuilderDockWindow("Texture Viewer", dock_id_bottom_right);
    }

    bool open_options_window = false;
    bool open_extract_window = false;

    if (ImGui::BeginMainMenuBar()) {
        for (const auto& [menu, items] : m_menu_items) {
            if (ImGui::BeginMenu(menu.c_str())) {
                for (const auto& item : items) {
                    if (ImGui::MenuItem(item.Name, item.Shortcut)) {
                        item.Callback(this);
                    }
                }

                ImGui::EndMenu();
            }
        }

        if (ImGui::BeginMenu("Tools")) {
            if (ImGui::BeginMenu(ICON_FA_PALETTE " Themes")) {
                if (ImGui::MenuItem("Refresh")) {
                    m_theme_manager.refresh();
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Default")) {
                    m_theme_manager.apply_default_style();
                    m_settings.Theme = "";
                    m_settings.save();
                }

                for (const auto& name : m_theme_manager.get_styles() | std::views::keys) {
                    if (ImGui::MenuItem(name.c_str())) {
                        m_theme_manager.apply_style(name);
                        m_settings.Theme = name;
                        m_settings.save();
                    }
                }

                ImGui::EndMenu();
            }

            if (ImGui::MenuItem(ICON_FA_GEARS " Options")) {
                open_options_window = true;
            }

            if (ImGui::MenuItem(ICON_FA_FILE_PEN " Extract GUI Object Data")) {
                dump_object_data();
            }

            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Extracts the InitParams of all GUI objects found in GUI files in the selected directory.");
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    render_overview();
    render_tree_viewer();
    render_resource_manager();
    render_texture_viewer();
    render_object_editor();

    if (open_options_window) {
        ImGui::OpenPopup("Options##OptionsWindow");
        m_options_menu_open = true;
    }

    ImGui::SetNextWindowSize(ImVec2(900, 300), ImGuiCond_Once);
    if (ImGui::BeginPopupModal("Options##OptionsWindow", &m_options_menu_open, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize)) {

        ImGui::BeginGroupPanel(ICON_FA_BARS " General");

        ImGui::InputText("Chunk Directory", &m_settings.ChunkPath);
        ImGui::SameLine();
        if (ImGui::Button("...##ChunkDir")) {
            select_chunk_dir();
        }

        ImGui::InputText("NativePC Directory", &m_settings.NativePath);
        ImGui::SameLine();
        if (ImGui::Button("...##NativeDir")) {
            select_native_dir();
        }

        ImGui::EndGroupPanel();
        ImGui::BeginGroupPanel(ICON_FA_BOLT " Optimization");

        ImGui::Text("Allow Multiple Referenes to data");

        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("Prevents creating separate data entries for identical values.");
            ImGui::Text("Turning this off makes editing with a hex editor easier but increases filesize.");
            ImGui::EndTooltip();
        }

        ImGui::Indent();

        ImGui::Checkbox("KeyValue8", &m_settings.AllowMultipleKV8References);
        ImGui::Checkbox("KeyValue32", &m_settings.AllowMultipleKV32References);
        ImGui::Checkbox("KeyValue128", &m_settings.AllowMultipleKV128References);

        ImGui::Unindent();
        ImGui::EndGroupPanel();
        ImGui::BeginGroupPanel(ICON_FA_WRENCH " Utilities");

        if (ImGui::Checkbox("Auto-Adjust Keyframes", &m_settings.AutoAdjustKeyFrames)) {
            if (m_settings.AutoAdjustKeyFrames) {
                m_popup_queue.emplace("Are you sure?", 
                    "Turning on this option and decreasing the ValueCount on a Param will cause Keyframes to be deleted.\nThis cannot be undone.",
                    PopupType::OkCancel, [this](YesNoCancelPopupResult result, auto) {
                    if (result == YesNoCancelPopupResult::Cancel) {
                        m_settings.AutoAdjustKeyFrames = false;
                    }
                });
            }
        }

        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("Automatically adjusts the number of keyframes to the number of values a Param has.");
            ImGui::EndTooltip();
        }

        ImGui::EndGroupPanel();

        ImGui::NewLine();
        if (ImGui::Button("OK")) {
            m_settings.save();
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        if (ImGui::Button("Apply")) {
            m_settings.save();
        }

        ImGui::EndPopup();
    }

    if (m_error_popup_select_file_open) {
        ImGui::OpenPopup("Error##SelectFile");
        m_error_popup_select_file_open = false;
    }

    if (ImGui::BeginPopup("Error##SelectFile")) {
        ImGui::Text("Error: File paths have to be relative to the chunk directory.\nSet your chunk directory first in the options menu.");
        if (ImGui::Button("Ok")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    if (m_error_popup_open) {
        ImGui::OpenPopup("Error##GenericError");
        m_error_popup_open = false;
    }

    if (ImGui::BeginPopup("Error##GenericError")) {
        ImGui::Text("%s", m_error_popup_message.c_str());
        if (ImGui::Button("Ok")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    if (!m_popup_queue.empty()) {
        ImGui::PushID("YesNoCancelPopupQueue");

        auto& popup = m_popup_queue.front();
        if (!ImGui::IsPopupOpen(popup.Title.c_str())) {
            ImGui::OpenPopup(popup.Title.c_str());
        }

        if (ImGui::BeginPopupModal(m_popup_queue.front().Title.c_str(), nullptr, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("%s", popup.Message.c_str());

            bool new_line = false;

            if (popup.Type & PopupType::Ok) {
                new_line = true;

                if (ImGui::Button("Ok")) {
                    popup.Callback(YesNoCancelPopupResult::Ok, popup.UserData);

                    m_popup_queue.pop();
                    ImGui::CloseCurrentPopup();
                }
            }

            if (popup.Type & PopupType::Yes) {
                if (new_line) {
                    ImGui::SameLine();
                } else {
                    new_line = true;
                }

                if (ImGui::Button("Yes")) {
                    popup.Callback(YesNoCancelPopupResult::Yes, popup.UserData);

                    m_popup_queue.pop();
                    ImGui::CloseCurrentPopup();
                }
            }

            if (popup.Type & PopupType::No) {
                if (new_line) {
                    ImGui::SameLine();
                } else {
                    new_line = true;
                }

                if (ImGui::Button("No")) {
                    popup.Callback(YesNoCancelPopupResult::No, popup.UserData);

                    m_popup_queue.pop();
                    ImGui::CloseCurrentPopup();
                }
            }

            if (popup.Type & PopupType::Cancel) {
                if (new_line) {
                    ImGui::SameLine();
                }

                if (ImGui::Button("Cancel")) {
                    popup.Callback(YesNoCancelPopupResult::Cancel, popup.UserData);

                    m_popup_queue.pop();
                    ImGui::CloseCurrentPopup();
                }
            }

            ImGui::EndPopup();
        }

        ImGui::PopID();
    }

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

void GUIEditor::render_object_editor() {
    if (m_selected_object == -1) {
        return;
    }

    if (!ImGui::Begin("Object Editor", &m_object_editor_visible)) {
        return;
    }

    ImGui::End();
}

void GUIEditor::open_file() {
    HR_INIT(S_OK);

    Microsoft::WRL::ComPtr<IFileOpenDialog> dialog;
    HR_ASSERT(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&dialog)));

    constexpr COMDLG_FILTERSPEC filters[] = {
        { L"MHW GUI Files", L"*.gui" }
    };

    HR_ASSERT(dialog->SetFileTypes(ARRAYSIZE(filters), filters));
    HR_ASSERT(dialog->SetDefaultExtension(L"gui"));
    HR_ASSERT(dialog->SetOptions(
        FOS_STRICTFILETYPES | FOS_PATHMUSTEXIST | FOS_FILEMUSTEXIST
    ));
    
    if (dialog->Show(nullptr) == HRESULT_FROM_WIN32(ERROR_CANCELLED)) {
        return;
    }

    Microsoft::WRL::ComPtr<IShellItem> item;
    HR_ASSERT(dialog->GetResult(&item));
    
    LPWSTR file_name;
    HR_ASSERT(item->GetDisplayName(SIGDN_FILESYSPATH, &file_name));

    std::wstring wpath = file_name;
    BinaryReader reader{ std::string{ wpath.begin(), wpath.end() } };
    m_file.load_from(reader);
    m_file_path = wpath;

    if (!m_settings.ChunkPath.empty() || !m_settings.NativePath.empty()) {
        m_file.load_resources(m_settings.ChunkPath, m_settings.NativePath, m_owner->get_device().Get(), m_owner->get_context().Get());
    }

    m_file.run_data_usage_analysis(true);

    update_indices();

    m_first_render = true;
}

void GUIEditor::save_file() {
    if (m_file_path.empty()) {
        return;
    }

    BinaryWriter writer(m_file_path);

    m_file.save_to(writer, m_settings);

    ImGuiToast toast(ImGuiToastType_Success);
    toast.set_title("Saved");
    toast.set_type(ImGuiToastType_Success);
    toast.set_content("Successfully saved file to %s", m_file_path.string().c_str());
    ImGui::InsertNotification(toast);
}

void GUIEditor::save_file_as() {
    HR_INIT(S_OK);

    Microsoft::WRL::ComPtr<IFileSaveDialog> dialog;
    HR_ASSERT(CoCreateInstance(CLSID_FileSaveDialog, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&dialog)));

    constexpr COMDLG_FILTERSPEC filters[] = {
        { L"MHW GUI Files", L"*.gui" }
    };

    HR_ASSERT(dialog->SetFileTypes(ARRAYSIZE(filters), filters));
    HR_ASSERT(dialog->SetDefaultExtension(L"gui"));
    HR_ASSERT(dialog->SetOptions(
        FOS_STRICTFILETYPES | FOS_PATHMUSTEXIST | FOS_OVERWRITEPROMPT
    ));

    if (dialog->Show(nullptr) == HRESULT_FROM_WIN32(ERROR_CANCELLED)) {
        return;
    }

    Microsoft::WRL::ComPtr<IShellItem> item;
    HR_ASSERT(dialog->GetResult(&item));

    LPWSTR file_name;
    HR_ASSERT(item->GetDisplayName(SIGDN_FILESYSPATH, &file_name));

    const std::wstring wpath = file_name;
    BinaryWriter writer(wpath);

    m_file.save_to(writer, m_settings);
    m_file_path = wpath;

    ImGuiToast toast(ImGuiToastType_Success);
    toast.set_title("Saved");
    toast.set_type(ImGuiToastType_Success);
    toast.set_content("Successfully saved file to %s", m_file_path.string().c_str());
    ImGui::InsertNotification(toast);
}

ParamValidationResult GUIEditor::is_valid_param(ParamType type, const std::string& name, ObjectType source_object) const {
    if (source_object == ObjectType::None) {
        return { .CorrectType = type };
    }

    bool invalid_name = false;
    bool invalid_type = false;
    ParamType correct_type = type;

    const std::string source_object_name = enum_to_string(source_object);
    if (source_object != ObjectType::None) {
        if (m_object_info.contains(source_object_name)) {
            const auto& object_info = m_object_info.at(source_object_name);
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

void GUIEditor::select_chunk_dir() {
    m_settings.ChunkPath = open_folder_dialog(L"Select Chunk Directory").string();
}

void GUIEditor::select_native_dir() {
    m_settings.NativePath = open_folder_dialog(L"Select NativePC Directory").string();
}

std::filesystem::path GUIEditor::open_file_dialog(std::wstring_view title, 
    const std::vector<COMDLG_FILTERSPEC>& filters, std::wstring_view default_ext) const {
    HR_INIT(S_OK);

    Microsoft::WRL::ComPtr<IFileOpenDialog> dialog;
    HR_ASSERT(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&dialog)));
    HR_ASSERT(dialog->SetOptions(FOS_PATHMUSTEXIST | FOS_FILEMUSTEXIST | FOS_STRICTFILETYPES));

    HR_ASSERT(dialog->SetTitle(title.data()));
    HR_ASSERT(dialog->SetFileTypes(filters.size(), filters.data()));
    HR_ASSERT(dialog->SetDefaultExtension(default_ext.data()));

    if (dialog->Show(nullptr) == HRESULT_FROM_WIN32(ERROR_CANCELLED)) {
        return {};
    }

    Microsoft::WRL::ComPtr<IShellItem> item;
    LPWSTR path;

    HR_ASSERT(dialog->GetResult(&item));
    HR_ASSERT(item->GetDisplayName(SIGDN_FILESYSPATH, &path));

    return path;
}

std::filesystem::path GUIEditor::open_folder_dialog(std::wstring_view title) const {
    HR_INIT(S_OK);

    Microsoft::WRL::ComPtr<IFileOpenDialog> dialog;
    HR_ASSERT(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&dialog)));
    HR_ASSERT(dialog->SetOptions(FOS_PATHMUSTEXIST | FOS_PICKFOLDERS));
    HR_ASSERT(dialog->SetTitle(title.data()));

    if (dialog->Show(nullptr) == HRESULT_FROM_WIN32(ERROR_CANCELLED)) {
        return {};
    }

    Microsoft::WRL::ComPtr<IShellItem> item;
    LPWSTR path;
    HR_ASSERT(dialog->GetResult(&item));
    HR_ASSERT(item->GetDisplayName(SIGDN_FILESYSPATH, &path));

    return path;
}

void GUIEditor::dump_object_data() const {
    const auto dir = open_folder_dialog(L"Select a search directory");

    nlohmann::json data = nlohmann::json::object();
    GUIFile gui{};

    auto object_contains_field = [](const nlohmann::json& object, std::string_view name) {
        return std::ranges::any_of(object["Fields"], [&](const auto& field) {
            return field.value("Name", "") == name;
        });
    };

    for (const auto& file : std::filesystem::recursive_directory_iterator(dir)) {
        if (file.path().extension() == ".gui") {
            BinaryReader reader(file.path().string());
            gui.load_from(reader);

            for (const auto& obj : gui.m_objects) {
                if (data.contains(enum_to_string(obj.Type))) {
                    for (auto i = 0; i < obj.InitParamNum; ++i) {
                        const auto& param = gui.m_init_params[i + obj.InitParamIndex];
                        auto& jobj = data[enum_to_string(obj.Type)];
                        if (!object_contains_field(jobj, param.Name)) {
                            jobj["Fields"].push_back({
                                {"Name", param.Name},
                                {"Type", std::to_underlying(param.Type)}
                            });
                        }
                    }

                    continue;
                }

                auto type = nlohmann::json::object();
                auto fields = nlohmann::json::array();

                type["Name"] = enum_to_string(obj.Type);

                for (auto i = 0; i < obj.InitParamNum; ++i) {
                    const auto& param = gui.m_init_params[i + obj.InitParamIndex];

                    auto field = nlohmann::json::object();
                    field["Name"] = param.Name;
                    field["Type"] = std::to_underlying(param.Type);

                    fields.push_back({
                        {"Name", param.Name},
                        {"Type", std::to_underlying(param.Type)}
                    });
                }

                type["Fields"] = fields;
                data[enum_to_string(obj.Type)] = type;
            }
        }
    }

    std::ofstream("cGUIObject.json") << data.dump(4);
}

void GUIEditor::open_animation_editor() {
    if (m_animation_editor_first) {
        m_animation_editor_first = false;
    }
    
    m_animation_editor_visible = !m_animation_editor_visible;
}
