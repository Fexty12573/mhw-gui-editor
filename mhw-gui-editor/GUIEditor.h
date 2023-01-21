#pragma once

#include "GUIFile.h"
#include "Settings.h"
#include "ImThemeManager.h"

#include <filesystem>
#include <functional>
#include <map>
#include <vector>

#include <ShObjIdl.h>

class App;

struct MenuItem {
	const char* Name;
	const char* Shortcut;
	std::function<void(GUIEditor*)> Callback;
};

class GUIEditor {
public:
    GUIEditor(App* owner);

	void add_menu_item(const std::string& menu, const MenuItem& item);

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
    std::filesystem::path open_file_dialog(std::wstring_view title, const std::vector<COMDLG_FILTERSPEC>& filters, std::wstring_view default_ext = L"") const;

	void open_animation_editor();

private:
    App* m_owner;
    ImThemeManager m_theme_manager;

	GUIFile m_file;
	std::map<std::string, std::vector<MenuItem>> m_menu_items;

	bool m_first_render = true;
	bool m_options_menu_open = false;
    bool m_error_popup_select_file_open = false;
	bool m_animation_editor_first = true;
	bool m_animation_editor_visible = false;

    int m_selected_texture = -1;

	Settings m_settings;
};
