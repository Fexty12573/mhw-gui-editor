#pragma once

#include "GUIFile.h"
#include "Settings.h"
#include "ImThemeManager.h"

#include <any>
#include <filesystem>
#include <functional>
#include <map>
#include <queue>
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
struct ObjectInfo {
	std::string Name;
	ObjectType Type;
	std::unordered_map<std::string, ParamType> Params;
};

enum class YesNoCancelPopupResult {
    Yes,
    No,
    Cancel,
	Ok
};
enum class PopupType {
	Yes = 1,
    No = 2,
    Cancel = 4,
    Ok = 8,

    YesNo = Yes | No,
    YesNoCancel = Yes | No | Cancel,
    YesCancel = Yes | Cancel,
    OkCancel = Ok | Cancel
};
struct YesNoCancelPopup {
    std::string Title;
    std::string Message;
	PopupType Type;
    std::function<void(YesNoCancelPopupResult, std::any&)> Callback;
	std::any UserData;
};
struct GenericPopup {
    std::string Title;
    std::function<bool(std::any&)> DrawCallback; // return true if the popup should close
    std::any UserData;
};

struct ParamValidationResult {
	bool InvalidName = false;
    bool InvalidType = false;
    ParamType CorrectType = ParamType::UNKNOWN;
};

inline bool operator&(PopupType a, PopupType b) {
    return static_cast<int>(a) & static_cast<int>(b);
}

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
	void render_obj_sequence(GUIObjectSequence& objseq, ObjectType source_object = ObjectType::None, u32 object_id = -1);
	void render_init_param(GUIInitParam& param, ObjectType source_object = ObjectType::None) const;
	void render_param(GUIParam& param, ObjectType source_object = ObjectType::None);
	void render_instance(GUIInstance& inst);
	void render_key(GUIKey& key, ParamType type = ParamType::UNKNOWN) const;

    [[nodiscard]] ParamValidationResult is_valid_param(ParamType type, const std::string& name, ObjectType source_object) const;
	void update_indices();

	void select_chunk_dir();
	void select_native_dir();
    [[nodiscard]] std::filesystem::path open_file_dialog(std::wstring_view title, const std::vector<COMDLG_FILTERSPEC>& filters, std::wstring_view default_ext = L"") const;
    [[nodiscard]] std::filesystem::path open_folder_dialog(std::wstring_view title) const;

	void dump_object_data() const;

	void open_animation_editor();

private:
    App* m_owner;
    ImThemeManager m_theme_manager;

	GUIFile m_file;
    std::filesystem::path m_file_path;

	std::vector<Menu> m_menu_items;

    std::unordered_map<std::string, std::shared_ptr<ObjectInfo>> m_object_info;
    std::unordered_map<ObjectType, std::shared_ptr<ObjectInfo>> m_object_info2;

	bool m_first_render = true;
	bool m_options_menu_open = false;
    bool m_error_popup_select_file_open = false;
	bool m_animation_editor_first = true;
	bool m_animation_editor_visible = false;
	bool m_object_editor_visible = false;

    // yes/no/cancel popup
    std::queue<YesNoCancelPopup> m_popup_queue;

    // Generic popup
    std::queue<GenericPopup> m_generic_popup_queue;

	bool m_error_popup_open = false;
	std::string m_error_popup_message;

	int m_selected_object = -1;
    int m_selected_texture = -1;

	Settings m_settings;
};

///	TODO: Add an 'About' window and a 'Help' window
///		- About: Show version, credits, etc.
///		- Help: Show hotkeys, object info, etc.
/// DONE: Add a way to add new params to an objsequence
///	TODO: Add a way to add all missing InitParams to an object
///	TODO: Add a way to display GUIResources and GeneralResources
///	TODO: Add a flow editor (Low priority)
///
