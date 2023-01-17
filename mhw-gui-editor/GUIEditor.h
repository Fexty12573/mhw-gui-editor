#pragma once

#include "GUIFile.h"

#include <functional>
#include <map>
#include <vector>

struct MenuItem {
	const char* Name;
	const char* Shortcut;
	std::function<void(GUIEditor*)> Callback;
};

class GUIEditor {
public:
	GUIEditor();

	void add_menu_item(const std::string& menu, MenuItem item);

	void render(u32 dockspace_id = 0);
	void open_file();

	[[nodiscard]] GUIFile& get_file() { return m_file; }

private:
	void render_animation(GUIAnimation& anim);
	void render_object(GUIObject& obj);
	void render_sequence(GUISequence& seq);

	void update_indices();

	void open_animation_editor();

private:
	GUIFile m_file;
	std::map<std::string, std::vector<MenuItem>> m_menu_items;

	bool m_first_render = true;
	
	bool m_animation_editor_first = true;
	bool m_animation_editor_visible = false;
};
