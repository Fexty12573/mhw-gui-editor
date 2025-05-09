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

    struct MiniHeader {
        u32 magic;
        Version version;
    };

    const auto mini_header = stream.read<MiniHeader>();
    if (mini_header.magic != 0x495547) {
        spdlog::error("Invalid GUI file magic: {:08X}", mini_header.magic);
        return;
    }

    stream.seek_absolute(0);

    if (mini_header.version == Version::MHW) { // ---------------------------- MHW
        m_header = stream.read<GUIHeader>();
        const auto& header = m_header;

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
    } else if (mini_header.version == Version::MHGU) { // ---------------------------- MHGU
        m_header_mhgu = stream.read<GUIHeaderMHGU>();
        const auto& header = m_header_mhgu;

        std::memcpy(m_magic.data(), header.fileType, m_magic.size());
        m_version = header.guiVersion;
        m_attr = header.attr;
        m_revision_time = header.revisionDate;
        m_instance_id = header.instanceID;
        m_flow_id = header.flowID;
        m_variable_id = header.variableID;
        m_start_instance_index = header.startInstanceIndex;

        stream.seek_absolute(header.animationOffset);
        for (auto i = 0u; i < header.animationNum; ++i) {
            m_animations.emplace_back(GUIAnimation::read_mhgu(stream, header.stringOffset));
        }

        stream.seek_absolute(header.sequenceOffset);
        for (auto i = 0u; i < header.sequenceNum; ++i) {
            m_sequences.emplace_back(GUISequence::read_mhgu(stream, header.stringOffset));
        }

        stream.seek_absolute(header.objectOffset);
        for (auto i = 0u; i < header.objectNum; ++i) {
            m_objects.emplace_back(GUIObject::read_mhgu(stream, header));
        }

        stream.seek_absolute(header.objSequenceOffset);
        for (auto i = 0u; i < header.objSequenceNum; ++i) {
            m_obj_sequences.emplace_back(GUIObjectSequence::read_mhgu(stream));
        }

        stream.seek_absolute(header.initParamOffset);
        for (auto i = 0u; i < header.initParamNum; ++i) {
            m_init_params.emplace_back(GUIInitParam::read_mhgu(stream, header));
        }

        stream.seek_absolute(header.paramOffset);
        for (auto i = 0u; i < header.paramNum; ++i) {
            m_params.emplace_back(GUIParam::read_mhgu(stream, header));
        }

        stream.seek_absolute(header.instanceOffset);
        for (auto i = 0u; i < header.instanceNum; ++i) {
            m_instances.emplace_back(GUIInstance::read_mhgu(stream, header));
        }

        stream.seek_absolute(header.flowOffset);
        for (auto i = 0u; i < header.flowNum; ++i) {
            m_flows.emplace_back(GUIFlow::read_mhgu(stream, header));
        }

        stream.seek_absolute(header.flowProcessOffset);
        for (auto i = 0u; i < header.flowProcessNum; ++i) {
            m_flow_processes.emplace_back(GUIFlowProcess::read_mhgu(stream, header));
        }

        stream.seek_absolute(header.textureOffset);
        for (auto i = 0u; i < header.textureNum; ++i) {
            m_textures.emplace_back(GUITexture::read_mhgu(stream, header));
        }

        stream.seek_absolute(header.fontFilterOffset);
        for (auto i = 0u; i < header.fontFilterNum; ++i) {
            m_font_filters.emplace_back(GUIFontFilter::read_mhgu(stream, header));
        }

        stream.seek_absolute(header.messageOffset);
        for (auto i = 0u; i < header.messageNum; ++i) {
            m_messages.emplace_back(GUIMessage::read_mhgu(stream, header));
        }

        stream.seek_absolute(header.guiResourceOffset);
        for (auto i = 0u; i < header.guiResourceNum; ++i) {
            m_resources.emplace_back(GUIResource::read_mhgu(stream, header));
        }

        stream.seek_absolute(header.generalResourceOffset);
        for (auto i = 0u; i < header.generalResourceNum; ++i) {
            m_general_resources.emplace_back(GUIGeneralResource::read_mhgu(stream, header));
        }

        stream.seek_absolute(header.keyOffset);
        for (auto i = 0u; i < header.keyNum; ++i) {
            m_keys.emplace_back(GUIKey::read_mhgu(stream, header));
        }

        stream.seek_absolute(header.vertexOffset);

        if (header.vertexBufferSize % GUIVertex::size != 0) {
            spdlog::error("Vertex buffer size is not a multiple of vertex size");
        }

        for (auto i = 0u; i < header.vertexBufferSize / GUIVertex::size; ++i) {
            m_vertices.emplace_back(GUIVertex::read(stream));
        }
    } else {
        spdlog::error("Invalid GUI file version: {:08X}", (u32)mini_header.version);
        return;
    }

    // Determine instance root(s)
    for (const auto [i, inst] : std::views::enumerate(m_instances)) {
        const bool is_child_instance = std::ranges::any_of(m_instances, [i, &inst](const GUIInstance& other) {
            if (other.ID == inst.ID) {
                return false;
            }

            return other.ChildIndex == i || other.NextIndex == i;
        });

        if (!is_child_instance) {
            m_root_instances.push_back(inst.Index);
        }
    }
}

