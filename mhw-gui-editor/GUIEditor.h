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
struct Menu {
    std::string Name;
    std::vector<MenuItem> Items;
};

class GUIEditor {
public:
    GUIEditor(App* owner);

	void add_menu_item(const std::string& menu, const MenuItem& item);

	void render(u32 dockspace_id = 0);
	void open_file();
	void save_file();
	void save_file_as();

	[[nodiscard]] GUIFile& get_file() { return m_file; }

private:
	void render_tree_viewer();
	void render_overview() const;
	void render_resource_manager();
	void render_texture_viewer() const;
	void render_object_editor();

	void render_animation(GUIAnimation& anim);
	void render_object(GUIObject& obj, u32 seq_count = 0);
	void render_sequence(GUISequence& seq) const;
	void render_obj_sequence(GUIObjectSequence& objseq);
	void render_init_param(GUIInitParam& param) const;
	void render_param(GUIParam& param);
	void render_instance(GUIInstance& inst);
	void render_key(GUIKey& key, ParamType type = ParamType::UNKNOWN) const;

	void update_indices();

	void select_chunk_dir();
	void select_native_dir();
    [[nodiscard]] std::filesystem::path open_file_dialog(std::wstring_view title, const std::vector<COMDLG_FILTERSPEC>& filters, std::wstring_view default_ext = L"") const;
    [[nodiscard]] std::filesystem::path open_folder_dialog(std::wstring_view title) const;

	void open_animation_editor();

private:
    App* m_owner;
    ImThemeManager m_theme_manager;

	GUIFile m_file;
	std::vector<Menu> m_menu_items;

	bool m_first_render = true;
	bool m_options_menu_open = false;
    bool m_error_popup_select_file_open = false;
	bool m_animation_editor_first = true;
	bool m_animation_editor_visible = false;
	bool m_object_editor_visible = false;

	bool m_error_popup_open = false;
	std::string m_error_popup_message;

	int m_selected_object = -1;
    int m_selected_texture = -1;

	Settings m_settings;
};
