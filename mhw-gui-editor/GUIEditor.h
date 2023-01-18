#pragma once

#include "GUIFile.h"
#include "Texture.h"

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
	void render_tree_viewer();
	void render_overview();
	void render_resource_manager();
	void render_texture_viewer();

	void render_animation(GUIAnimation& anim);
	void render_object(GUIObject& obj, u32 seq_count = 0);
	void render_sequence(GUISequence& seq);
	void render_obj_sequence(GUIObjectSequence& objseq);
	void render_init_param(GUIInitParam& param);
	void render_param(GUIParam& param);

	void update_indices();

	void select_chunk_dir();

	void open_animation_editor();

private:
	GUIFile m_file;
	std::map<std::string, std::vector<MenuItem>> m_menu_items;

	bool m_first_render = true;
	bool m_options_menu_open = false;
	bool m_animation_editor_first = true;
	bool m_animation_editor_visible = false;

	std::string m_chunk_dir;
};