void GUIFile::load_resources(const std::string& chunk_path, const std::string& native_path, const std::string& arcfs_path, ID3D11Device* device, ID3D11DeviceContext* context) {
    if (get_version() == Version::MHW) {
        for (auto& texture : m_textures) {
            const char* extension = texture.Meta.RenderTargetType == 1 ? ".rtex" : ".tex";

            const std::filesystem::path chunk = std::filesystem::path(chunk_path) / (texture.Path + extension);
            const std::filesystem::path native = std::filesystem::path(native_path) / (texture.Path + extension);
            std::filesystem::path path;

            if (exists(native)) {
                path = native;
            } else if (exists(chunk)) {
                path = chunk;
            } else {
                spdlog::error("Could not find texture: {}", texture.Path);
                continue;
            }

            try {
                BinaryReader stream(path);
                texture.RenderTexture.load_from(stream, false, device, context);
            }
            catch (...) {
                spdlog::error("Could not load texture: {}", texture.Path);
            }
        }
    }

    if (get_version() == Version::MHGU) {
        for (auto& texture : m_textures) {
            const char* extension = texture.Meta.RenderTargetType == 1 ? ".rtex" : ".tex";
            const std::filesystem::path path = std::filesystem::path(arcfs_path) / (texture.Path + extension);

            if (!exists(path)) {
                spdlog::error("Could not find texture: {}", texture.Path);
                continue;
            }

            try {
                BinaryReader stream(path);
                texture.RenderTexture.load_from(stream, true, device, context);
            }
            catch (...) {
                spdlog::error("Could not load texture: {}", texture.Path);
            }
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
        if (param.ValueCount > 0 && param.ValueOffsetType != KeyValueType::None) {
            p_offsets[param.ValueOffsetType][param.OrgValueOffset] += 1;
        }
    }

    std::unordered_map<u32, u32> ip_val_offsets;
    for (const auto& param : m_init_params) {
        if (param.ValueOffsetType != KeyValueType::None) {
            ip_offsets[param.ValueOffsetType][param.OrgValueOffset] += 1;
        }
    }

    for (auto i = std::to_underlying(KeyValueType::KV8); i < std::to_underlying(KeyValueType::String); ++i) {
        const auto kvt = static_cast<KeyValueType>(i);

        const std::unordered_map<u32, u32>& param_data = p_offsets[kvt];
        const std::unordered_map<u32, u32>& iparam_data = ip_offsets[kvt];

        const auto p_keys = param_data | std::views::keys;
        const auto p_values = param_data | std::views::values;
        const auto ip_keys = iparam_data | std::views::keys;
        const auto ip_values = iparam_data | std::views::values;

        const ptrdiff_t p_overlaps = std::ranges::count_if(p_values, [](u32 v) { return v > 0; });
        const ptrdiff_t ip_overlaps = std::ranges::count_if(ip_values, [](u32 v) {return v > 0; });
        const u32 p_max_overlap = p_values.size() > 0 ? *std::ranges::max_element(p_values) : 0;
        const u32 ip_max_overlap = ip_values.size() > 0 ? *std::ranges::max_element(ip_values) : 0;

        std::vector<u32> overlapping_offsets{};
        std::ranges::set_intersection(p_keys, ip_keys, std::back_inserter(overlapping_offsets));

        spdlog::info("[Param] {} total overlapping {} Offsets. (Highest: {})", p_overlaps, enum_to_string(kvt), p_max_overlap);
        spdlog::info("[InitParam] {} total overlapping {} Offsets. (Highest: {})", ip_overlaps, enum_to_string(kvt), ip_max_overlap);
        spdlog::info("[Param & InitParam] share {} common {} offsets", overlapping_offsets.size(), enum_to_string(kvt));

        if (log_overlapping_offsets) {
            for (const u32 offset : overlapping_offsets) {
                spdlog::info("{:08X}", offset);
            }

            const auto more_than_1 = [](const auto& p) { return std::get<1>(p) > 1; };

            spdlog::info("[Param] The following {} offsets are used multiple times:", enum_to_string(kvt));
            for (const auto& [offset, count] : param_data | std::views::filter(more_than_1)) {
                spdlog::info("{:08X}: {} times", offset, count);
            }

            spdlog::info("[InitParam] The following {} offsets are used multiple times:", enum_to_string(kvt));
            for (const auto& [offset, count] : iparam_data | std::views::filter(more_than_1)) {
                spdlog::info("{:08X}: {} times", offset, count);
            }
        }
    }


    for (auto i = 0u; i < m_init_params.size(); ++i) {
        u32 obj_use_count = 0;
        u32 inst_use_count = 0;
        u32 objseq_use_count = 0;

        for (const auto& obj : m_objects) {
            const u32 start = obj.InitParamIndex;
            const u32 end = start + obj.InitParamNum;

            if (i <= start && i < end) {
                ++obj_use_count;
            }
        }

        for (const auto& inst : m_instances) {
            const u32 start = inst.InitParamIndex;
            const u32 end = start + inst.InitParamNum;

            if (i <= start && i < end) {
                ++inst_use_count;
            }
        }

        for (const auto& objseq : m_obj_sequences) {
            const u32 start = objseq.InitParamIndex;
            const u32 end = start + objseq.InitParamNum;

            if (i <= start && i < end) {
                ++objseq_use_count;
            }
        }

        spdlog::info("InitParam[{}] used by {} Objects, {} Instances, and {} ObjSequences", i, obj_use_count, inst_use_count, objseq_use_count);
    }


#else
    spdlog::info("GUIFile Analysis disabled. Skipping analysis.");
#endif
}

void GUIFile::save_to(BinaryWriter& stream, const Settings& settings) const {
    StringBuffer string_buffer;
    KeyValueBuffers kv_buffers{
        .Config = {
            .MultipleKV8Refs = settings.AllowMultipleKV8References,
            .MultipleKV32Refs = settings.AllowMultipleKV32References,
            .MultipleKV128Refs = settings.AllowMultipleKV128References,
        }
    };

    if (get_version() == Version::MHW) {
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
            .instExeParamNum = static_cast<u32>(m_instances.size()),
            .vertexBufferSize = 0,
            .optionBitFlag = m_header.optionBitFlag,
            .viewSize_Width = m_header.viewSize_Width,
            .viewSize_Height = m_header.viewSize_Height,
            .startFlowIndex = m_header.startFlowIndex,
            .instanceParamEntryStartIndex = m_header.instanceParamEntryStartIndex
            // Rest of the fields will be filled later
        };

        stream.write(header);

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

        if (m_objects.size() % 2) {
            // On odd number of objects, there are 8 bytes of padding
            // Not technically necessary, just for alignment purposes
            stream.write<u64>(0);
        }

        header.objSequenceOffset = stream.tell();
        for (const auto& obj_sequence : m_obj_sequences) {
            obj_sequence.write(stream, string_buffer);
        }

        if (m_obj_sequences.size() % 2) {
            // Same thing here
            stream.write<u64>(0);
        }

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

        if (m_instances.size() % 2) {
            stream.write<u64>(0);
        }

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
        auto n_align = ((16 - stream.tell() % 16) / 4) % 4; // All font filters have a multiple of 4 size
        for (auto i = 0u; i < n_align; ++i) {
            stream.write<u32>(0);
        }

        header.messageOffset = stream.tell();
        for (const auto& message : m_messages) {
            message.write(stream, string_buffer);
        }

        if (m_messages.size() % 2) {
            stream.write<u64>(0);
        }

        header.guiResourceOffset = stream.tell();
        for (const auto& resource : m_resources) {
            resource.write(stream, string_buffer);
        }

        if (m_resources.size() % 2) {
            stream.write<u64>(0);
        }

        header.generalResourceOffset = stream.tell();
        for (const auto& general_resource : m_general_resources) {
            general_resource.write(stream, string_buffer);
        }

        if (m_general_resources.size() % 2) {
            stream.write<u64>(0);
        }

        header.keyOffset = stream.tell();
        for (const auto& key : m_keys) {
            key.write(stream, string_buffer, kv_buffers);
        }

        header.keyValue8Offset = stream.tell();
        stream.write(std::span<const u8>(kv_buffers.KeyValue8));

        n_align = 16 - stream.tell() % 16;
        for (auto i = 0u; i < n_align; ++i) {
            stream.write<u8>(0);
        }

        header.keyValue32Offset = stream.tell();
        stream.write(std::span<const u32>(kv_buffers.KeyValue32));

        n_align = ((16 - stream.tell() % 16) / 4) % 4;
        for (auto i = 0u; i < n_align; ++i) {
            stream.write<u32>(0);
        }

        header.keyValue128Offset = stream.tell();
        stream.write(std::span<const vector4>(kv_buffers.KeyValue128));

        header.extendDataOffset = stream.tell();
        stream.write(std::span<const u8>(kv_buffers.ExtendData));

        n_align = 16 - stream.tell() % 16;
        for (auto i = 0u; i < n_align; ++i) {
            stream.write<u8>(0);
        }

        header.stringOffset = stream.tell();
        stream.write(string_buffer.data(), string_buffer.size());

        n_align = 16 - stream.tell() % 16;
        for (auto i = 0u; i < n_align; ++i) {
            stream.write<u8>(0);
        }

        header.vertexOffset = stream.tell();
        header.vertexBufferSize = static_cast<u32>(m_vertices.size() * GUIVertex::size);
        for (const auto& vertex : m_vertices) {
            vertex.write(stream, string_buffer);
        }

        header.fileSize = static_cast<u32>(stream.tell());

        stream.seek_absolute(0);
        stream.write(header);
    } else if (get_version() == Version::MHGU) {
        GUIHeaderMHGU header = {
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
            .instExeParamNum = static_cast<u32>(m_instances.size()),
            .vertexBufferSize = 0,
            .optionBitFlag = m_header_mhgu.optionBitFlag,
            .viewSize_Width = m_header_mhgu.viewSize_Width,
            .viewSize_Height = m_header_mhgu.viewSize_Height,
            .startFlowIndex = m_header_mhgu.startFlowIndex,
            .instExeParamOffset = m_header_mhgu.instExeParamOffset
            // Rest of the fields will be filled later
        };

        stream.write(header);
        stream.pad_to(16);

        header.animationOffset = stream.tell32();
        for (const auto& animation : m_animations) {
            animation.write_mhgu(stream, string_buffer);
        }

        stream.pad_to(16);

        header.sequenceOffset = stream.tell32();
        for (const auto& sequence : m_sequences) {
            sequence.write_mhgu(stream, string_buffer);
        }

        stream.pad_to(16);

        header.objectOffset = stream.tell32();
        for (const auto& object : m_objects) {
            object.write_mhgu(stream, string_buffer, kv_buffers);
        }

        stream.pad_to(16);

        header.objSequenceOffset = stream.tell32();
        for (const auto& obj_sequence : m_obj_sequences) {
            obj_sequence.write_mhgu(stream, string_buffer);
        }

        stream.pad_to(16);

        header.initParamOffset = stream.tell32();
        for (const auto& init_param : m_init_params) {
            init_param.write_mhgu(stream, string_buffer, kv_buffers);
        }

        stream.pad_to(16);

        header.paramOffset = stream.tell32();
        for (const auto& param : m_params) {
            param.write_mhgu(stream, string_buffer, kv_buffers);
        }

        stream.pad_to(16);

        header.instanceOffset = stream.tell32();
        for (const auto& instance : m_instances) {
            instance.write_mhgu(stream, string_buffer, kv_buffers);
        }

        stream.pad_to(16);

        header.flowOffset = stream.tell32();
        for (const auto& flow : m_flows) {
            flow.write_mhgu(stream, string_buffer);
        }

        stream.pad_to(16);

        header.flowProcessOffset = stream.tell32();
        for (const auto& flow_process : m_flow_processes) {
            flow_process.write_mhgu(stream, string_buffer);
        }

        stream.pad_to(16);

        header.textureOffset = stream.tell32();
        for (const auto& texture : m_textures) {
            texture.write_mhgu(stream, string_buffer);
        }

        stream.pad_to(16);

        header.fontFilterOffset = stream.tell32();
        for (const auto& font_filter : m_font_filters) {
            font_filter->write_mhgu(stream, string_buffer);
        }

        stream.pad_to(16);

        header.messageOffset = stream.tell32();
        for (const auto& message : m_messages) {
            message.write_mhgu(stream, string_buffer);
        }

        stream.pad_to(16);

        header.guiResourceOffset = stream.tell32();
        for (const auto& resource : m_resources) {
            resource.write_mhgu(stream, string_buffer);
        }

        stream.pad_to(16);

        header.generalResourceOffset = stream.tell32();
        for (const auto& general_resource : m_general_resources) {
            general_resource.write_mhgu(stream, string_buffer);
        }

        stream.pad_to(16);

        header.keyOffset = stream.tell32();
        for (const auto& key : m_keys) {
            key.write_mhgu(stream, string_buffer, kv_buffers);
        }

        header.keyValue8Offset = stream.tell32();
        stream.write(std::span<const u8>(kv_buffers.KeyValue8));

        stream.pad_to(16);

        header.keyValue32Offset = stream.tell32();
        stream.write(std::span<const u32>(kv_buffers.KeyValue32));

        stream.pad_to(16);

        header.keyValue128Offset = stream.tell32();
        stream.write(std::span<const vector4>(kv_buffers.KeyValue128));

        header.extendDataOffset = stream.tell32();
        stream.write(std::span<const u8>(kv_buffers.ExtendData));

        stream.pad_to(16);

        header.stringOffset = stream.tell32();
        stream.write(string_buffer.data(), string_buffer.size());

        stream.pad_to(16);

        header.vertexOffset = stream.tell32();
        header.vertexBufferSize = static_cast<u32>(m_vertices.size() * GUIVertex::size);
        for (const auto& vertex : m_vertices) {
            vertex.write_mhgu(stream, string_buffer);
        }

        stream.pad_to(16);
        header.fileSize = static_cast<u32>(stream.tell());

        stream.seek_absolute(0);
        stream.write(header);
    }
}

