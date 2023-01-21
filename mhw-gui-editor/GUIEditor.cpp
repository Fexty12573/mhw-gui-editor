#include "pch.h"
#include "GUIEditor.h"
#include "HrException.h"
#include "RichText.h"

#include <fmt/format.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include <imgui.h>
#include <ShObjIdl.h>
#include <wrl.h>
#include <DirectXTex.h>
#include <ranges>

#include "App.h"

GUIEditor::GUIEditor(App* owner) : m_owner(owner) {
    HR_INIT(S_OK);
    HR_ASSERT(CoInitializeEx(nullptr, COINIT_MULTITHREADED));
    
    add_menu_item("File", { "Open", "Ctrl+O", [](GUIEditor* e) { e->open_file(); } });
    add_menu_item("View", { "Animation Editor", "Ctrl+Shift+A", [this](GUIEditor* e) {
        m_animation_editor_visible = true;
        
        
    } });

    m_settings.load();
    m_theme_manager.set_theme_directory("./themes");
    m_theme_manager.refresh();

    m_theme_manager.apply_style("Default");
}

void GUIEditor::add_menu_item(const std::string& menu, const MenuItem& item) {
    m_menu_items[menu].push_back(item);
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
            if (ImGui::BeginMenu("Themes")) {
                if (ImGui::MenuItem("Refresh")) {
                    m_theme_manager.refresh();
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Default")) {
                    m_theme_manager.apply_default_style();
                }

                for (const auto& name : m_theme_manager.get_styles() | std::views::keys) {
                    if (ImGui::MenuItem(name.c_str())) {
                        m_theme_manager.apply_style(name);
                    }
                }

                ImGui::EndMenu();
            }

            if (ImGui::MenuItem("Options")) {
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

    if (open_options_window) {
        ImGui::OpenPopup("Options##OptionsWindow");
        m_options_menu_open = true;
    }

    if (ImGui::BeginPopupModal("Options##OptionsWindow", &m_options_menu_open, ImGuiWindowFlags_NoDocking)) {
        ImGui::InputText("Chunk Directory", &m_settings.ChunkPath);
        ImGui::SameLine();
        if (ImGui::Button("Choose Folder")) {
            select_chunk_dir();
        }

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

    ImGui::ShowDemoWindow();
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

    update_indices();

    m_first_render = true;
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

    ImGui::End();
}

void GUIEditor::render_overview() {
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
            ImGui::InputFloat2("InvSize", &tex.InvSize.x, "%.3f");

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

            ImGui::NewLine();
            if (ImGui::Button("Reload")) {
                if (m_settings.ChunkPath.empty()) {
                    m_error_popup_select_file_open = true;
                } else {
                    auto path = std::filesystem::path(m_settings.ChunkPath) / tex.Path;
                    path.replace_extension("tex");

                    BinaryReader reader(path);
                    tex.RenderTexture.load_from(reader, m_owner->get_device().Get(), m_owner->get_context().Get());
                }
            }

            ImGui::TreePop();
        }
    }

    ImGui::End();
}

void GUIEditor::render_texture_viewer() {
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

    if (ImGui::RichTextTreeNode("Obj", obj.get_preview(obj.Index))) {
        ImGui::InputScalar("ID", ImGuiDataType_U32, &obj.ID, &u32_step, &u32_fast_step);
        ImGui::InputInt("Child Index", &obj.ChildIndex);
        ImGui::InputInt("Next Index", &obj.NextIndex);
        ImGui::InputText("Name", &obj.Name);
        ImGui::Text("Type: %s", enum_to_string(obj.Type));

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

void GUIEditor::render_sequence(GUISequence& seq) {
    constexpr u32 u32_step = 1;
    constexpr u32 u32_fast_step = 10;

    ImGui::PushID("Sequence");
    ImGui::PushID(seq.Index);

    if (ImGui::RichTextTreeNode("Seq", seq.get_preview(seq.Index))) {
        ImGui::InputScalar("ID", ImGuiDataType_U32, &seq.ID, &u32_step, &u32_fast_step);
        ImGui::InputText("Name", &seq.Name);
        ImGui::InputScalar("Frame Count", ImGuiDataType_U32, &seq.FrameCount, &u32_step, &u32_fast_step);
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
                const u64 max = std::min(static_cast<u64>(objseq.InitParamIndex + objseq.InitParamNum), initparams.size());

                for (auto i = objseq.InitParamIndex; i < max; ++i) {
                    render_init_param(initparams[i]);
                }

                ImGui::TreePop();
            }
        }

        if (objseq.ParamNum > 0) {
            if (ImGui::TreeNodeEx("Params", ImGuiTreeNodeFlags_SpanAvailWidth)) {
                auto& params = m_file.m_params;
                const u64 max = std::min(static_cast<u64>(objseq.ParamIndex + objseq.ParamNum), params.size());

                for (auto i = objseq.ParamIndex; i < max; ++i) {
                    render_param(params[i]);
                }

                ImGui::TreePop();
            }
        }

        ImGui::TreePop();
    }

    ImGui::PopID();
    ImGui::PopID();
}

void GUIEditor::render_init_param(GUIInitParam& param) {
    constexpr u32 u32_step = 1;
    constexpr u32 u32_fast_step = 10;

    ImGui::PushID("InitParam");
    ImGui::PushID(param.Index);

    if (ImGui::RichTextTreeNode("InitParam", param.get_preview(param.Index))) {
        ImGui::Checkbox("Use", &param.Use);
        ImGui::InputScalar("Type", ImGuiDataType_U8, &param.Type, &u32_step, &u32_fast_step);
        ImGui::InputText("Name", &param.Name);

        ImGui::TreePop();
    }

    ImGui::PopID();
    ImGui::PopID();
}

void GUIEditor::render_param(GUIParam& param) {
    constexpr u32 u32_step = 1;
    constexpr u32 u32_fast_step = 10;

    ImGui::PushID("Param");
    ImGui::PushID(param.Index);

    if (ImGui::RichTextTreeNode("Param", param.get_preview(param.Index))) {
        ImGui::InputScalar("Type", ImGuiDataType_U8, &param.Type, &u32_step, &u32_fast_step);
        ImGui::InputScalar("Count", ImGuiDataType_U8, &param.ValueCount, &u32_step, &u32_fast_step);
        ImGui::InputText("Name", &param.Name);

        ImGui::TreePop();
    }

    ImGui::PopID();
    ImGui::PopID();
}

#define UPDATE_INDEX_LOOP(list, IndexMember) \
for (auto i = 0u; i < list.size(); ++i) list[i].##IndexMember = i

void GUIEditor::update_indices() {
    UPDATE_INDEX_LOOP(m_file.m_animations, Index);
    UPDATE_INDEX_LOOP(m_file.m_objects, Index);
    UPDATE_INDEX_LOOP(m_file.m_sequences, Index);
    UPDATE_INDEX_LOOP(m_file.m_obj_sequences, Index);
    UPDATE_INDEX_LOOP(m_file.m_init_params, Index);
    UPDATE_INDEX_LOOP(m_file.m_params, Index);
}

void GUIEditor::select_chunk_dir() {
    HR_INIT(S_OK);

    Microsoft::WRL::ComPtr<IFileOpenDialog> dialog;
    HR_ASSERT(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&dialog)));
    HR_ASSERT(dialog->SetOptions(FOS_PATHMUSTEXIST | FOS_PICKFOLDERS));

    if (dialog->Show(nullptr) == ERROR_CANCELLED) {
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
