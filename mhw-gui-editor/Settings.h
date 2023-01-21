#pragma once

#include <string>


struct Settings {
    static constexpr const char* const DEFAULT_SETTINGS_FILE = "Config.toml";

    Settings();

    bool load();
    bool save();

    std::string ChunkPath;
    std::string Theme;
};

