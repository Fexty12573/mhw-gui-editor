#include "pch.h"
#include "GUIEditor.h"
#include "HrException.h"

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
        
        ImGui::DockB
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

    ImGui::Text("Animation Count: %llu", m_file.m_animations.size());
    ImGui::Text("Sequence Count: %llu", m_file.m_sequences.size());
    ImGui::Text("Object Count: %llu", m_file.m_objects.size());
    ImGui::Text("ObjectSequence Count: %llu", m_file.m_obj_sequences.size());
    ImGui::Text("InitParam Count: %llu", m_file.m_init_params.size());
    ImGui::Text("Param Count: %llu", m_file.m_params.size());
    ImGui::Text("Key Count: %llu", m_file.m_keys.size());
    ImGui::Text("Instance Count: %llu", m_file.m_instances.size());
    ImGui::Text("Flow Count: %llu", m_file.m_flows.size());
    ImGui::Text("FlowProcess Count: %llu", m_file.m_flow_processes.size());
    ImGui::Text("Texture Count: %llu", m_file.m_textures.size());
    ImGui::Text("FontFilter Count: %llu", m_file.m_font_filters.size());
    ImGui::Text("Message Count: %llu", m_file.m_messages.size());
    ImGui::Text("Resource Count: %llu", m_file.m_resources.size());
    ImGui::Text("GeneralResource Count: %llu", m_file.m_general_resources.size());

    ImGui::End();

    ImGui::Begin("Tree Viewer");

    if (ImGui::TreeNode("Animations")) {
        for (auto i = 0u; i < m_file.m_animations.size(); ++i) {
            if (ImGui::TreeNode(fmt::format("Anim{}", i).c_str(), m_file.m_animations[i].get_preview(i).c_str())) {
                render_animation(m_file.m_animations[i]);
                ImGui::TreePop();
            }
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Objects")) {
        for (auto i = 0u; i < m_file.m_objects.size(); ++i) {
            if (ImGui::TreeNode(fmt::format("Obj{}", i).c_str(), m_file.m_objects[i].get_preview(i).c_str())) {
                render_object(m_file.m_objects[i]);
                ImGui::TreePop();
            }
        }

        ImGui::TreePop();
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

    HR_ASSERT(dialog->Show(nullptr));

    Microsoft::WRL::ComPtr<IShellItem> item;
    HR_ASSERT(dialog->GetResult(&item));
    
    LPWSTR file_name;
    HR_ASSERT(item->GetDisplayName(SIGDN_FILESYSPATH, &file_name));

    std::wstring wpath = file_name;
    BinaryReader reader{ { wpath.begin(), wpath.end() } };
    m_file.load_from(reader);

    m_first_render = true;
}

void GUIEditor::render_animation(GUIAnimation& anim) {
    constexpr u32 u32_step = 1;
    constexpr u32 u32_fast_step = 10;

    ImGui::PushID("Animation");
    ImGui::PushID(anim.ID);

    ImGui::InputScalar("ID", ImGuiDataType_U32, &anim.ID, &u32_step, &u32_fast_step);
    ImGui::InputText("Name", &anim.Name);
    ImGui::InputScalar("Root Object Index", ImGuiDataType_U32, &anim.RootObjectIndex, &u32_step, &u32_fast_step);
    ImGui::InputScalar("Object Count", ImGuiDataType_U32, &anim.ObjectNum, &u32_step, &u32_fast_step);

    if (ImGui::TreeNode("Objects")) {
        auto& objects = m_file.m_objects;
        const u64 max = std::min(static_cast<u64>(anim.RootObjectIndex + anim.ObjectNum), objects.size());

        for (auto i = anim.RootObjectIndex; i < max; ++i) {
            if (ImGui::TreeNode(fmt::format("Obj{}", i).c_str(), objects[i].get_preview(i).c_str())) {
                render_object(objects[i]);

                ImGui::TreePop();
            }
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
    ImGui::PushID(obj.ID);

    ImGui::InputScalar("ID", ImGuiDataType_U32, &obj.ID, &u32_step, &u32_fast_step);
    ImGui::InputText("Name", &obj.Name);
    ImGui::Text("Type: %s", enum_to_string(obj.Type));

    ImGui::PopID();
    ImGui::PopID();
}

void GUIEditor::open_animation_editor() {
    if (m_animation_editor_first) {
        m_animation_editor_first = false;
    }
    
    m_animation_editor_visible = !m_animation_editor_visible;
}
