#pragma once

#include <filesystem>
#include <string>
#include <map>

#include <imgui.h>

class ImThemeManager {
public:
    ImThemeManager();

    void set_theme_directory(const std::filesystem::path& dir);
    void refresh();

    void apply_style(const std::string& name) const;
    const ImGuiStyle& get_style(const std::string& name) const;
    const auto& get_styles() const { return m_styles; }

private:
    ImGuiStyle read_theme(const std::filesystem::path& path);

private:
    std::filesystem::path m_theme_dir;
    std::map<std::string, ImGuiStyle> m_styles;

    ImGuiStyle m_default_style;
};

