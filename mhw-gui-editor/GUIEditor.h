#pragma once

#include "Editor.h"
#include "GUIFile.h"
#include "Settings.h"

#include "imgui-notify/imgui_notify.h"

#include <any>
#include <filesystem>
#include <functional>
#include <map>
#include <queue>
#include <vector>

#include <ShObjIdl.h>


struct GenericPopup {
    std::string Title;
    std::function<bool(std::any&)> DrawCallback; // return true if the popup should close
    std::any UserData;
};

struct ParamValidationResult {
	bool InvalidName = false;
    bool InvalidType = false;
    ParamType CorrectType = ParamType::UNKNOWN;

    operator bool() const {
        return !(InvalidName || InvalidType);
    }
};

inline bool operator&(PopupType a, PopupType b) {
    return static_cast<int>(a) & static_cast<int>(b);
}

class GUIEditor {
public:
    GUIEditor(Editor* owner, const std::filesystem::path& file);

	void render(u32 dockspace_id = 0);
	void save_file();
	void save_file_as(const std::filesystem::path& path);

	[[nodiscard]] GUIFile& get_file() { return m_file; }

	[[nodiscard]] std::string get_name() const {
		return m_file_path.filename().string();
	}
    [[nodiscard]] const std::filesystem::path& get_path() const {
        return m_file_path;
    }

	void render_tree_viewer();
	void render_overview() const;
	void render_resource_manager();
	void render_texture_viewer() const;
	void render_object_editor();

private:
	void render_animation(GUIAnimation& anim);
	void render_object(GUIObject& obj, u32 seq_count = 0, GUIAnimation* parent_anim = nullptr);
	void render_sequence(GUISequence& seq) const;
	void render_obj_sequence(GUIObjectSequence& objseq, ObjectType source_object = ObjectType::None, u32 object_id = -1, const GUISequence* seq = nullptr);
	void render_init_param(GUIInitParam& param, ObjectType source_object = ObjectType::None) const;
	void render_param(GUIParam& param, ObjectType source_object = ObjectType::None);
	void render_instance(GUIInstance& inst);
	void render_key(GUIKey& key, ParamType type = ParamType::UNKNOWN) const;
    void render_font_filter(const std::shared_ptr<GUIFontFilter>& filter) const;
    void render_font_filter_shadow(const std::shared_ptr<GUIFontFilterShadow>& filter) const;
    void render_font_filter_border(const std::shared_ptr<GUIFontFilterBorder>& filter) const;
    void render_font_filter_gradation_overlay(const std::shared_ptr<GUIFontFilterGradationOverlay>& filter) const;
    void render_font_filter_distance_field(const std::shared_ptr<GUIFontFilterDistanceField>& filter) const;

    [[nodiscard]] ParamValidationResult is_valid_param(ParamType type, const std::string& name, ObjectType source_object) const;
	void update_indices();

	void open_animation_editor();

	struct DrawCallbackData {
		ID3D11DeviceContext* Context;
		Sampler* Sampler = nullptr;
		ID3D11SamplerState* OriginalState;
	};

private:
    Editor* m_owner;
    App* m_app;

	GUIFile m_file;
    std::filesystem::path m_file_path;

    std::vector<std::unique_ptr<DrawCallbackData>> m_draw_callback_data;

	bool m_first_render = true;
	bool m_animation_editor_first = true;
	bool m_animation_editor_visible = false;
	bool m_object_editor_visible = false;

    // Generic popup
    std::queue<GenericPopup> m_generic_popup_queue;

	int m_selected_object = -1;
    int m_selected_texture = -1;
};

///	TODO: Add an 'About' window and a 'Help' window
///		- About: Show version, credits, etc.
///		- Help: Show hotkeys, object info, etc.
/// DONE: Add a way to add new params to an objsequence
///	TODO: Add a way to add all missing InitParams to an object
///	TODO: Add a way to display GUIResources and GeneralResources
///	TODO: Add a flow editor (Low priority)
///
