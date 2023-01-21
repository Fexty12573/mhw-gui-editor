#include "pch.h"
#include "ImThemeManager.h"

#include <toml++/toml.h>

ImThemeManager::ImThemeManager() : m_default_style(ImGui::GetStyle()) {}

void ImThemeManager::set_theme_directory(const std::filesystem::path& dir) {
    m_theme_dir = dir;
}

void ImThemeManager::refresh() {
    m_styles.clear();

    for (auto& p : std::filesystem::directory_iterator(m_theme_dir)) {
        if (p.is_regular_file() && p.path().extension() == ".toml") {
            m_styles.emplace(p.path().stem().string(), read_theme(p.path()));
        }
    }
}

void ImThemeManager::apply_default_style() const {
    ImGui::GetStyle() = m_default_style;
}


ImGuiStyle ImThemeManager::read_theme(const std::filesystem::path& path) {
    auto read_vec4 = []<typename T>(const T& table, const std::string& key, const ImVec4& def) {
        if (const auto v = table[key].as_array()) {
            return ImVec4{
                v->get(0)->value_or(def.x),
                v->get(1)->value_or(def.y),
                v->get(2)->value_or(def.z),
                v->get(3)->value_or(def.w)
            };
        }

        return def;
    };
    auto read_vec2 = []<typename T>(const T & table, const std::string & key, const ImVec2 & def) {
        if (const auto v = table[key].as_array()) {
            return ImVec2{
                v->get(0)->value_or(def.x),
                v->get(1)->value_or(def.y)
            };
        }

        return def;
    };

    ImGuiStyle style{};
    toml::parse_result result;

    try {
        result = toml::parse_file(path.string());
    } catch (const toml::parse_error&) {
        throw; // Intentionally rethrow
    }
    
    const auto& general = result["Style"];
    const auto& colors = result["Colors"];


    style.Alpha = general["Alpha"].value_or(m_default_style.Alpha);
    style.DisabledAlpha = general["DisabledAlpha"].value_or(m_default_style.DisabledAlpha);

    style.WindowPadding = read_vec2(general, "WindowPadding", m_default_style.WindowPadding);

    style.WindowRounding = general["WindowRounding"].value_or(m_default_style.WindowRounding);
    style.WindowBorderSize = general["WindowBorderSize"].value_or(m_default_style.WindowBorderSize);

    style.WindowMinSize = read_vec2(general, "WindowMinSize", m_default_style.WindowMinSize);

    style.WindowTitleAlign = read_vec2(general, "WindowTitleAlign", m_default_style.WindowTitleAlign);

    style.WindowMenuButtonPosition = general["WindowMenuButtonPosition"].value_or(m_default_style.WindowMenuButtonPosition);
    style.ChildRounding = general["ChildRounding"].value_or(m_default_style.ChildRounding);
    style.ChildBorderSize = general["ChildBorderSize"].value_or(m_default_style.ChildBorderSize);
    style.PopupRounding = general["PopupRounding"].value_or(m_default_style.PopupRounding);
    style.PopupBorderSize = general["PopupBorderSize"].value_or(m_default_style.PopupBorderSize);
    style.FramePadding = read_vec2(general, "FramePadding", m_default_style.FramePadding);
    style.FrameRounding = general["FrameRounding"].value_or(m_default_style.FrameRounding);
    style.FrameBorderSize = general["FrameBorderSize"].value_or(m_default_style.FrameBorderSize);
    style.ItemSpacing = read_vec2(general, "ItemSpacing", m_default_style.ItemSpacing);
    style.ItemInnerSpacing = read_vec2(general, "ItemInnerSpacing", m_default_style.ItemInnerSpacing);
    style.CellPadding = read_vec2(general, "CellPadding", m_default_style.CellPadding);
    style.TouchExtraPadding = read_vec2(general, "TouchExtraPadding", m_default_style.TouchExtraPadding);

    style.IndentSpacing = general["IndentSpacing"].value_or(m_default_style.IndentSpacing);
    style.ColumnsMinSpacing = general["ColumnsMinSpacing"].value_or(m_default_style.ColumnsMinSpacing);
    style.ScrollbarSize = general["ScrollbarSize"].value_or(m_default_style.ScrollbarSize);
    style.ScrollbarRounding = general["ScrollbarRounding"].value_or(m_default_style.ScrollbarRounding);
    style.GrabMinSize = general["GrabMinSize"].value_or(m_default_style.GrabMinSize);
    style.GrabRounding = general["GrabRounding"].value_or(m_default_style.GrabRounding);
    style.LogSliderDeadzone = general["LogSliderDeadzone"].value_or(m_default_style.LogSliderDeadzone);
    style.TabRounding = general["TabRounding"].value_or(m_default_style.TabRounding);
    style.TabBorderSize = general["TabBorderSize"].value_or(m_default_style.TabBorderSize);
    style.TabMinWidthForCloseButton = general["TabMinWidthForCloseButton"].value_or(m_default_style.TabMinWidthForCloseButton);
    style.ColorButtonPosition = general["ColorButtonPosition"].value_or(m_default_style.ColorButtonPosition);

    style.ButtonTextAlign = read_vec2(general, "ButtonTextAlign", m_default_style.ButtonTextAlign);

    style.SelectableTextAlign = read_vec2(general, "SelectableTextAlign", m_default_style.SelectableTextAlign);

    style.DisplayWindowPadding = read_vec2(general, "DisplayWindowPadding", m_default_style.DisplayWindowPadding);

    style.DisplaySafeAreaPadding = read_vec2(general, "DisplaySafeAreaPadding", m_default_style.DisplaySafeAreaPadding);

    style.MouseCursorScale = general["MouseCursorScale"].value_or(m_default_style.MouseCursorScale);
    style.AntiAliasedLines = general["AntiAliasedLines"].value_or(m_default_style.AntiAliasedLines);
    style.AntiAliasedLinesUseTex = general["AntiAliasedLinesUseTex"].value_or(m_default_style.AntiAliasedLinesUseTex);
    style.AntiAliasedFill = general["AntiAliasedFill"].value_or(m_default_style.AntiAliasedFill);
    style.CurveTessellationTol = general["CurveTessellationTol"].value_or(m_default_style.CurveTessellationTol);
    style.CircleTessellationMaxError = general["CircleTessellationMaxError"].value_or(m_default_style.CircleTessellationMaxError);

    style.Colors[ImGuiCol_Text] = read_vec4(colors, "Text", m_default_style.Colors[ImGuiCol_Text]);
    style.Colors[ImGuiCol_TextDisabled] = read_vec4(colors, "TextDisabled", m_default_style.Colors[ImGuiCol_TextDisabled]);
    style.Colors[ImGuiCol_WindowBg] = read_vec4(colors, "WindowBg", m_default_style.Colors[ImGuiCol_WindowBg]);              
    style.Colors[ImGuiCol_ChildBg] = read_vec4(colors, "ChildBg", m_default_style.Colors[ImGuiCol_ChildBg]);               
    style.Colors[ImGuiCol_PopupBg] = read_vec4(colors, "PopupBg", m_default_style.Colors[ImGuiCol_PopupBg]);               
    style.Colors[ImGuiCol_Border] = read_vec4(colors, "Border", m_default_style.Colors[ImGuiCol_Border]);
    style.Colors[ImGuiCol_BorderShadow] = read_vec4(colors, "BorderShadow", m_default_style.Colors[ImGuiCol_BorderShadow]);
    style.Colors[ImGuiCol_FrameBg] = read_vec4(colors, "FrameBg", m_default_style.Colors[ImGuiCol_FrameBg]);               
    style.Colors[ImGuiCol_FrameBgHovered] = read_vec4(colors, "FrameBgHovered", m_default_style.Colors[ImGuiCol_FrameBgHovered]);
    style.Colors[ImGuiCol_FrameBgActive] = read_vec4(colors, "FrameBgActive", m_default_style.Colors[ImGuiCol_FrameBgActive]);
    style.Colors[ImGuiCol_TitleBg] = read_vec4(colors, "TitleBg", m_default_style.Colors[ImGuiCol_TitleBg]);
    style.Colors[ImGuiCol_TitleBgActive] = read_vec4(colors, "TitleBgActive", m_default_style.Colors[ImGuiCol_TitleBgActive]);
    style.Colors[ImGuiCol_TitleBgCollapsed] = read_vec4(colors, "TitleBgCollapsed", m_default_style.Colors[ImGuiCol_TitleBgCollapsed]);
    style.Colors[ImGuiCol_MenuBarBg] = read_vec4(colors, "MenuBarBg", m_default_style.Colors[ImGuiCol_MenuBarBg]);
    style.Colors[ImGuiCol_ScrollbarBg] = read_vec4(colors, "ScrollbarBg", m_default_style.Colors[ImGuiCol_ScrollbarBg]);
    style.Colors[ImGuiCol_ScrollbarGrab] = read_vec4(colors, "ScrollbarGrab", m_default_style.Colors[ImGuiCol_ScrollbarGrab]);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = read_vec4(colors, "ScrollbarGrabHovered", m_default_style.Colors[ImGuiCol_ScrollbarGrabHovered]);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = read_vec4(colors, "ScrollbarGrabActive", m_default_style.Colors[ImGuiCol_ScrollbarGrabActive]);
    style.Colors[ImGuiCol_CheckMark] = read_vec4(colors, "CheckMark", m_default_style.Colors[ImGuiCol_CheckMark]);
    style.Colors[ImGuiCol_SliderGrab] = read_vec4(colors, "SliderGrab", m_default_style.Colors[ImGuiCol_SliderGrab]);
    style.Colors[ImGuiCol_SliderGrabActive] = read_vec4(colors, "SliderGrabActive", m_default_style.Colors[ImGuiCol_SliderGrabActive]);
    style.Colors[ImGuiCol_Button] = read_vec4(colors, "Button", m_default_style.Colors[ImGuiCol_Button]);
    style.Colors[ImGuiCol_ButtonHovered] = read_vec4(colors, "ButtonHovered", m_default_style.Colors[ImGuiCol_ButtonHovered]);
    style.Colors[ImGuiCol_ButtonActive] = read_vec4(colors, "ButtonActive", m_default_style.Colors[ImGuiCol_ButtonActive]);
    style.Colors[ImGuiCol_Header] = read_vec4(colors, "Header", m_default_style.Colors[ImGuiCol_Header]);                
    style.Colors[ImGuiCol_HeaderHovered] = read_vec4(colors, "HeaderHovered", m_default_style.Colors[ImGuiCol_HeaderHovered]);
    style.Colors[ImGuiCol_HeaderActive] = read_vec4(colors, "HeaderActive", m_default_style.Colors[ImGuiCol_HeaderActive]);
    style.Colors[ImGuiCol_Separator] = read_vec4(colors, "Separator", m_default_style.Colors[ImGuiCol_Separator]);
    style.Colors[ImGuiCol_SeparatorHovered] = read_vec4(colors, "SeparatorHovered", m_default_style.Colors[ImGuiCol_SeparatorHovered]);
    style.Colors[ImGuiCol_SeparatorActive] = read_vec4(colors, "SeparatorActive", m_default_style.Colors[ImGuiCol_SeparatorActive]);
    style.Colors[ImGuiCol_ResizeGrip] = read_vec4(colors, "ResizeGrip", m_default_style.Colors[ImGuiCol_ResizeGrip]);
    style.Colors[ImGuiCol_ResizeGripHovered] = read_vec4(colors, "ResizeGripHovered", m_default_style.Colors[ImGuiCol_ResizeGripHovered]);
    style.Colors[ImGuiCol_ResizeGripActive] = read_vec4(colors, "ResizeGripActive", m_default_style.Colors[ImGuiCol_ResizeGripActive]);
    style.Colors[ImGuiCol_Tab] = read_vec4(colors, "Tab", m_default_style.Colors[ImGuiCol_Tab]);
    style.Colors[ImGuiCol_TabHovered] = read_vec4(colors, "TabHovered", m_default_style.Colors[ImGuiCol_TabHovered]);
    style.Colors[ImGuiCol_TabActive] = read_vec4(colors, "TabActive", m_default_style.Colors[ImGuiCol_TabActive]);
    style.Colors[ImGuiCol_TabUnfocused] = read_vec4(colors, "TabUnfocused", m_default_style.Colors[ImGuiCol_TabUnfocused]);
    style.Colors[ImGuiCol_TabUnfocusedActive] = read_vec4(colors, "TabUnfocusedActive", m_default_style.Colors[ImGuiCol_TabUnfocusedActive]);
    style.Colors[ImGuiCol_DockingPreview] = read_vec4(colors, "DockingPreview", m_default_style.Colors[ImGuiCol_DockingPreview]);        
    style.Colors[ImGuiCol_DockingEmptyBg] = read_vec4(colors, "DockingEmptyBg", m_default_style.Colors[ImGuiCol_DockingEmptyBg]);        
    style.Colors[ImGuiCol_PlotLines] = read_vec4(colors, "PlotLines", m_default_style.Colors[ImGuiCol_PlotLines]);
    style.Colors[ImGuiCol_PlotLinesHovered] = read_vec4(colors, "PlotLinesHovered", m_default_style.Colors[ImGuiCol_PlotLinesHovered]);
    style.Colors[ImGuiCol_PlotHistogram] = read_vec4(colors, "PlotHistogram", m_default_style.Colors[ImGuiCol_PlotHistogram]);
    style.Colors[ImGuiCol_PlotHistogramHovered] = read_vec4(colors, "PlotHistogramHovered", m_default_style.Colors[ImGuiCol_PlotHistogramHovered]);
    style.Colors[ImGuiCol_TableHeaderBg] = read_vec4(colors, "TableHeaderBg", m_default_style.Colors[ImGuiCol_TableHeaderBg]);         
    style.Colors[ImGuiCol_TableBorderStrong] = read_vec4(colors, "TableBorderStrong", m_default_style.Colors[ImGuiCol_TableBorderStrong]);     
    style.Colors[ImGuiCol_TableBorderLight] = read_vec4(colors, "TableBorderLight", m_default_style.Colors[ImGuiCol_TableBorderLight]);      
    style.Colors[ImGuiCol_TableRowBg] = read_vec4(colors, "TableRowBg", m_default_style.Colors[ImGuiCol_TableRowBg]);            
    style.Colors[ImGuiCol_TableRowBgAlt] = read_vec4(colors, "TableRowBgAlt", m_default_style.Colors[ImGuiCol_TableRowBgAlt]);         
    style.Colors[ImGuiCol_TextSelectedBg] = read_vec4(colors, "TextSelectedBg", m_default_style.Colors[ImGuiCol_TextSelectedBg]);
    style.Colors[ImGuiCol_DragDropTarget] = read_vec4(colors, "DragDropTarget", m_default_style.Colors[ImGuiCol_DragDropTarget]);
    style.Colors[ImGuiCol_NavHighlight] = read_vec4(colors, "NavHighlight", m_default_style.Colors[ImGuiCol_NavHighlight]);          
    style.Colors[ImGuiCol_NavWindowingHighlight] = read_vec4(colors, "NavWindowingHighlight", m_default_style.Colors[ImGuiCol_NavWindowingHighlight]); 
    style.Colors[ImGuiCol_NavWindowingDimBg] = read_vec4(colors, "NavWindowingDimBg", m_default_style.Colors[ImGuiCol_NavWindowingDimBg]);     
    style.Colors[ImGuiCol_ModalWindowDimBg] = read_vec4(colors, "ModalWindowDimBg", m_default_style.Colors[ImGuiCol_ModalWindowDimBg]);

    return style;
}

void ImThemeManager::apply_style(const std::string& name) const {
    if (m_styles.contains(name)) {
        ImGui::GetStyle() = m_styles.at(name);
    }
}

const ImGuiStyle& ImThemeManager::get_style(const std::string& name) const {
    if (m_styles.contains(name)) {
        return m_styles.at(name);
    }

    return m_default_style;
}
