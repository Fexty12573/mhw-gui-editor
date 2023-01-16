#include "pch.h"
#include "RichText.h"

namespace ImGui::Internal {

std::vector<TaggedText> ParseRichText(const std::string& text) {
	std::vector<TaggedText> result;
	std::vector<TagType> tag_stack;
	std::stack<ImU32> color_stack;

	// XML like tags, e.g.
	// <B>Bold text</B>
	// <C FF0000>Red text</C>

	size_t last_tag_change = 0;

	for (size_t i = 0; i < text.size();) {
		auto& c = text[i];

		if (c == '<') {
			if (i >= text.size() - 3) {
				// Not enough characters for a tag
				break;
			}

			TagType tag;

			if (text[i + 1] == '/') {
				if (tag_stack.empty()) {
                    throw std::runtime_error("Invalid rich text: closing tag without opening tag");
				}

				switch (text[i + 2]) {
				case 'B':
					tag = TagType::Bold;
					break;
				case 'I':
					tag = TagType::Italic;
					break;
				case 'U':
					tag = TagType::Underline;
					break;
				case 'S':
					tag = TagType::StrikeThrough;
					break;
				case 'C':
					tag = TagType::Color;
					break;
				default:
				    ++i;
					continue;
				}

				const auto iter = std::find_if(tag_stack.rbegin(), tag_stack.rend(), [tag](TagType t) {
					return t == tag;
				});

				if (i != last_tag_change) {
					result.push_back({
					    .Text = std::string_view(text.c_str() + last_tag_change, i - last_tag_change),
					    .Tags = tag_stack,
					    .Param = {.Color = color_stack.empty() ? 0 : color_stack.top() }
					});
				}

				if (tag == TagType::Color) {
					color_stack.pop();
				}
				if (iter != tag_stack.rend()) {
					tag_stack.erase(std::next(iter).base());
				}

				i += 4; // </#>
				last_tag_change = i;
			} else {
				const auto pre_pos = i;

				switch (text[i + 1]) {
				case 'B':
					tag_stack.push_back(TagType::Bold);
					i += 3;
					break;
				case 'I':
					tag_stack.push_back(TagType::Italic);
					i += 3;
					break;
				case 'U':
					tag_stack.push_back(TagType::Underline);
					i += 3;
					break;
				case 'S':
					tag_stack.push_back(TagType::StrikeThrough);
					i += 3;
					break;
				case 'C': {
					tag_stack.push_back(TagType::Color);

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
					    .Tags = {},
					    .Param = {.Color = color_stack.empty() ? 0 : color_stack.top() }
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
            .Tags = tag_stack,
            .Param = {.Color = color_stack.empty() ? 0 : color_stack.top() }
        });
    }

	if (!tag_stack.empty() || !color_stack.empty()) {
		throw std::runtime_error(std::format("Missing closing tag in string '{}'", text));
	}

	return result;
}

}
