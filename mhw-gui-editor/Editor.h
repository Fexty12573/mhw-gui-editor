#pragma once

#include <any>
#include <functional>
#include <memory>
#include <queue>
#include <shtypes.h>
#include <vector>

#include "ImThemeManager.h"
#include "Sampler.h"
#include "Settings.h"


class App;
class Editor;
class GUIEditor;

struct MenuItem {
    const char* Name;
    const char* Shortcut;
    std::function<void(Editor*)> Callback;
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
    using CallbackType = std::function<void(YesNoCancelPopupResult, std::any&)>;

    std::string Title;
    std::string Message;
    PopupType Type;
    CallbackType Callback;
    std::any UserData;
};

class Editor {
public:
    explicit Editor(App* owner);
    ~Editor() = default;

    [[nodiscard]] App* get_app() const { return m_owner; }
    [[nodiscard]] const Settings& get_settings() const { return m_settings; }

    void add_menu_item(const std::string& menu, const MenuItem& item);
    void add_popup(const std::string& title, const std::string& message, PopupType type,
        const YesNoCancelPopup::CallbackType& callback, std::any user_data = {});
    void error_file_open() { m_error_popup_select_file_open = true; }
    void error_popup(const std::string& message) {
        m_error_popup_open = true;
        m_error_popup_message = message;
    }

    void render(u32 dockspace_id = 0);

    void open_file();
    void open_file(const std::filesystem::path& path);
    void save_file() const;
    void save_file_as() const;

    std::weak_ptr<GUIEditor> get_active_editor() const {
        return m_editors.empty() || m_active_editor == -1
            ? std::weak_ptr<GUIEditor>()
            : m_editors[m_active_editor];
    }

    const auto& get_object_info() const { return m_object_info; }
    const auto& get_object_info2() const { return m_object_info2; }
    const auto& get_samplers() const { return m_samplers; }

    [[nodiscard]] std::filesystem::path open_file_dialog(std::wstring_view title, const std::vector<COMDLG_FILTERSPEC>& filters, std::wstring_view default_ext = L"") const;
    [[nodiscard]] std::filesystem::path open_folder_dialog(std::wstring_view title) const;

private:
    void render_menu_bar();
    void render_options_menu();
    void render_popups();
    void render_tree_viewer();
    void render_overview() const;
    void render_resource_manager() const;
    void render_texture_viewer() const;
    void render_object_editor();
    void render_preview();

    void select_chunk_dir();
    void select_native_dir();
    void select_arcfs_dir();

    void dump_object_data() const;

    void handle_file_drop(HDROP drop);

private:
    App* m_owner;
    ImThemeManager m_theme_manager;

    bool m_first_render = true;
    bool m_options_menu_open = false;
    bool m_error_popup_select_file_open = false;
    bool m_error_popup_open = false;
    std::string m_error_popup_message;

    // Currently open editors
    std::vector<std::shared_ptr<GUIEditor>> m_editors;
    size_t m_active_editor = -1;

    // yes/no/cancel popup
    std::queue<YesNoCancelPopup> m_popup_queue;

    // Menu items
    std::vector<Menu> m_menu_items;

    // Available texture samplers
    std::map<SamplerMode, std::unique_ptr<Sampler>> m_samplers;

    std::unordered_map<std::string, std::shared_ptr<ObjectInfo>> m_object_info;
    std::unordered_map<ObjectType, std::shared_ptr<ObjectInfo>> m_object_info2;

    //Microsoft::WRL::ComPtr<ID3D11PixelShader> m_wrap_shader;
    //Microsoft::WRL::ComPtr<ID3D11Buffer> m_wrap_shader_buffer;

    Settings m_settings;
};
