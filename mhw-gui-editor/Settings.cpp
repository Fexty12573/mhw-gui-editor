#include "pch.h"
#include "Settings.h"

#include <filesystem>
#include <toml++/toml.h>

Settings::Settings() {
    if (!std::filesystem::exists(DEFAULT_SETTINGS_FILE)) {
        save();
    }
}

bool Settings::load() {
    toml::parse_result result;

    try {
        result = toml::parse_file(DEFAULT_SETTINGS_FILE);
    } catch (const toml::parse_error&) {
        return false;
    }

    ChunkPath = result["General"]["ChunkDirectory"].value_or("");
    NativePath = result["General"]["NativeDirectory"].value_or("");
    Theme = result["General"]["Theme"].value_or("");

    AllowMultipleKV8References = result["Optimization"]["AllowMultipleReferences"]["KV8"].value_or(false);
    AllowMultipleKV32References = result["Optimization"]["AllowMultipleReferences"]["KV32"].value_or(false);
    AllowMultipleKV128References = result["Optimization"]["AllowMultipleReferences"]["KV128"].value_or(false);

    AutoAdjustKeyFrames = result["Utility"]["AutoAdjustKeyFrames"].value_or(false);

    return true;
}

bool Settings::save() {
    toml::table config{};

    config.insert("General", toml::table{
        {"ChunkDirectory", ChunkPath},
        {"NativeDirectory", NativePath},
        {"Theme", Theme}
    });
    config.insert("Optimization", toml::table{
        {"AllowMultipleReferences", toml::table{
            {"KV8", AllowMultipleKV8References},
            {"KV32", AllowMultipleKV32References},
            {"KV128", AllowMultipleKV128References}
        }}
    });
    config.insert("Utility", toml::table{
        {"AutoAdjustKeyFrames", AutoAdjustKeyFrames}
    });

    try {
        std::ofstream(DEFAULT_SETTINGS_FILE) << config;
    } catch (const std::exception&) {
        return false;
    }

    return true;
}
