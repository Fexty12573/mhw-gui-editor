#pragma once

#include <imgui.h>
#include <imgui_internal.h>

#include <algorithm>
#include <format>
#include <span>
#include <vector>

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

inline bool g_last_item_hovered = false;
inline ImGuiID g_last_item_id = 0;

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

    Internal::g_last_item_id = GetID(str_id.data());

    if (!IsItemVisible()) { // If the node is not visible, don't bother formatting the text
        Internal::g_last_item_hovered = false;
        return open;
    }

    Internal::g_last_item_hovered = IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup);

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

template<typename IndexT> requires std::is_integral_v<IndexT>
bool RichTextCombo(std::string_view label, IndexT* selected, std::span<const char* const> items, ImColor item_color) {
    PushStyleColor(ImGuiCol_Text, item_color.Value);
    
    const bool open = BeginCombo(label.data(), items[*selected]);
    Internal::g_last_item_hovered = IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup);
    Internal::g_last_item_id = GetID(label.data());

    if (!open) {
        PopStyleColor();
        return false;
    }

    bool value_changed = false;
    for (IndexT i = 0; i < items.size(); ++i) {
        if (Selectable(items[i], i == *selected)) {
            value_changed = true;
            *selected = i;
        }
    }

    EndCombo();
    PopStyleColor();

    return value_changed;
}

// Returns true if the last RichTextTreeNode or RichTextCombo was hovered
namespace RT {

bool IsItemHovered();

bool BeginPopupContextItem(std::string_view str_id = "", ImGuiPopupFlags popup_flags = ImGuiPopupFlags_MouseButtonRight);

}

}
