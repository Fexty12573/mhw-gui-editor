#include "pch.h"
#include "GUIEditor.h"
#include "HrException.h"
#include "RichText.h"
#include "GroupPanel.h"
#include "CurveEditor.h"
#include "App.h"
#include "IconFontAwesome.h"
#include "crc32.h"

#include <fmt/format.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include <imgui.h>
#include <ShObjIdl.h>
#include <wrl.h>
#include <ranges>


GUIEditor::GUIEditor(App* owner) : m_owner(owner) {
    HR_INIT(S_OK);
    HR_ASSERT(CoInitializeEx(nullptr, COINIT_MULTITHREADED));
    
    add_menu_item("File", { ICON_FA_FILE " Open", "Ctrl+O", [](GUIEditor* e) { e->open_file(); } });
    add_menu_item("File", { ICON_FA_FLOPPY_DISK " Save", "Ctrl+S", [](GUIEditor* e) { e->save_file(); } });
    add_menu_item("File", { "Save As...", "Ctrl+Shift+S", [](GUIEditor* e) {e->save_file_as(); }});
    add_menu_item("View", { "Animation Editor", "Ctrl+Shift+A", [this](GUIEditor* e) {
        m_animation_editor_visible = true;
        
        
    } });

    m_settings.load();
    m_theme_manager.set_theme_directory("./themes");
    m_theme_manager.refresh();

    if (m_settings.Theme.empty()) {
        m_theme_manager.apply_style("Default");
    } else {
        m_theme_manager.apply_style(m_settings.Theme);
    }

    const auto fonts = ImGui::GetIO().Fonts;

    fonts->AddFontFromFileTTF("./fonts/CascadiaMono.ttf", 16.0f);

    ImFontConfig config;
    config.MergeMode = true;
    config.GlyphMinAdvanceX = 16.0f;
    config.GlyphMaxAdvanceX = 16.0f;
    static constexpr ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };

    fonts->AddFontFromFileTTF("./fonts/Font Awesome 6 Free-Solid-900.otf", 16.0f, &config, icon_ranges);
    fonts->Build();
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

    if (ImGui::BeginPopupModal("Options##OptionsWindow", &m_options_menu_open, ImGuiWindowFlags_NoDocking)) {

        ImGui::BeginGroupPanel(ICON_FA_BARS " General");

        ImGui::InputText("Chunk Directory", &m_settings.ChunkPath);
        ImGui::SameLine();
        if (ImGui::Button("Choose Folder")) {
            select_chunk_dir();
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

    if (!m_settings.ChunkPath.empty()) {
        m_file.load_resources(m_settings.ChunkPath, m_owner->get_device().Get(), m_owner->get_context().Get());
    }

    m_file.run_data_usage_analysis(true);

    update_indices();

    m_first_render = true;
}

void GUIEditor::save_file() {
    
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
        FOS_STRICTFILETYPES | FOS_PATHMUSTEXIST
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
}

void GUIEditor::render_tree_viewer() {
    ImGui::Begin("Tree Viewer");

    if (ImGui::RichTextTreeNode("Animations", "<C FFC6913F>Animations</C> ({})", m_file.m_animations.size())) {
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

    if (ImGui::TreeNodeEx("Params", ImGuiTreeNodeFlags_SpanAvailWidth)) {
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

void GUIEditor::render_overview() const {
    ImGui::Begin("Overview");

    ImGui::RichText("<C FFC6913F>Animation Count:</C> {}", m_file.m_animations.size());
    ImGui::RichText("<C FFC6913F>Sequence Count:</C> {}", m_file.m_sequences.size());
    ImGui::RichText("<C FFC6913F>Object Count:</C> {}", m_file.m_objects.size());
    ImGui::RichText("<C FFC6913F>ObjectSequence Count:</C> {}", m_file.m_obj_sequences.size());
    ImGui::RichText("<C FFC6913F>InitParam Count:</C> {}", m_file.m_init_params.size());
    ImGui::RichText("<C FFC6913F>Param Count:</C> {}", m_file.m_params.size());
    ImGui::RichText("<C FFC6913F>Key Count:</C> {}", m_file.m_keys.size());
    ImGui::RichText("<C FFC6913F>Instance Count:</C> {}", m_file.m_instances.size());
    ImGui::RichText("<C FFC6913F>Flow Count:</C> {}", m_file.m_flows.size());
    ImGui::RichText("<C FFC6913F>FlowProcess Count:</C> {}", m_file.m_flow_processes.size());
    ImGui::RichText("<C FFC6913F>Texture Count:</C> {}", m_file.m_textures.size());
    ImGui::RichText("<C FFC6913F>FontFilter Count:</C> {}", m_file.m_font_filters.size());
    ImGui::RichText("<C FFC6913F>Message Count:</C> {}", m_file.m_messages.size());
    ImGui::RichText("<C FFC6913F>Resource Count:</C> {}", m_file.m_resources.size());
    ImGui::RichText("<C FFC6913F>GeneralResource Count:</C> {}", m_file.m_general_resources.size());

    static MtHermiteCurve curve = {
        {0.087f, 0.154f, 0.479f, 0.527f, 0.7f, 0.738f, 0.857f, 1.0f},
        {0.195f, 0.475f, 0.305f, 0.670f, 0.7f, 0.340f, 0.275f, 0.105f}
    };

    ImGui::HermiteCurve("Curve", &curve, { 400, 400 });
    ImGui::Text("X: { %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f }", 
        curve.x[0], curve.x[1], curve.x[2], curve.x[3], curve.x[4], curve.x[5], curve.x[6], curve.x[7]);
    ImGui::Text("Y: { %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f }", 
        curve.y[0], curve.y[1], curve.y[2], curve.y[3], curve.y[4], curve.y[5], curve.y[6], curve.y[7]);

    ImGui::End();
}

void GUIEditor::render_resource_manager() {
    ImGui::Begin("Resource Manager");

    constexpr ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
    constexpr u32 u32_step = 1;
    constexpr u32 u32_fast_step = 10;

    for (auto i = 0; i < m_file.m_textures.size(); ++i) {
        ImGuiTreeNodeFlags node_flags = flags;

        auto& tex = m_file.m_textures[i];

        if (m_selected_texture == i) {
            node_flags |= ImGuiTreeNodeFlags_Selected;
        }

        const bool open = ImGui::TreeNodeEx(tex.Name.c_str(), node_flags);
        if (ImGui::IsItemClicked()) {
            m_selected_texture = i;
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
                        tex.Path = relpath.replace_extension().generic_string();

                        BinaryReader reader(path);
                        tex.RenderTexture.load_from(reader, m_owner->get_device().Get(), m_owner->get_context().Get());
                    }
                }
            }

            if (ImGui::Button(ICON_FA_ARROW_ROTATE_RIGHT " Reload")) {
                if (m_settings.ChunkPath.empty()) {
                    m_error_popup_select_file_open = true;
                } else {
                    auto path = std::filesystem::path(m_settings.ChunkPath) / tex.Path;
                    path.replace_extension("tex");

                    BinaryReader reader(path);
                    tex.RenderTexture.load_from(reader, m_owner->get_device().Get(), m_owner->get_context().Get());
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

void GUIEditor::render_texture_viewer() const {
    ImGui::Begin("Texture Viewer");

    if (m_selected_texture != -1 && m_selected_texture < m_file.m_textures.size()) {
        const auto& tex = m_file.m_textures[m_selected_texture];
        if (tex.RenderTexture.is_valid()) {
            ImGui::Image(tex.RenderTexture.get_view().Get(), ImVec2{ static_cast<float>(tex.Width), static_cast<float>(tex.Height) });
        } else {
            ImGui::TextColored({ 1.0f, 0.2f, 0.2f, 1.0f }, "Error: Couldn't load texture!");
            ImGui::Text("Make sure the file exists and you have the correct chunk path set under Tools -> Options");
        }
    }

    ImGui::End();
}

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
                    render_init_param(m_file.m_init_params[i]);
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
                    render_obj_sequence(m_file.m_obj_sequences[i]);
                }

                ImGui::TreePop();
            }
        }

        ImGui::TreePop();
    }

    ImGui::PopID();
    ImGui::PopID();
}

void GUIEditor::render_sequence(GUISequence& seq) const {
    constexpr u32 u32_step = 1;
    constexpr u32 u32_fast_step = 10;

    ImGui::PushID("Sequence");
    ImGui::PushID(seq.Index);

    if (ImGui::RichTextTreeNode("Seq", seq.get_preview(seq.Index))) {
        ImGui::InputScalar("ID", ImGuiDataType_U32, &seq.ID, &u32_step, &u32_fast_step);
        ImGui::InputText("Name", &seq.Name);
        ImGui::InputScalar("Frame Count", ImGuiDataType_U32, &seq.FrameCount, &u32_step, &u32_fast_step);

        ImGui::TreePop();
    }

    ImGui::PopID();
    ImGui::PopID();
}

void GUIEditor::render_obj_sequence(GUIObjectSequence& objseq) {
    constexpr u32 u32_step = 1;
    constexpr u32 u32_fast_step = 10;

    ImGui::PushID("ObjectSequence");
    ImGui::PushID(objseq.Index);

    if (ImGui::RichTextTreeNode("ObjSeq", objseq.get_preview(objseq.Index))) {
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
                const u64 max = std::min(static_cast<u64>(objseq.InitParamIndex + static_cast<u64>(objseq.InitParamNum)), initparams.size());

                for (auto i = objseq.InitParamIndex; i < max; ++i) {
                    render_init_param(initparams[i]);
                }

                if (objseq.InitParamIndex + static_cast<u64>(objseq.InitParamNum) > m_file.m_init_params.size()) {
                    ImGui::TextColored({ 1.0f, 1.0f, 0.2f, 1.0f }, "Warning: InitParamNum goes out of bounds!");
                }

                ImGui::TreePop();
            }
        }

        if (objseq.ParamNum > 0) {
            if (ImGui::TreeNodeEx("Params", ImGuiTreeNodeFlags_SpanAvailWidth)) {
                auto& params = m_file.m_params;
                const u64 max = std::min(static_cast<u64>(objseq.ParamIndex + static_cast<u64>(objseq.ParamNum)), params.size());

                for (auto i = objseq.ParamIndex; i < max; ++i) {
                    render_param(params[i]);
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

void GUIEditor::render_init_param(GUIInitParam& param) const {
    using namespace crc::literals;
    constexpr u32 u32_step = 1;
    constexpr u32 u32_fast_step = 10;

    ImGui::PushID("InitParam");
    ImGui::PushID(param.Index);

    if (ImGui::RichTextTreeNode("InitParam", param.get_preview(param.Index))) {
        ImGui::Checkbox("Use", &param.Use);
        ImGui::RichTextCombo("Type", reinterpret_cast<u8*>(&param.Type), ParamTypeNames, 0xFFB0C94E);
        ImGui::InputText("Name", &param.Name);

        switch (param.Type) {
        case ParamType::UNKNOWN:
            break;
        case ParamType::INT:
            ImGui::InputScalar("Value", ImGuiDataType_U32, &param.Value32, &u32_step, &u32_fast_step);
            break;
        case ParamType::FLOAT:
            ImGui::InputFloat("Value", &param.ValueFloat, 0.01f, 0.1f, "%.3f");
            break;
        case ParamType::BOOL:
            ImGui::Checkbox("Value", &param.ValueBool);
            break;
        case ParamType::VECTOR:
            ImGui::ColorEdit4("Value", &param.ValueVector.x, ImGuiColorEditFlags_Float);
            break;
        case ParamType::RESOURCE:
            ImGui::InputScalar("Value", ImGuiDataType_U32, &param.Value32, &u32_step, &u32_fast_step);
            for (const auto& res : m_file.m_resources) {
                if (res.ID == param.Value32) {
                    ImGui::SameLine();
                    ImGui::Text("Resource: %s", res.Path.c_str());
                    break;
                }
            }
            break;
        case ParamType::STRING:
            ImGui::InputText("Value", &param.ValueString);
            break;
        case ParamType::TEXTURE:
            ImGui::InputScalar("Value", ImGuiDataType_U32, &param.Value32, &u32_step, &u32_fast_step);
            for (const auto& tex : m_file.m_textures) {
                if (tex.ID == param.Value32) {
                    ImGui::SameLine();
                    ImGui::Text("Texture: %s", tex.Path.c_str());
                    break;
                }
            }
            break;
        case ParamType::GUIRESOURCE:
            ImGui::InputScalar("Value", ImGuiDataType_U32, &param.Value32, &u32_step, &u32_fast_step);
            for (const auto& res : m_file.m_resources) {
                if (res.ID == param.Value32) {
                    ImGui::SameLine();
                    ImGui::Text("GUIResource: %s", res.Path.c_str());
                    break;
                }
            }
            break;
        case ParamType::GENERALRESOURCE:
            ImGui::InputScalar("Value", ImGuiDataType_U32, &param.Value32, &u32_step, &u32_fast_step);
            for (const auto& res : m_file.m_general_resources) {
                if (res.ID == param.Value32) {
                    ImGui::SameLine();
                    ImGui::Text("GeneralResource: %s", res.Path.c_str());
                    break;
                }
            }
            break;
        case ParamType::FONT: [[fallthrough]];
        case ParamType::MESSAGE: [[fallthrough]];
        case ParamType::VARIABLE: [[fallthrough]]; 
        case ParamType::ANIMATION: [[fallthrough]]; 
        case ParamType::EVENT: [[fallthrough]]; 
        case ParamType::FONT_FILTER: [[fallthrough]];
        case ParamType::ANIMEVENT: [[fallthrough]]; 
        case ParamType::SEQUENCE: 
            ImGui::InputScalar("Value", ImGuiDataType_U32, &param.Value32, &u32_step, &u32_fast_step);
            break;
        case ParamType::INIT_BOOL:
            ImGui::Checkbox("Value", &param.ValueBool);
            break;
        case ParamType::INIT_INT:
            switch (param.NameCRC) {
            case "BlendState"_crc:
                ImGui::RichTextCombo("Value", &param.Value8, BlendStateNames, 0xFFA3D7B8);
                break;
            case "Alignment"_crc:
                ImGui::RichTextCombo("Value", &param.Value8, AlignmentNames, 0xFFA3D7B8);
                break;
            case "ResolutionAdjust"_crc:
                ImGui::RichTextCombo("Value", &param.Value8, ResolutionAdjustNames, 0xFFA3D7B8);
                break;
            case "AutoWrap"_crc:
                ImGui::RichTextCombo("Value", &param.Value8, AutoWrapNames, 0xFFA3D7B8);
                break;
            case "ColorControl"_crc:
                ImGui::RichTextCombo("Value", &param.Value8, ColorControlNames, 0xFFA3D7B8);
                break;
            case "LetterHAlign"_crc:
                ImGui::RichTextCombo("Value", &param.Value8, LetterHAlignNames, 0xFFA3D7B8);
                break;
            case "LetterVAlign"_crc:
                ImGui::RichTextCombo("Value", &param.Value8, LetterVAlignNames, 0xFFA3D7B8);
                break;
            case "DepthState"_crc:
                ImGui::RichTextCombo("Value", &param.Value8, DepthStateNames, 0xFFA3D7B8);
                break;
            case "Billboard"_crc:
                ImGui::RichTextCombo("Value", &param.Value8, BillboardNames, 0xFFA3D7B8);
                break;
            case "DrawPass"_crc:
                ImGui::RichTextCombo("Value", &param.Value8, DrawPassNames, 0xFFA3D7B8);
                break;
            case "Mask"_crc:
                ImGui::RichTextCombo("Value", &param.Value8, MaskTypeNames, 0xFFA3D7B8);
                break;
            default:
                ImGui::InputScalar("Value", ImGuiDataType_U8, &param.Value8, &u32_step, &u32_fast_step);
                break;
            }

            break;
            
        case ParamType::INIT_INT32:
            switch (param.NameCRC) {
            case "FontStyleId"_crc:
                ImGui::RichTextCombo("Value", &param.Value32, FontStyleNames, 0xFFA3D7B8);
                break;
            }
        }

        ImGui::TreePop();
    }

    ImGui::PopID();
    ImGui::PopID();
}

#define GET_VEC_V(V, TYPE, IDX) std::get<std::vector<TYPE>>(V)[IDX]

void GUIEditor::render_param(GUIParam& param) {
    using namespace crc::literals;
    constexpr u32 u32_step = 1;
    constexpr u32 u32_fast_step = 10;

    ImGui::PushID("Param");
    ImGui::PushID(param.Index);

    if (ImGui::RichTextTreeNode("Param", param.get_preview(param.Index))) {
        if (ImGui::RichTextCombo("Type", reinterpret_cast<u8*>(&param.Type), ParamTypeNames, 0xFFB0C94E)) {
            param.perform_value_operation(
                [](auto, auto& p) { p.Values = std::vector<u8>(p.ValueCount, 0); },
                [](auto, auto& p) { p.Values = std::vector<u32>(p.ValueCount, 0); },
                [](auto, auto& p) { p.Values = std::vector<f32>(p.ValueCount, 0.0f); },
                [](auto, auto& p) { p.Values = std::vector<MtVector4>(p.ValueCount, MtVector4{}); },
                [](auto, auto& p) { p.Values = std::vector<std::string>(p.ValueCount, ""); },
                false
            );
        }

        if (ImGui::InputScalar("Count", ImGuiDataType_U8, &param.ValueCount, &u32_step, &u32_fast_step)) {
            param.perform_value_operation(
                [](auto v, auto& p) { v->resize(p.ValueCount, 0); },
                [](auto v, auto& p) { v->resize(p.ValueCount, 0); },
                [](auto v, auto& p) { v->resize(p.ValueCount, 0.0f); },
                [](auto v, auto& p) { v->resize(p.ValueCount, MtVector4{}); },
                [](auto v, auto& p) { v->resize(p.ValueCount, ""); }
            );
        }
        ImGui::InputText("Name", &param.Name);

        for (auto i = 0u; i < param.ValueCount; ++i) {
            const std::string name = fmt::format("Value[{}]", i);
            switch (param.Type) {
            case ParamType::UNKNOWN:
                break;
            case ParamType::INT:
                ImGui::InputScalar(name.c_str(), ImGuiDataType_U32, &GET_VEC_V(param.Values, u32, i), &u32_step, &u32_fast_step);
                break;
            case ParamType::FLOAT:
                ImGui::InputFloat(name.c_str(), &GET_VEC_V(param.Values, float, i), 0.01f, 0.1f, "%.3f");
                break;
            case ParamType::BOOL:
                ImGui::Checkbox(name.c_str(), reinterpret_cast<bool*>(&GET_VEC_V(param.Values, u8, i)));
                break;
            case ParamType::VECTOR:
                ImGui::ColorEdit4(name.c_str(), &GET_VEC_V(param.Values, vector4, i).x, ImGuiColorEditFlags_Float);
                break;
            case ParamType::RESOURCE: {
                auto v = GET_VEC_V(param.Values, u32, i);
                ImGui::InputScalar(name.c_str(), ImGuiDataType_U32, &v, &u32_step, &u32_fast_step);
                for (const auto& res : m_file.m_resources) {
                    if (res.ID == v) {
                        ImGui::SameLine();
                        ImGui::Text("Resource: %s", res.Path.c_str());
                        break;
                    }
                }

                GET_VEC_V(param.Values, u32, i) = v;
                break;
            }
            case ParamType::STRING:
                ImGui::InputText(name.c_str(), &GET_VEC_V(param.Values, std::string, i));
                break;
            case ParamType::TEXTURE: {
                auto v = GET_VEC_V(param.Values, u32, i);
                ImGui::InputScalar(name.c_str(), ImGuiDataType_U32, &v, &u32_step, &u32_fast_step);
                for (const auto& tex : m_file.m_textures) {
                    if (tex.ID == v) {
                        ImGui::SameLine();
                        ImGui::Text("Texture: %s", tex.Path.c_str());
                        break;
                    }
                }

                GET_VEC_V(param.Values, u32, i) = v;
                break;
            }
            case ParamType::GUIRESOURCE: {
                auto v = GET_VEC_V(param.Values, u32, i);
                ImGui::InputScalar(name.c_str(), ImGuiDataType_U32, &v, &u32_step, &u32_fast_step);
                for (const auto& tex : m_file.m_resources) {
                    if (tex.ID == v) {
                        ImGui::SameLine();
                        ImGui::Text("GUIResource: %s", tex.Path.c_str());
                        break;
                    }
                }

                GET_VEC_V(param.Values, u32, i) = v;
                break;
            }
            case ParamType::GENERALRESOURCE: {
                auto v = GET_VEC_V(param.Values, u32, i);
                ImGui::InputScalar(name.c_str(), ImGuiDataType_U32, &v, &u32_step, &u32_fast_step);
                for (const auto& tex : m_file.m_general_resources) {
                    if (tex.ID == v) {
                        ImGui::SameLine();
                        ImGui::Text("GeneralResource: %s", tex.Path.c_str());
                        break;
                    }
                }

                GET_VEC_V(param.Values, u32, i) = v;
                break;
            }
            case ParamType::FONT: [[fallthrough]];
            case ParamType::MESSAGE: [[fallthrough]];
            case ParamType::VARIABLE: [[fallthrough]];
            case ParamType::ANIMATION: [[fallthrough]];
            case ParamType::EVENT: [[fallthrough]];
            case ParamType::FONT_FILTER: [[fallthrough]];
            case ParamType::ANIMEVENT: [[fallthrough]];
            case ParamType::SEQUENCE:
                ImGui::InputScalar(name.c_str(), ImGuiDataType_U32, &GET_VEC_V(param.Values, u32, i), &u32_step, &u32_fast_step);
                break;
            case ParamType::INIT_BOOL:
                ImGui::Checkbox(name.c_str(), reinterpret_cast<bool*>(&GET_VEC_V(param.Values, u8, i)));
                break;
            case ParamType::INIT_INT:
                switch (param.NameCRC) {
                case "BlendState"_crc:
                    ImGui::RichTextCombo(name.c_str(), &GET_VEC_V(param.Values, u8, i), BlendStateNames, 0xFFA3D7B8);
                    break;
                case "Alignment"_crc:
                    ImGui::RichTextCombo(name.c_str(), &GET_VEC_V(param.Values, u8, i), AlignmentNames, 0xFFA3D7B8);
                    break;
                case "ResolutionAdjust"_crc:
                    ImGui::RichTextCombo(name.c_str(), &GET_VEC_V(param.Values, u8, i), ResolutionAdjustNames, 0xFFA3D7B8);
                    break;
                case "AutoWrap"_crc:
                    ImGui::RichTextCombo(name.c_str(), &GET_VEC_V(param.Values, u8, i), AutoWrapNames, 0xFFA3D7B8);
                    break;
                case "ColorControl"_crc:
                    ImGui::RichTextCombo(name.c_str(), &GET_VEC_V(param.Values, u8, i), ColorControlNames, 0xFFA3D7B8);
                    break;
                case "LetterHAlign"_crc:
                    ImGui::RichTextCombo(name.c_str(), &GET_VEC_V(param.Values, u8, i), LetterHAlignNames, 0xFFA3D7B8);
                    break;
                case "LetterVAlign"_crc:
                    ImGui::RichTextCombo(name.c_str(), &GET_VEC_V(param.Values, u8, i), LetterVAlignNames, 0xFFA3D7B8);
                    break;
                case "DepthState"_crc:
                    ImGui::RichTextCombo(name.c_str(), &GET_VEC_V(param.Values, u8, i), DepthStateNames, 0xFFA3D7B8);
                    break;
                case "Billboard"_crc:
                    ImGui::RichTextCombo(name.c_str(), &GET_VEC_V(param.Values, u8, i), BillboardNames, 0xFFA3D7B8);
                    break;
                case "DrawPass"_crc:
                    ImGui::RichTextCombo(name.c_str(), &GET_VEC_V(param.Values, u8, i), DrawPassNames, 0xFFA3D7B8);
                    break;
                case "Mask"_crc:
                    ImGui::RichTextCombo(name.c_str(), &GET_VEC_V(param.Values, u8, i), MaskTypeNames, 0xFFA3D7B8);
                    break;
                default:
                    ImGui::InputScalar(name.c_str(), ImGuiDataType_U8, &GET_VEC_V(param.Values, u8, i), &u32_step, &u32_fast_step);
                    break;
                }

                break;

            case ParamType::INIT_INT32:
                switch (param.NameCRC) {
                case "FontStyleId"_crc:
                    ImGui::RichTextCombo(name.c_str(), &GET_VEC_V(param.Values, u32, i), FontStyleNames, 0xFFA3D7B8);
                    break;
                }
                break;
            }

            
        }

        if (ImGui::TreeNode("Keyframes")) {
            const u64 max = std::min(static_cast<u64>(param.KeyIndex + param.ValueCount), m_file.m_keys.size());

            for (auto i = param.KeyIndex; i < max; i++) {
                render_key(m_file.m_keys[i], param.Type);
            }

            ImGui::TreePop();
        }

        ImGui::TreePop();
    }

    ImGui::PopID();
    ImGui::PopID();
}

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

        const auto param_idx = m_file.m_header.instanceParamEntryStartIndex + inst.Index;
        if (param_idx < m_file.m_params.size()) {
            render_param(m_file.m_params[param_idx]);
        }

        ImGui::TreePop();
    }

    ImGui::PopID();
    ImGui::PopID();
}

void GUIEditor::render_key(GUIKey& key, ParamType type) const {
    constexpr u32 u32_step = 1;
    constexpr u32 u32_fast_step = 10;

    ImGui::PushID("Key");
    ImGui::PushID(key.Index);

    if (ImGui::RichTextTreeNode("Key", key.get_preview(key.Index))) {
        u32 frame = key.Data.Bitfield.Frame;
        u8 mode = key.Data.Bitfield.Mode_;

        ImGui::InputScalar("Frame", ImGuiDataType_U32, &frame, &u32_step, &u32_fast_step);
        ImGui::RichTextCombo("Interpolation Mode", &mode, KeyModeNames, 0xFFA3D7B8);
        if (type != ParamType::FLOAT && type != ParamType::VECTOR && 
            type != ParamType::INT && type != ParamType::UNKNOWN) {
            ImGui::TextColored({ 1.0f, 1.0f, 0.2f, 1.0f }, "Note: Interpolation Mode only works for float, vector and int types.");
        } else {
            if (key.Data.Named.Mode == KeyMode::HERMITE2) {
                ImGui::HermiteCurve("Curve", &key.Curve, { 350, 350 });
            }
        }

        key.Data.Bitfield.Frame = frame;
        key.Data.Bitfield.Mode_ = mode;

        ImGui::TreePop();
    }

    ImGui::PopID();
    ImGui::PopID();
}

#undef GET_VEC_V
#define UPDATE_INDEX_LOOP(list, IndexMember) \
for (auto i = 0u; i < list.size(); ++i) list[i].##IndexMember = i

void GUIEditor::update_indices() {
    UPDATE_INDEX_LOOP(m_file.m_animations, Index);
    UPDATE_INDEX_LOOP(m_file.m_objects, Index);
    UPDATE_INDEX_LOOP(m_file.m_sequences, Index);
    UPDATE_INDEX_LOOP(m_file.m_obj_sequences, Index);
    UPDATE_INDEX_LOOP(m_file.m_init_params, Index);
    UPDATE_INDEX_LOOP(m_file.m_params, Index);
    UPDATE_INDEX_LOOP(m_file.m_instances, Index);
    UPDATE_INDEX_LOOP(m_file.m_keys, Index);
}

void GUIEditor::select_chunk_dir() {
    HR_INIT(S_OK);

    Microsoft::WRL::ComPtr<IFileOpenDialog> dialog;
    HR_ASSERT(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&dialog)));
    HR_ASSERT(dialog->SetOptions(FOS_PATHMUSTEXIST | FOS_PICKFOLDERS));

    if (dialog->Show(nullptr) == HRESULT_FROM_WIN32(ERROR_CANCELLED)) {
        return;
    }

    Microsoft::WRL::ComPtr<IShellItem> item;
    LPWSTR path;
    HR_ASSERT(dialog->GetResult(&item));
    HR_ASSERT(item->GetDisplayName(SIGDN_FILESYSPATH, &path));

    std::wstring wpath = path;
    m_settings.ChunkPath = { wpath.begin(), wpath.end() };
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

#undef UPDATE_INDEX_LOOP

void GUIEditor::open_animation_editor() {
    if (m_animation_editor_first) {
        m_animation_editor_first = false;
    }
    
    m_animation_editor_visible = !m_animation_editor_visible;
}
