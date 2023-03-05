#pragma once

#include <string>


struct Settings {
    static constexpr const char* const DEFAULT_SETTINGS_FILE = "Config.toml";

    Settings();

    bool load();
    bool save();

    std::string ChunkPath;
    std::string NativePath;
    std::string Theme;

    bool AllowMultipleKV8References;
    bool AllowMultipleKV32References;
    bool AllowMultipleKV128References;

    bool AutoAdjustKeyFrames;
};

