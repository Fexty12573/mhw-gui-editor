#include "pch.h"
#include "App.h"
#include "Editor.h"
#include "HrException.h"
#include "IconFontAwesome.h"
#include "BinaryReader.h"
#include "BinaryWriter.h"
#include "GUIEditor.h"
#include "GUIFile.h"
#include "GroupPanel.h"

#include "imgui-notify/imgui_notify.h"
#include "imgui-notify/tahoma.h"

#include <fmt/format.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include <imgui.h>
#include <ShObjIdl.h>
#include <spdlog/spdlog.h>
#include <wrl.h>
#include <nlohmann/json.hpp>

#include <fstream>
#include <ranges>



Editor::Editor(App* owner) : m_owner(owner) {
    HR_INIT(S_OK);
    HR_ASSERT(CoInitializeEx(nullptr, COINIT_MULTITHREADED));

    add_menu_item("File", { ICON_FA_FILE " Open", "Ctrl+O", [](Editor* e) { e->open_file(); } });
    add_menu_item("File", { ICON_FA_FLOPPY_DISK " Save", "Ctrl+S", [](Editor* e) { e->save_file(); } });
    add_menu_item("File", { "Save As...", "Ctrl+Shift+S", [](Editor* e) { e->save_file_as(); } });
    add_menu_item("View", { ICON_FA_FILM " Animation Editor", "Ctrl+Shift+A", [this](Editor* e) {

    } });
    add_menu_item("View", { ICON_FA_EXPAND " Preview", "Ctrl+Shift+P", [this](Editor* e) {
        m_preview_open = true;
    }});

    add_menu_item("Edit", { ICON_FA_ARROW_RIGHT " Go To...", "Ctrl+G", [](Editor* e) {
        //e->m_generic_popup_queue.emplace("Go To...", [e](std::any& data) {
        //    auto& path = std::any_cast<std::string&>(data);
        //    if (ImGui::InputText("Path", &path)) {
        //        //data = path;
        //    }

        //    ImGui::NewLine();

        //    if (ImGui::Button("Go")) {
        //        ImGui::CloseCurrentPopup();
        //        //e->m_selected_object = e->m_file.find_object(path);
        //        return true;
        //    }

        //    if (ImGui::Button("Cancel")) {
        //        ImGui::CloseCurrentPopup();
        //        return true;
        //    }

        //    return false;
        //}, std::string{});
    } });

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
    }
    catch (const std::exception& e) {
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

    m_owner->add_window_message_callback([this](UINT msg, WPARAM wparam, LPARAM lparam) {
        if (msg == WM_DROPFILES) {
            handle_file_drop((HDROP)wparam);
        }
    });
}

void Editor::add_menu_item(const std::string& menu, const MenuItem& item) {
    for (auto& m : m_menu_items) {
        if (m.Name == menu) {
            m.Items.push_back(item);
            return;
        }
    }

    m_menu_items.emplace_back(menu, std::vector{ item });
}

void Editor::add_popup(const std::string& title, const std::string& message, PopupType type, const YesNoCancelPopup::CallbackType& callback, std::any user_data) {
    m_popup_queue.emplace(title, message, type, callback, user_data);
}