void GUIFile::insert_animation(GUIAnimation anim, s32 index, bool) {
    if (index < 0) {
        m_animations.emplace_back(std::move(anim));
        return;
    }

    m_animations.emplace(m_animations.begin() + index, std::move(anim));
}

void GUIFile::insert_sequence(GUISequence seq, s32 index, bool update_indices) {
    if (index < 0) {
        m_sequences.emplace_back(std::move(seq));
        return;
    }

    m_sequences.emplace(m_sequences.begin() + index, std::move(seq));

    if (update_indices) {
        for (auto& anim : m_animations) {
            if (anim.SequenceIndex >= index) {
                ++anim.SequenceIndex;
            }
        }
    }
}

void GUIFile::insert_object(GUIObject obj, s32 index, bool update_indices) {
    if (index < 0) {
        m_objects.emplace_back(std::move(obj));
        return;
    }

    m_objects.emplace(m_objects.begin() + index, std::move(obj));

    if (update_indices) {
        for (auto& anim : m_animations) {
            if (anim.RootObjectIndex >= index) {
                ++anim.RootObjectIndex;
            }
        }
    }
}

void GUIFile::insert_object_sequence(GUIObjectSequence objseq, s32 index, bool update_indices) {
    if (index < 0) {
        m_obj_sequences.emplace_back(objseq);
        return;
    }

    m_obj_sequences.emplace(m_obj_sequences.begin() + index, objseq);

    if (update_indices) {
        for (auto& obj : m_objects) {
            if (obj.ObjectSequenceIndex >= index) {
                ++obj.ObjectSequenceIndex;
            }
        }
    }
}

