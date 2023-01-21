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

    return true;
}

bool Settings::save() {
    toml::table config{};

    config.insert("General", toml::table{
        {"ChunkDirectory", ChunkPath}
    });

    try {
        std::ofstream(DEFAULT_SETTINGS_FILE) << config;
    } catch (const std::exception&) {
        return false;
    }

    return true;
}
