#pragma once

#include "dti_types.h"
#include "BinaryReader.h"
#include "BinaryWriter.h"

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
    void load_resources(const std::string& chunk_path, ID3D11Device* device, ID3D11DeviceContext* context);

    void save_to(BinaryWriter& stream);

	friend class GUIEditor;

private:
	GUIHeader m_header{};

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
	std::vector<u8> m_extend_data;
};

