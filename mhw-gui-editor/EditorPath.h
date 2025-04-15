#pragma once

#include <string_view>
#include <memory>
#include <regex>
#include <unordered_map>

enum class EditorPathCategory {
    Animations,
    Objects,
    Sequences,
    ObjectSequences,
    InitParams,
    Params,
    Instances,
    FontFilters,
    Keyframes,
    // --- Not yet supported ---
    FlowProcesses,
    Flows,
    GeneralResources,
    Messages,
    Resources,
    Textures,
    Vertices
    // -------------------------
};

enum class EditorPathAccessorType {
    Index,
    Id,
    Name
};

struct EditorPath {
    EditorPathCategory Category;
    EditorPathAccessorType AccessorType;
    union {
        u32 Index = 0;
        u32 Id = 0;
        std::string_view Name;
    };

    std::unique_ptr<EditorPath> Child;

    static EditorPath parse(std::string_view path) {
        EditorPath result{};

        const auto first_slash = path.find_first_of('/');
        const auto element = path.substr(0, first_slash);

        std::smatch match;
        std::regex_search(element, match, ElementRegex);
        result.Category = parse_category(element);

    }

private:
    static EditorPathCategory parse_category(std::string_view element) {
        return CategoryNames.at(element);
    }

    static inline const std::unordered_map<std::string_view, EditorPathCategory> CategoryNames = {
       { "Animations", EditorPathCategory::Animations },
       { "Objects", EditorPathCategory::Objects },
       { "Sequences", EditorPathCategory::Sequences },
       { "ObjectSequences", EditorPathCategory::ObjectSequences },
       { "InitParams", EditorPathCategory::InitParams },
       { "Params", EditorPathCategory::Params },
       { "Instances", EditorPathCategory::Instances },
       { "FontFilters", EditorPathCategory::FontFilters },
       { "Keyframes", EditorPathCategory::Keyframes },
       { "FlowProcesses", EditorPathCategory::FlowProcesses },
       { "Flows", EditorPathCategory::Flows },
       { "GeneralResources", EditorPathCategory::GeneralResources },
       { "Messages", EditorPathCategory::Messages },
       { "Resources", EditorPathCategory::Resources },
       { "Textures", EditorPathCategory::Textures },
       { "Vertices", EditorPathCategory::Vertices }
   };

    static inline const std::regex ElementRegex = std::regex(R"(^([^:]+):([ixn]):(.+)$)");
};
