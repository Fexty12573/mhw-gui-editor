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


GUIEditor::GUIEditor() {
    HR_INIT(S_OK);
    HR_ASSERT(CoInitializeEx(nullptr, COINIT_MULTITHREADED));

    add_menu_item("File", { "Open", "Ctrl+O", [](GUIEditor* e) { e->open_file(); } });
    add_menu_item("View", { "Animation Editor", "Ctrl+Shift+A", [this](GUIEditor* e) {
        m_animation_editor_visible = true;
        
        
    } });
}

void GUIEditor::add_menu_item(const std::string& menu, MenuItem item) {
    m_menu_items[menu].emplace_back(std::move(item));
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

        ImGui::DockBuilderDockWindow("Overview", dock_id_right);
        ImGui::DockBuilderDockWindow("Tree Viewer", dockspace_id);
    }

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

        ImGui::EndMainMenuBar();
    }

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

    ImGui::Begin("Tree Viewer");

    if (ImGui::RichTextTreeNode("Animations", "<C FFC6913F>Animations</C> ({})", m_file.m_animations.size())) {
        for (auto& m_animation : m_file.m_animations) {
            render_animation(m_animation);
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Objects")) {
        for (auto& m_object : m_file.m_objects) {
            render_object(m_object);
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Sequences")) {
        for (auto& sequence : m_file.m_sequences) {
            render_sequence(sequence);
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("ObjectSequences")) {
        for (auto& m_obj_sequence : m_file.m_obj_sequences) {
            render_obj_sequence(m_obj_sequence);
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("InitParams")) {
        for (auto& m_init_param : m_file.m_init_params) {
            render_init_param(m_init_param);
        }

        ImGui::TreePop();
    }

    ImGui::End();

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

    HR_ASSERT(dialog->Show(nullptr));

    Microsoft::WRL::ComPtr<IShellItem> item;
    HR_ASSERT(dialog->GetResult(&item));
    
    LPWSTR file_name;
    HR_ASSERT(item->GetDisplayName(SIGDN_FILESYSPATH, &file_name));

    std::wstring wpath = file_name;
    BinaryReader reader{ std::string{ wpath.begin(), wpath.end() } };
    m_file.load_from(reader);
    update_indices();

    m_first_render = true;
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

        if (ImGui::TreeNode("Objects")) {
            auto object = &m_file.m_objects[anim.RootObjectIndex];
            render_object(*object);

            while (object->NextIndex != -1) {
                object = &m_file.m_objects[object->NextIndex];
                render_object(*object);
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Sequences")) {
            auto& sequences = m_file.m_sequences;
            const u64 max = std::min(static_cast<u64>(anim.SequenceIndex + anim.SequenceNum), sequences.size());

            for (auto i = anim.SequenceIndex; i < max; ++i) {
                if (ImGui::RichTextTreeNode(fmt::format("Seq{}", i), sequences[i].get_preview(i))) {
                    render_sequence(sequences[i]);
                    ImGui::TreePop();
                }
            }

            ImGui::TreePop();
        }

        ImGui::TreePop();
    }

    ImGui::PopID();
    ImGui::PopID();
}

void GUIEditor::render_object(GUIObject& obj) {
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
            render_object(*object);

            while (object->NextIndex != -1) {
                object = &m_file.m_objects[object->NextIndex];
                render_object(*object);
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

#define UPDATE_INDEX_LOOP(list, IndexMember) \
for (auto i = 0u; i < list.size(); ++i) list[i].##IndexMember = i

void GUIEditor::update_indices() {
    UPDATE_INDEX_LOOP(m_file.m_animations, Index);
    UPDATE_INDEX_LOOP(m_file.m_objects, Index);
    UPDATE_INDEX_LOOP(m_file.m_sequences, Index);
    UPDATE_INDEX_LOOP(m_file.m_obj_sequences, Index);
    UPDATE_INDEX_LOOP(m_file.m_init_params, Index);
}

#undef UPDATE_INDEX_LOOP

void GUIEditor::open_animation_editor() {
    if (m_animation_editor_first) {
        m_animation_editor_first = false;
    }
    
    m_animation_editor_visible = !m_animation_editor_visible;
}
