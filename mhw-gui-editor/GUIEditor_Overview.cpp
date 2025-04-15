#include "pch.h"
#include "GUIEditor.h"
#include "RichText.h"


void GUIEditor::render_overview() const {
    ImGui::RichText("<C FFC6913F>File:</C> {}", m_file_path.string());
    ImGui::NewLine();

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
}
