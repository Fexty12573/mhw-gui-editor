#pragma once

#include <imgui.h>
#include <imgui_internal.h>

#include <algorithm>
#include <format>
#include <stack>
#include <vector>
#include <stdexcept>

namespace ImGui {

namespace Internal {

enum class TagType {
	None,
	Bold, // <B>
	Italic, // <I>
	Underline, // <U>
	StrikeThrough, // <S>
	Color // <C AABBGGRR>
};

struct TaggedText {
	std::string_view Text;
	std::vector<TagType> Tags;

	union {
		ImU32 Color;
		std::string_view Link;
	} Param;
};

std::vector<TaggedText> ParseRichText(const std::string& text);

}

template<typename ...Args>
void RichText(std::string_view fmt, Args&&... args) {
	const std::string formatted = std::vformat(fmt, std::make_format_args(args...));
    const auto tagged = Internal::ParseRichText(formatted);

	for (const auto& msg : tagged) {
		bool bold = false;
		bool italic = false;
		bool underline = false;
		bool strikethrough = false;
		ImU32 color = 0xFFFFFFFF;

		for (const auto tag : msg.Tags) {
			switch (tag) {
			case Internal::TagType::None: break;
			case Internal::TagType::Bold: bold = true; break;
			case Internal::TagType::Italic: italic = true; break;
			case Internal::TagType::Underline: underline = true; break;
			case Internal::TagType::StrikeThrough: strikethrough = true; break;
			case Internal::TagType::Color: color = msg.Param.Color; break;
			default: break;
			}
		}

		PushStyleColor(ImGuiCol_Text, ImColor(color).Value);
        TextEx(msg.Text.data(), msg.Text.data() + msg.Text.size(), ImGuiTextFlags_NoWidthForLargeClippedText);
		PopStyleColor();
        SameLine(0.0f, 0.0f);
	}

	NewLine();
}

template<typename ...Args>
bool RichTextTreeNode(std::string_view str_id, std::string_view fmt, Args&&... args) {
    const bool open = TreeNodeEx(str_id.data(), ImGuiTreeNodeFlags_SpanFullWidth , ""); // Display only the arrow

    const std::string formatted = std::vformat(fmt, std::make_format_args(args...));
    const auto tagged = Internal::ParseRichText(formatted);

	ImGui::SameLine();

    for (const auto& msg : tagged) {
        bool bold = false;
        bool italic = false;
        bool underline = false;
        bool strikethrough = false;
        ImU32 color = 0xFFFFFFFF;

        for (const auto tag : msg.Tags) {
            switch (tag) {
            case Internal::TagType::None: break;
            case Internal::TagType::Bold: bold = true; break;
            case Internal::TagType::Italic: italic = true; break;
            case Internal::TagType::Underline: underline = true; break;
            case Internal::TagType::StrikeThrough: strikethrough = true; break;
            case Internal::TagType::Color: color = msg.Param.Color; break;
            default: break;
            }
        }

        PushStyleColor(ImGuiCol_Text, ImColor(color).Value);
        TextEx(msg.Text.data(), msg.Text.data() + msg.Text.size(), ImGuiTextFlags_NoWidthForLargeClippedText);
        PopStyleColor();
        SameLine(0.0f, 0.0f);
    }

	NewLine();

    return open;
}

}
