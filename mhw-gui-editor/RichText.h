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
    bool Bold = false;
    bool Italic = false;
    bool Underline = false;
    bool StrikeThrough = false;
    ImU32 Color = 0xFFFFFFFF;
};

std::vector<TaggedText> ParseRichText(const std::string& text);

}

template<typename ...Args>
void RichText(std::string_view fmt, Args&&... args) {
	const std::string formatted = std::vformat(fmt, std::make_format_args(args...));
    const auto tagged = Internal::ParseRichText(formatted);

	for (const auto& msg : tagged) {
		PushStyleColor(ImGuiCol_Text, ImColor(msg.Color).Value);
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
        PushStyleColor(ImGuiCol_Text, ImColor(msg.Color).Value);
        TextEx(msg.Text.data(), msg.Text.data() + msg.Text.size(), ImGuiTextFlags_NoWidthForLargeClippedText);
        PopStyleColor();
        SameLine(0.0f, 0.0f);
    }

	NewLine();

    return open;
}

}