void Editor::render(u32 dockspace_id) {
    if (m_first_render) {
        m_first_render = false;

        //ImGui::DockBuilderRemoveNode(dockspace_id);
        //ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
        //ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);

        //const ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(
        //    dockspace_id,
        //    ImGuiDir_Right,
        //    0.25f,
        //    nullptr,
        //    &dockspace_id
        //);
        //ImGuiID dock_id_bottom_left = ImGui::DockBuilderSplitNode(
        //    dockspace_id,
        //    ImGuiDir_Down,
        //    0.4f,
        //    nullptr,
        //    &dockspace_id
        //);
        //const ImGuiID dock_id_bottom_right = ImGui::DockBuilderSplitNode(
        //    dock_id_bottom_left,
        //    ImGuiDir_Right,
        //    0.4f,
        //    nullptr,
        //    &dock_id_bottom_left
        //);

        //ImGui::DockBuilderDockWindow("Overview", dock_id_right);
        //ImGui::DockBuilderDockWindow("##Tree Viewer", dockspace_id);
        //ImGui::DockBuilderDockWindow("Resource Manager", dock_id_bottom_left);
        //ImGui::DockBuilderDockWindow("Texture Viewer", dock_id_bottom_right);

        // Create samplers
        const auto device = m_owner->get_device().Get();
        m_samplers[SamplerMode::WrapLinear] = std::make_unique<Sampler>(device, SamplerMode::WrapLinear);
        m_samplers[SamplerMode::ClampLinear] = std::make_unique<Sampler>(device, SamplerMode::ClampLinear);
        m_samplers[SamplerMode::WrapPoint] = std::make_unique<Sampler>(device, SamplerMode::WrapPoint);
        m_samplers[SamplerMode::ClampPoint] = std::make_unique<Sampler>(device, SamplerMode::ClampPoint);
    }

    render_menu_bar();
    render_overview();
    render_tree_viewer();
    render_resource_manager();
    render_texture_viewer();
    render_object_editor();
    render_options_menu();
    render_preview();

    const auto active_editor = get_active_editor();
    if (!active_editor.expired()) {
        const auto editor = active_editor.lock();
        editor->render(dockspace_id);
    }
}

void Editor::open_file() {
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

    return open_file(file_name);
}

void Editor::open_file(const std::filesystem::path& path) {
    m_editors.emplace_back(std::make_shared<GUIEditor>(this, path));
    m_active_editor = m_editors.size() - 1;

    m_first_render = true;
}

void Editor::save_file() const {
    const auto active_editor = get_active_editor();
    if (active_editor.expired()) {
        return;
    }

    const auto editor = active_editor.lock();
    editor->save_file();
}

void Editor::save_file_as() const {
    const auto active_editor = get_active_editor();
    if (active_editor.expired()) {
        return;
    }

    HR_INIT(S_OK);

    Microsoft::WRL::ComPtr<IFileSaveDialog> dialog;
    HR_ASSERT(CoCreateInstance(CLSID_FileSaveDialog, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&dialog)));

    constexpr COMDLG_FILTERSPEC filters[] = {
        { L"MHW/MHGU GUI Files", L"*.gui" }
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

    const auto editor = active_editor.lock();
    editor->save_file_as(wpath);
}

void Editor::open_preview() {
    m_preview_open = true;
}

void Editor::select_chunk_dir() {
    m_settings.ChunkPath = open_folder_dialog(L"Select Chunk Directory").string();
}

void Editor::select_native_dir() {
    m_settings.NativePath = open_folder_dialog(L"Select NativePC Directory").string();
}

void Editor::select_arcfs_dir() {
    m_settings.ArcfsPath = open_folder_dialog(L"Select ArcFS Directory").string();
}