void GUIFile::insert_init_param(GUIInitParam param, s32 index, bool update_indices) {
    if (index < 0) {
        m_init_params.emplace_back(std::move(param));
        return;
    }

    m_init_params.emplace(m_init_params.begin() + index, std::move(param));

    if (update_indices) {
        // Objects, Instances, ObjectSequences
        auto update_init_param_indices = [&](auto& container) {
            for (auto& obj : container) {
                if (obj.InitParamIndex >= index) {
                    ++obj.InitParamIndex;
                }
            }
        };

        update_init_param_indices(m_objects);
        update_init_param_indices(m_instances);
        update_init_param_indices(m_obj_sequences);
    }
}

void GUIFile::insert_param(GUIParam param, s32 index, bool update_indices) {
    if (index < 0) {
        m_params.emplace_back(std::move(param));
        return;
    }

    m_params.emplace(m_params.begin() + index, std::move(param));

    if (update_indices) {
        for (auto& objseq : m_obj_sequences) {
            if (objseq.ParamIndex >= index) {
                ++objseq.ParamIndex;
            }
        }
    }
}

void GUIFile::insert_key(GUIKey key, s32 index, bool update_indices) {
    if (index < 0) {
        m_keys.emplace_back(key);
        return;
    }

    m_keys.emplace(m_keys.begin() + index, key);

    if (update_indices) {
        for (auto& param : m_params) {
            if (param.KeyIndex >= index) {
                ++param.KeyIndex;
            }
        }
    }
}

