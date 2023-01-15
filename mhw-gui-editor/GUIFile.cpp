#include "pch.h"
#include "GUIFile.h"

#include "GUITypes.h"


GUIFile::GUIFile() = default;

void GUIFile::load_from(BinaryReader& stream) {
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
        m_objects.emplace_back(GUIObject::read(stream, static_cast<s64>(header.stringOffset)));
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

    m_header = header;
}
