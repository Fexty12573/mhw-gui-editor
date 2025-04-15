#pragma once

#include "dti_types.h"
#include "BinaryReader.h"
#include "BinaryWriter.h"
#include "Settings.h"

#include "GUIAnimation.h"
#include "GUISequence.h"
#include "GUIObject.h"
#include "GUIObjectSequence.h"
#include "GUIInitParam.h"
#include "GUIParam.h"
#include "GUIKey.h"
#include "GUIInstance.h"
#include "GUIFlow.h"
#include "GUIFlowProcess.h"
#include "GUITexture.h"
#include "GUIFontFilter.h"
#include "GUIMessage.h"
#include "GUIResource.h"
#include "GUIGeneralResource.h"
#include "GUIVertex.h"

#include <array>
#include <vector>


class GUIEditor;

class GUIFile {
public:
	GUIFile();

	void load_from(BinaryReader& stream);
    void load_resources(
		const std::string& chunk_path, 
		const std::string& native_path,
        const std::string& arcfs_path,
		ID3D11Device* device, 
		ID3D11DeviceContext* context
	);

	void run_data_usage_analysis(bool log_overlapping_offsets = false) const;

    void save_to(BinaryWriter& stream, const Settings& settings) const;

	enum class Version {
		MHW = 144150,
		MHGU = 141077,
	};

	Version get_version() const { return static_cast<Version>(m_header.guiVersion); }
    bool is_mhgu() const { return get_version() == Version::MHGU; }

    void insert_animation(GUIAnimation anim, s32 index = -1, bool update_indices = true);
    void insert_sequence(GUISequence seq, s32 index = -1, bool update_indices = true);
    void insert_object(GUIObject obj, s32 index = -1, bool update_indices = true);
    void insert_object_sequence(GUIObjectSequence objseq, s32 index = -1, bool update_indices = true);
    void insert_init_param(GUIInitParam param, s32 index = -1, bool update_indices = true);
    void insert_param(GUIParam param, s32 index = -1, bool update_indices = true);
    void insert_key(GUIKey key, s32 index = -1, bool update_indices = true);
    void insert_instance(GUIInstance inst, s32 index = -1, bool update_indices = true);
    void insert_flow(GUIFlow flow, s32 index = -1, bool update_indices = true);
    void insert_flow_process(GUIFlowProcess flow, s32 index = -1, bool update_indices = true);
    void insert_texture(GUITexture tex, s32 index = -1, bool update_indices = true);
    void insert_font_filter(std::shared_ptr<GUIFontFilter> filter, s32 index = -1, bool update_indices = true);

    void erase_keys(u32 index, u32 count = 1, bool update_indices = true);

    template<class T> [[nodiscard]] u32 get_unused_id(const std::vector<T>& vec) const {
        return std::ranges::max_element(vec, [](const T& a, const T& b) { return a.ID < b.ID; })->ID + 1;
    }

	friend class GUIEditor;
	friend class Editor;

private:
    u32 get_animation_object_count(u32 anim_index) const;

private:
	union {
		GUIHeader m_header{};
		GUIHeaderMHGU m_header_mhgu;
	};

	std::array<char, 4> m_magic{};
	u32 m_version = 0;
	u32 m_attr = 0;
	time_t m_revision_time = 0;
	u32 m_instance_id = 0;
	u32 m_flow_id = 0;
	u32 m_variable_id = 0;
	u32 m_start_instance_index = 0;

	std::vector<GUIAnimation> m_animations;
	std::vector<GUISequence> m_sequences;
	std::vector<GUIObject> m_objects;
	std::vector<GUIObjectSequence> m_obj_sequences;
	std::vector<GUIInitParam> m_init_params;
	std::vector<GUIParam> m_params;
	std::vector<GUIKey> m_keys;
	std::vector<GUIInstance> m_instances;
	std::vector<GUIFlow> m_flows;
	std::vector<GUIFlowProcess> m_flow_processes;
	std::vector<GUITexture> m_textures;
	std::vector<std::shared_ptr<GUIFontFilter>> m_font_filters;
	std::vector<GUIMessage> m_messages;
	std::vector<GUIResource> m_resources;
	std::vector<GUIGeneralResource> m_general_resources;
    std::vector<GUIVertex> m_vertices;
};

