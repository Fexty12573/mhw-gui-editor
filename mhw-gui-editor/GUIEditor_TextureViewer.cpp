#include "pch.h"
#include "GUIEditor.h"


void GUIEditor::render_texture_viewer() const {
    if (m_selected_texture != -1 && m_selected_texture < m_file.m_textures.size()) {
        const auto& tex = m_file.m_textures[m_selected_texture];
        if (tex.RenderTexture.is_valid()) {
            ImGui::Image(tex.RenderTexture.get_view().Get(), ImVec2{ static_cast<float>(tex.Width), static_cast<float>(tex.Height) });
            if (ImGui::IsItemHovered()) {
                const ImVec2 pos = ImGui::GetWindowPos();
                const ImVec2 padding = ImGui::GetStyle().WindowPadding;
                const ImVec2 mouse = ImGui::GetMousePos();
                const ImVec2 relpos = {
                    mouse.x - (pos.x + padding.x),
                    mouse.y - (pos.y + padding.y) - 25.0f // TODO: Find out where that extra 25 is coming from
                };

                ImGui::SetTooltip("(%.2f, %.2f)", relpos.x, relpos.y);
            }
        } else {
            ImGui::TextColored({ 1.0f, 0.2f, 0.2f, 1.0f }, "Error: Couldn't load texture!");
            ImGui::Text("Make sure the file exists and you have the correct chunk path set under Tools -> Options");
        }
    }
}
