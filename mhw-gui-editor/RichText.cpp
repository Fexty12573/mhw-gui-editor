#include "pch.h"
#include "RichText.h"

namespace ImGui::Internal {

std::vector<TaggedText> ParseRichText(const std::string& text) {
	std::vector<TaggedText> result;
    bool bold = false;
    bool italic = false;
    bool underline = false;
    bool strike = false;
	std::stack<ImU32> color_stack;

    color_stack.push(ImGui::GetColorU32(ImGuiCol_Text)); // default color

	// XML like tags, e.g.
	// <B>Bold text</B>
	// <C FF0000>Red text</C>

	size_t last_tag_change = 0;

	for (size_t i = 0; i < text.size();) {
		if (text[i] == '<') {
			if (i >= text.size() - 3) {
				// Not enough characters for a tag
				break;
			}

            const ImU32 pre_color = color_stack.top();

			if (text[i + 1] == '/') {
				switch (text[i + 2]) {
				case 'B':
				    bold = false;
					break;
				case 'I':
					italic = false;
					break;
				case 'U':
					underline = false;
					break;
				case 'S':
					strike = false;
					break;
				case 'C':
				    if (color_stack.size() > 1) {
					    color_stack.pop();
				    } else {
					    throw std::runtime_error("Closing color tag without opening tag");
				    }
					break;
				default:
				    ++i;
					continue;
				}

				if (i != last_tag_change) {
					result.push_back({
					    .Text = std::string_view(text.c_str() + last_tag_change, i - last_tag_change),
					    .Bold = bold,
                        .Italic = italic,
                        .Underline = underline,
                        .StrikeThrough = strike,
					    .Color = pre_color
					});
				}

				i += 4; // </#>
				last_tag_change = i;
			} else {
				const auto pre_pos = i;

				switch (text[i + 1]) {
				case 'B':
				    bold = true;
					i += 3;
					break;
				case 'I':
					italic = true;
					i += 3;
					break;
				case 'U':
					underline = true;
					i += 3;
					break;
				case 'S':
					strike = true;
					i += 3;
					break;
				case 'C': {
					const size_t begin_col = i + 3; // <C RRGGBBAA>
					const size_t end_col = text.find_first_of('>', begin_col);
					color_stack.push(std::strtoul(text.c_str() + begin_col, nullptr, 16));
					i = end_col + 1;
					break;
				}
				default:
				    ++i;
					continue;
				}

                if (pre_pos != last_tag_change) {
					result.push_back({
					    .Text = std::string_view(text.c_str() + last_tag_change, pre_pos - last_tag_change),
						.Bold = bold,
						.Italic = italic,
						.Underline = underline,
						.StrikeThrough = strike,
						.Color = pre_color
					});
                }

				last_tag_change = i;
			}
		} else {
			++i;
		}
	}

    if (last_tag_change < text.size()) { // Add the last part of the text
        result.push_back({
            .Text = std::string_view(text.c_str() + last_tag_change, text.size() - last_tag_change),
			.Bold = bold,
		    .Italic = italic,
		    .Underline = underline,
		    .StrikeThrough = strike,
		    .Color = color_stack.top()
        });
    }

    if (color_stack.size() > 1) {
		throw std::runtime_error(std::format("Missing closing color tag in string '{}'", text));
	}

	return result;
}

}
