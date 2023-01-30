#include "pch.h"
#include "GUIFile.h"
#include "GUITypes.h"
#include "Console.h"

#include <filesystem>
#include <ranges>

GUIFile::GUIFile() = default;

void GUIFile::load_from(BinaryReader& stream) {
    m_animations.clear();
    m_sequences.clear();
    m_objects.clear();
    m_obj_sequences.clear();
    m_init_params.clear();
    m_params.clear();
    m_keys.clear();
    m_instances.clear();
    m_flows.clear();
    m_flow_processes.clear();
    m_textures.clear();
    m_font_filters.clear();
    m_messages.clear();
    m_resources.clear();
    m_general_resources.clear();
    m_vertices.clear();

    const auto header = stream.read<GUIHeader>();

    std::memcpy(m_magic.data(), header.fileType, m_magic.size());
    m_version = header.guiVersion;
    m_attr = header.attr;
    m_revision_time = header.revisionDate;
    m_instance_id = header.instanceID;
    m_flow_id = header.flowID;
    m_variable_id = header.variableID;
    m_start_instance_index = header.startInstanceIndex;

    stream.seek_absolute(static_cast<s64>(header.animationOffset));
    for (auto i = 0u; i < header.animationNum; ++i) {
        m_animations.emplace_back(GUIAnimation::read(stream, static_cast<s64>(header.stringOffset)));
    }

    stream.seek_absolute(static_cast<s64>(header.sequenceOffset));
    for (auto i = 0u; i < header.sequenceNum; ++i) {
        m_sequences.emplace_back(GUISequence::read(stream, static_cast<s64>(header.stringOffset)));
    }

    stream.seek_absolute(static_cast<s64>(header.objectOffset));
    for (auto i = 0u; i < header.objectNum; ++i) {
        m_objects.emplace_back(GUIObject::read(stream, header));
    }

    stream.seek_absolute(static_cast<s64>(header.objSequenceOffset));
    for (auto i = 0u; i < header.objSequenceNum; ++i) {
        m_obj_sequences.emplace_back(GUIObjectSequence::read(stream));
    }

    stream.seek_absolute(static_cast<s64>(header.initParamOffset));
    for (auto i = 0u; i < header.initParamNum; ++i) {
        m_init_params.emplace_back(GUIInitParam::read(stream, header));
    }

    stream.seek_absolute(static_cast<s64>(header.paramOffset));
    for (auto i = 0u; i < header.paramNum; ++i) {
        m_params.emplace_back(GUIParam::read(stream, header));
    }

    stream.seek_absolute(static_cast<s64>(header.instanceOffset));
    for (auto i = 0u; i < header.instanceNum; ++i) {
        m_instances.emplace_back(GUIInstance::read(stream, header));
    }

    stream.seek_absolute(static_cast<s64>(header.flowOffset));
    for (auto i = 0u; i < header.flowNum; ++i) {
        m_flows.emplace_back(GUIFlow::read(stream, header));
    }

    stream.seek_absolute(static_cast<s64>(header.flowProcessOffset));
    for (auto i = 0u; i < header.flowProcessNum; ++i) {
        m_flow_processes.emplace_back(GUIFlowProcess::read(stream, header));
    }

    stream.seek_absolute(static_cast<s64>(header.textureOffset));
    for (auto i = 0u; i < header.textureNum; ++i) {
        m_textures.emplace_back(GUITexture::read(stream, header));
    }

    stream.seek_absolute(static_cast<s64>(header.fontFilterOffset));
    for (auto i = 0u; i < header.fontFilterNum; ++i) {
        m_font_filters.emplace_back(GUIFontFilter::read(stream, header));
    }

    stream.seek_absolute(static_cast<s64>(header.messageOffset));
    for (auto i = 0u; i < header.messageNum; ++i) {
        m_messages.emplace_back(GUIMessage::read(stream, header));
    }

    stream.seek_absolute(static_cast<s64>(header.guiResourceOffset));
    for (auto i = 0u; i < header.guiResourceNum; ++i) {
        m_resources.emplace_back(GUIResource::read(stream, header));
    }

    stream.seek_absolute(static_cast<s64>(header.generalResourceOffset));
    for (auto i = 0u; i < header.generalResourceNum; ++i) {
        m_general_resources.emplace_back(GUIGeneralResource::read(stream, header));
    }

    stream.seek_absolute(static_cast<s64>(header.keyOffset));
    for (auto i = 0u; i < header.keyNum; ++i) {
        m_keys.emplace_back(GUIKey::read(stream, header));
    }

    stream.seek_absolute(static_cast<s64>(header.vertexOffset));

    if (header.vertexBufferSize % GUIVertex::size != 0) {
        spdlog::error("Vertex buffer size is not a multiple of vertex size");
    }

    for (auto i = 0u; i < header.vertexBufferSize / GUIVertex::size; ++i) {
        m_vertices.emplace_back(GUIVertex::read(stream));
    }

    m_header = header;
}