std::filesystem::path Editor::open_file_dialog(std::wstring_view title,
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

std::filesystem::path Editor::open_folder_dialog(std::wstring_view title) const {
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

void Editor::render_menu_bar() {
    bool open_options_window = false;

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

    if (open_options_window) {
        ImGui::OpenPopup("Options##OptionsWindow");
        m_options_menu_open = true;
    }
}

void Editor::render_options_menu() {
    constexpr float window_width = 900.0f;
    const float panel_width = window_width - ImGui::GetStyle().WindowPadding.x * 2.0f;
    const float input_text_width = panel_width - 220.0f - ImGui::GetStyle().ItemSpacing.x * 2.0f;

    ImGui::SetNextWindowSize(ImVec2(window_width, 0), ImGuiCond_Always);
    if (ImGui::BeginPopupModal("Options##OptionsWindow", &m_options_menu_open, 
        ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize)) {

        ImGui::BeginGroupPanel(ICON_FA_BARS " General", { panel_width, 0 });

        ImGui::SetNextItemWidth(input_text_width);
        ImGui::InputText("Chunk Directory", &m_settings.ChunkPath);
        ImGui::SameLine();
        if (ImGui::Button("...##ChunkDir")) {
            select_chunk_dir();
        }

        ImGui::SetNextItemWidth(input_text_width);
        ImGui::InputText("NativePC Directory", &m_settings.NativePath);
        ImGui::SameLine();
        if (ImGui::Button("...##NativeDir")) {
            select_native_dir();
        }

        ImGui::SetNextItemWidth(input_text_width);
        ImGui::InputText("ArcFS Directory", &m_settings.ArcfsPath);
        ImGui::SameLine();
        if (ImGui::Button("...##ArcfsDir")) {
            select_arcfs_dir();
        }

        ImGui::EndGroupPanel();
        ImGui::BeginGroupPanel(ICON_FA_BOLT " Optimization", { panel_width, 0 });

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
        ImGui::BeginGroupPanel(ICON_FA_WRENCH " Utilities", { panel_width, 0 });

        if (ImGui::Checkbox("Auto-Adjust Keyframes", &m_settings.AutoAdjustKeyFrames)) {
            if (m_settings.AutoAdjustKeyFrames) {
                m_popup_queue.emplace("Are you sure?",
                    "Turning on this option and decreasing the ValueCount on a Param will cause Keyframes to be deleted.\nThis cannot be undone.",
                    PopupType::OkCancel, [this](YesNoCancelPopupResult result, const auto&) {
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
}

void Editor::render_popups() {
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
}

void Editor::render_tree_viewer() {
    ImGuiWindowClass cls;
    cls.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar
        | ImGuiDockNodeFlags_NoDockingInCentralNode
        | ImGuiDockNodeFlags_NoUndocking;

    ImGui::SetNextWindowClass(&cls);
    ImGui::Begin("##Tree Viewer", nullptr, ImGuiWindowFlags_NoDecoration);

    if (ImGui::BeginTabBar("##TabBarEditors", ImGuiTabBarFlags_Reorderable)) {
        size_t to_close = -1;
        size_t selected = -1;

        for (auto i = 0; i < m_editors.size(); ++i) {
            bool open = true;
            const auto& editor = m_editors[i];
            if (ImGui::BeginTabItem(editor->get_name().c_str(), &open)) {
                ImGui::PushID(i);
                ImGui::BeginChild("##ScrollArea");

                selected = i;
                editor->render_tree_viewer();

                ImGui::EndChild();
                ImGui::PopID();

                ImGui::EndTabItem();
            }

            if (!open) {
                to_close = i;
            }
        }

        m_active_editor = selected;

        if (to_close != -1) {
            m_editors.erase(m_editors.begin() + to_close);
            if (m_active_editor == to_close) {
                m_active_editor = m_editors.empty() ? -1 : 0;
            } else if (m_active_editor > to_close) {
                --m_active_editor;
            }
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}

void Editor::render_overview() const {
    ImGui::Begin("Overview");

    const auto active_editor = get_active_editor();
    if (!active_editor.expired()) {
        const auto editor = active_editor.lock();
        editor->render_overview();
    }

    ImGui::End();
}

void Editor::render_resource_manager() const {
    ImGui::Begin("Resource Manager");

    const auto active_editor = get_active_editor();
    if (!active_editor.expired()) {
        const auto editor = active_editor.lock();
        editor->render_resource_manager();
    }

    ImGui::End();
}

void Editor::render_texture_viewer() const {
    ImGui::Begin("Texture Viewer");

    const auto active_editor = get_active_editor();
    if (!active_editor.expired()) {
        const auto editor = active_editor.lock();
        editor->render_texture_viewer();
    }

    ImGui::End();
}

void Editor::render_object_editor() {
    
}

void Editor::render_preview() {
    if (!m_preview_open) {
        return;
    }

    if (ImGui::Begin("Preview", &m_preview_open)) {
        const auto active_editor = get_active_editor();
        if (!active_editor.expired()) {
            const auto editor = active_editor.lock();
            editor->render_preview();
        }
    }

    ImGui::End();
}

void Editor::dump_object_data() const {
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

void Editor::handle_file_drop(HDROP drop) {
    char file_name[MAX_PATH];

    const auto count = DragQueryFileA(drop, 0xFFFFFFFF, nullptr, 0);
    for (auto i = 0; i < count; ++i) {
        if (DragQueryFileA(drop, i, file_name, sizeof file_name)) {
            const auto path = std::filesystem::path(file_name);
            if (path.extension() == ".gui") {
                open_file(path);
            }
        }
    }

    DragFinish(drop);
}