void GUIFile::insert_instance(GUIInstance inst, s32 index, bool update_indices) {
    if (index < 0) {
        m_instances.emplace_back(std::move(inst));
        return;
    }

    m_instances.emplace(m_instances.begin() + index, std::move(inst));

    if (update_indices) {
        if (m_start_instance_index >= index) {
            ++m_start_instance_index;
        }
    }
}

void GUIFile::insert_flow(GUIFlow flow, s32 index, bool update_indices) {
    if (index < 0) {
        m_flows.emplace_back(std::move(flow));
        return;
    }

    m_flows.emplace(m_flows.begin() + index, std::move(flow));

    if (update_indices) {
        if (m_header.startFlowIndex >= index) {
            ++m_header.startFlowIndex;
        }

        for (auto& _flow : m_flows) {
            if (_flow.Type == FlowType::START && _flow.FlowProcessIndex >= index) {
                ++_flow.FlowProcessIndex;
            }
        }

        for (auto& fp : m_flow_processes) {
            if (fp.FlowIndex >= index) {
                ++fp.FlowIndex;
            }
        }
    }
}

void GUIFile::insert_flow_process(GUIFlowProcess flow, s32 index, bool update_indices) {
    if (index < 0) {
        m_flow_processes.emplace_back(std::move(flow));
        return;
    }

    m_flow_processes.emplace(m_flow_processes.begin() + index, std::move(flow));

    if (update_indices) {
        for (auto& _flow : m_flows) {
            if (_flow.Type == FlowType::PROCESS && _flow.FlowProcessIndex >= index) {
                ++_flow.FlowProcessIndex;
            }
        }
    }
}

void GUIFile::insert_texture(GUITexture tex, s32 index, bool) {
    if (index < 0) {
        m_textures.emplace_back(std::move(tex));
        return;
    }

    m_textures.emplace(m_textures.begin() + index, std::move(tex));
}

void GUIFile::insert_font_filter(std::shared_ptr<GUIFontFilter> filter, s32 index, bool) {
    if (index < 0) {
        m_font_filters.emplace_back(std::move(filter));
        return;
    }

    m_font_filters.emplace(m_font_filters.begin() + index, std::move(filter));
}

void GUIFile::erase_keys(u32 index, u32 count, bool update_indices) {
    if (index >= m_keys.size() || count == 0) {
        return;
    }

    m_keys.erase(m_keys.begin() + index, m_keys.begin() + index + count);

    if (!update_indices) {
        return;
    }

    for (auto& param : m_params) {
        if (param.KeyIndex >= index) {
            param.KeyIndex -= count;
        }
    }
}

u32 GUIFile::get_animation_object_count(u32 anim_index) const {
    return 0;
}