void GUIFile::load_resources(const std::string& chunk_path, ID3D11Device* device, ID3D11DeviceContext* context) {
    for (auto& texture : m_textures) {
        try {
            BinaryReader stream(std::filesystem::path(chunk_path) / (texture.Path + ".tex"));
            texture.RenderTexture.load_from(stream, device, context);
        } catch (const std::runtime_error& e) { 
            spdlog::error(e.what());
        } catch (...) {
            continue;
        }
    }

    // TODO: load other kinds of resources
}

void GUIFile::run_data_usage_analysis(bool log_overlapping_offsets) const {
#ifdef GUI_FILE_ANALYSIS
    spdlog::info("Analyzing GUIFile...");

    std::unordered_map<KeyValueType, std::unordered_map<u32, u32>> p_offsets;
    std::unordered_map<KeyValueType, std::unordered_map<u32, u32>> ip_offsets;

    for (const auto& param : m_params) {
        if (param.ValueCount > 0) {
            p_offsets[param.ValueOffsetType][param.OrgValueOffset] += 1;
        }
    }

    std::unordered_map<u32, u32> ip_val_offsets;
    for (const auto& param : m_init_params) {
        ip_offsets[param.ValueOffsetType][param.OrgValueOffset] += 1;
    }

    for (auto i = 0u; i < 3; ++i) {
        const auto kvt = static_cast<KeyValueType>(i);

        const std::unordered_map<u32, u32>& param_data = p_offsets[kvt];
        const std::unordered_map<u32, u32>& iparam_data = p_offsets[kvt];

        const auto p_keys = param_data | std::views::keys;
        const auto p_values = param_data | std::views::values;
        const auto ip_keys = iparam_data | std::views::keys;
        const auto ip_values = iparam_data | std::views::values;

        const ptrdiff_t p_overlaps = std::ranges::count_if(p_values, [](u32 v) { return v > 0; });
        const ptrdiff_t ip_overlaps = std::ranges::count_if(ip_values, [](u32 v) {return v > 0; });
        const u32 p_max_overlap = *std::ranges::max_element(p_values);
        const u32 ip_max_overlap = *std::ranges::max_element(ip_values);

        std::vector<u32> overlapping_offsets{};
        std::ranges::set_intersection(p_keys, ip_keys, std::back_inserter(overlapping_offsets));

        spdlog::info("[Param] {} total overlapping {} Offsets. (Highest: {})", p_overlaps, enum_to_string(kvt), p_max_overlap);
        spdlog::info("[InitParam] {} total overlapping {} Offsets. (Highest: {})", ip_overlaps, enum_to_string(kvt), ip_max_overlap);
        spdlog::info("[Param & InitParam] share {} common {} offsets", overlapping_offsets.size(), enum_to_string(kvt));

        if (log_overlapping_offsets) {
            for (const u32 offset : overlapping_offsets) {
                spdlog::info("{:08X}", offset);
            }

            spdlog::info("[Param] The following {} offsets are used multiple times:", enum_to_string(kvt));
            for (const auto& [offset, count] : param_data) {
                spdlog::info("{:08X}: {} times", offset, count);
            }

            spdlog::info("[InitParam] The following {} offsets are used multiple times:", enum_to_string(kvt));
            for (const auto& [offset, count] : iparam_data) {
                spdlog::info("{:08X}: {} times", offset, count);
            }
        }
    }
#else
    spdlog::info("GUIFile Analysis disabled. Skipping analysis.");
#endif
}

