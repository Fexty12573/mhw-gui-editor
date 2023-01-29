#include "pch.h"
#include "GUIInitParam.h"
#include "crc32.h"

#include <spdlog/spdlog.h>
#include <fmt/format.h>

GUIInitParam GUIInitParam::read(BinaryReader& reader, const GUIHeader& header) {
    GUIInitParam result = {
        .Type = reader.read<ParamType>(),
        .Use = reader.read_skip<bool>(14),
        .Name = reader.abs_offset_read_string(static_cast<s64>(header.stringOffset) + reader.read_skip<u32>(4)),
        .NameCRC = crc::crc32(result.Name.c_str(), result.Name.size()),
    };

    result.ValueVector = {};

    const auto offset = reader.read_skip<u32>(4);
    switch (static_cast<u8>(result.Type)) {
    case 3: [[fallthrough]];
    case 17:
    case 18:
        result.Value8 = reader.abs_offset_read<u8>(static_cast<s64>(header.keyValue8Offset) + offset);
        break;

    case 1: [[fallthrough]];
    case 7: [[fallthrough]];
    case 8: [[fallthrough]];
    case 11: [[fallthrough]];
    case 12: [[fallthrough]];
    case 13: [[fallthrough]];
    case 14: [[fallthrough]];
    case 16: [[fallthrough]];
    case 19: [[fallthrough]];
    case 20:
        result.Value32 = reader.abs_offset_read<u32>(static_cast<s64>(header.keyValue32Offset) + offset);
        break;

    case 2: [[fallthrough]];
    case 15:
        result.ValueFloat = reader.abs_offset_read<float>(static_cast<s64>(header.keyValue32Offset) + offset);
        break;

    case 4:
        result.ValueVector = reader.abs_offset_read<vector4>(static_cast<s64>(header.keyValue128Offset) + offset);
        break;

    case 6:
        result.ValueString = reader.abs_offset_read_string(static_cast<s64>(header.stringOffset) +
            reader.abs_offset_read<s64>(static_cast<s64>(header.keyValue32Offset) + offset));
        break;

    default:
        spdlog::warn("Unknown param type: {}\n", static_cast<u8>(result.Type));
        result.Value32 = reader.abs_offset_read<u32>(static_cast<s64>(header.keyValue32Offset) + offset);
        break;
    }

    if (result.Name.contains("Color") && result.Type == ParamType::VECTOR) {
        result.ValueVector /= 255.0f;
    }

    return result;
}

void GUIInitParam::write(BinaryWriter& writer, StringBuffer& buffer, KeyValueBuffers& kvbuffers) const {
    writer.write(Type);
    writer.write(Use);
    writer.write<u16>(0);
    writer.write<u32>(0);
    writer.write<u64>(0); // For alignment purposes
    writer.write(buffer.append_no_duplicate(Name));

    switch (Type) {
    case ParamType::BOOL: [[fallthrough]];
    case ParamType::INIT_BOOL: [[fallthrough]];
    case ParamType::INIT_INT:
        writer.write(kvbuffers.KeyValue8.size());
        kvbuffers.KeyValue8.push_back(Value8);
        break;
    case ParamType::INT: [[fallthrough]];
    case ParamType::FLOAT: [[fallthrough]];
    case ParamType::RESOURCE: [[fallthrough]];
    case ParamType::TEXTURE: [[fallthrough]];
    case ParamType::FONT: [[fallthrough]];
    case ParamType::MESSAGE: [[fallthrough]];
    case ParamType::VARIABLE: [[fallthrough]];
    case ParamType::ANIMATION: [[fallthrough]];
    case ParamType::EVENT: [[fallthrough]];
    case ParamType::GUIRESOURCE: [[fallthrough]];
    case ParamType::FONT_FILTER: [[fallthrough]];
    case ParamType::ANIMEVENT: [[fallthrough]];
    case ParamType::SEQUENCE: [[fallthrough]];
    case ParamType::GENERALRESOURCE:
        writer.write(kvbuffers.KeyValue32.size());
        kvbuffers.KeyValue32.push_back(Value32);
        break;
    case ParamType::STRING: {
        union {
            u64 v64{};
            u32 v32[2];
        } offset;

        writer.write(kvbuffers.KeyValue32.size());
        offset.v64 = buffer.append_no_duplicate(ValueString);
        kvbuffers.KeyValue32.push_back(offset.v32[0]);
        kvbuffers.KeyValue32.push_back(offset.v32[1]);
        break;
    }
    case ParamType::VECTOR:
        writer.write(kvbuffers.KeyValue128.size());
        kvbuffers.KeyValue128.push_back(ValueVector);
        break;
    default:
        writer.write(kvbuffers.KeyValue32.size());
        kvbuffers.KeyValue32.push_back(Value32);
        break;
    }
}

std::string GUIInitParam::get_preview(u32 index) const {
    if (index == 0xFFFFFFFF) {
        return fmt::format("InitParam: <C FFB0C94E>{}</C> <C FFFEDC9C>{}</C>{}", enum_to_string(Type), Name, !Use ? " <C FF3030EE>(Unused)</C>" : "");
    }

    return fmt::format("[<C FFA3D7B8>{}</C>] InitParam: <C FFB0C94E>{}</C> <C FFFEDC9C>{}</C>{}", Index, enum_to_string(Type), Name, !Use ? " <C FF3030EE>(Unused)</C>" : "");
}