void GUIFile::save_to(BinaryWriter& stream) {
    GUIHeader header = {
        .fileType = {'G', 'U', 'I', '\0'},
        .guiVersion = m_version,
        .fileSize = 0,
        .attr = m_attr,
        .revisionDate = std::time(nullptr),
        .instanceID = m_instance_id,
        .flowID = m_flow_id,
        .variableID = m_variable_id,
        .startInstanceIndex = m_start_instance_index,
        .animationNum = static_cast<u32>(m_animations.size()),
        .sequenceNum = static_cast<u32>(m_sequences.size()),
        .objectNum = static_cast<u32>(m_objects.size()),
        .objSequenceNum = static_cast<u32>(m_obj_sequences.size()),
        .initParamNum = static_cast<u32>(m_init_params.size()),
        .paramNum = static_cast<u32>(m_params.size()),
        .keyNum = static_cast<u32>(m_keys.size()),
        .instanceNum = static_cast<u32>(m_instances.size()),
        .flowNum = static_cast<u32>(m_flows.size()),
        .flowProcessNum = static_cast<u32>(m_flow_processes.size()),
        .flowInputNum = 0,
        .flowSwitchNum = 0,
        .flowFunctionNum = 0,
        .actionNum = 0,
        .inputConditionNum = 0,
        .switchConditionNum = 0,
        .switchOperatorNum = 0,
        .variableNum = 0,
        .textureNum = static_cast<u32>(m_textures.size()),
        .fontNum = 0,
        .fontFilterNum = static_cast<u32>(m_font_filters.size()),
        .messageNum = static_cast<u32>(m_messages.size()),
        .guiResourceNum = static_cast<u32>(m_resources.size()),
        .generalResourceNum = static_cast<u32>(m_general_resources.size()),
        .cameraSettingNum = 0,
        .instExeParamNum = 0,
        .vertexBufferSize = 0,
        .optionBitFlag = m_header.optionBitFlag,
        .viewSize_Width = m_header.viewSize_Width,
        .viewSize_Height = m_header.viewSize_Height,
        .startFlowIndex = m_header.startFlowIndex,
        .instanceParamEntryStartIndex = m_header.instanceParamEntryStartIndex
        // Rest of the fields will be filled later
    };

    stream.write(header);

    StringBuffer string_buffer;
    KeyValueBuffers kv_buffers;

    header.animationOffset = stream.tell();
    for (const auto& animation : m_animations) {
        animation.write(stream, string_buffer);
    }

    header.sequenceOffset = stream.tell();
    for (const auto& sequence : m_sequences) {
        sequence.write(stream, string_buffer);
    }

    header.objectOffset = stream.tell();
    for (const auto& object : m_objects) {
        object.write(stream, string_buffer, kv_buffers);
    }

    stream.write<u64>(0); // Padding for alignment

    header.objSequenceOffset = stream.tell();
    for (const auto& obj_sequence : m_obj_sequences) {
        obj_sequence.write(stream, string_buffer);
    }

    stream.write<u64>(0); // Padding for alignment

    header.initParamOffset = stream.tell();
    for (const auto& init_param : m_init_params) {
        init_param.write(stream, string_buffer, kv_buffers);
    }

    header.paramOffset = stream.tell();
    for (const auto& param : m_params) {
        param.write(stream, string_buffer, kv_buffers);
    }

    header.instanceOffset = stream.tell();
    for (const auto& instance : m_instances) {
        instance.write(stream, string_buffer, kv_buffers);
    }

    stream.write<u64>(0); // Padding for alignment

    header.flowOffset = stream.tell();
    for (const auto& flow : m_flows) {
        flow.write(stream, string_buffer);
    }

    header.flowProcessOffset = stream.tell();
    for (const auto& flow_process : m_flow_processes) {
        flow_process.write(stream, string_buffer);
    }

    header.textureOffset = stream.tell();
    for (const auto& texture : m_textures) {
        texture.write(stream, string_buffer);
    }

    header.fontFilterOffset = stream.tell();
    for (const auto& font_filter : m_font_filters) {
        font_filter->write(stream, string_buffer);
    }

    // Align to 16 bytes
    const auto n_align = (16 - stream.tell() % 16) / 4; // All font filters have a multiple of 4 size
    for (auto i = 0u; i < n_align; ++i) {
        stream.write<u32>(0);
    }

    header.messageOffset = stream.tell();
    for (const auto& message : m_messages) {
        message.write(stream, string_buffer);
    }

    header.guiResourceOffset = stream.tell();
    for (const auto& resource : m_resources) {
        resource.write(stream, string_buffer);
    }

    header.generalResourceOffset = stream.tell();
    for (const auto& general_resource : m_general_resources) {
        general_resource.write(stream, string_buffer);
    }

    header.keyOffset = stream.tell();
    for (const auto& key : m_keys) {
        key.write(stream, string_buffer, kv_buffers);
    }

    header.keyValue8Offset = stream.tell();
    stream.write(std::span<const u8>(kv_buffers.KeyValue8));

    header.keyValue32Offset = stream.tell();
    stream.write(std::span<const u32>(kv_buffers.KeyValue32));

    header.keyValue128Offset = stream.tell();
    stream.write(std::span<const vector4>(kv_buffers.KeyValue128));

    header.extendDataOffset = stream.tell();
    stream.write(std::span<const u8>(kv_buffers.ExtendData));
    
    header.stringOffset = stream.tell();
    stream.write(string_buffer.data(), string_buffer.size() + 1); // +1 for the null terminator

    header.vertexOffset = stream.tell();
    header.vertexBufferSize = static_cast<u32>(m_vertices.size() * GUIVertex::size);
    for (const auto& vertex : m_vertices) {
        vertex.write(stream, string_buffer);
    }

    header.fileSize = static_cast<u32>(stream.tell());

    stream.seek_absolute(0);
    stream.write(header);
}
