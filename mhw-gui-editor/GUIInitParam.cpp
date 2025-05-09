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

    const auto offset = reader.read_skip<u32>(4);
#ifdef GUI_FILE_ANALYSIS
    result.OrgValueOffset = offset;

    reader.seek_relative(-16); // Back to Name offset
    result.OrgStringOffset = reader.read_skip<u32>(12);

#define SET_KV_TYPE(VAR, TYPE) (VAR).ValueOffsetType = TYPE
#else
#define SET_KV_TYPE(VAR, TYPE) 
#endif

    result.ValueVector = {};

    switch (static_cast<u8>(result.Type)) {
    case 3:
        SET_KV_TYPE(result, KeyValueType::KV8);
        result.ValueBool = reader.abs_offset_read<bool>(static_cast<s64>(header.keyValue8Offset) + offset) != 0;
        break;
    case 17: [[fallthrough]];
    case 18:
        SET_KV_TYPE(result, KeyValueType::None);
        result.Value32 = offset;
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
        SET_KV_TYPE(result, KeyValueType::KV32);
        result.Value32 = reader.abs_offset_read<u32>(static_cast<s64>(header.keyValue32Offset) + offset);
        break;

    case 2: [[fallthrough]];
    case 15:
        SET_KV_TYPE(result, KeyValueType::KV32);
        result.ValueFloat = reader.abs_offset_read<float>(static_cast<s64>(header.keyValue32Offset) + offset);
        break;

    case 4:
        SET_KV_TYPE(result, KeyValueType::KV128);
        result.ValueVector = reader.abs_offset_read<vector4>(static_cast<s64>(header.keyValue128Offset) + offset);
        break;

    case 6:
        SET_KV_TYPE(result, KeyValueType::KV32);
        result.ValueString = reader.abs_offset_read_string(static_cast<s64>(header.stringOffset) +
            reader.abs_offset_read<s64>(static_cast<s64>(header.keyValue32Offset) + offset));
        break;

    default:
        SET_KV_TYPE(result, KeyValueType::KV32);
        spdlog::warn("Unknown param type: {}\n", static_cast<u8>(result.Type));
        result.Value32 = reader.abs_offset_read<u32>(static_cast<s64>(header.keyValue32Offset) + offset);
        break;
    }

    result.IsColorParam = result.Name.contains("Color") && result.Name != "ColorScale";
    result.IsColorParam |= result.Name == "RGB";
    result.IsColorParam &= result.Type == ParamType::VECTOR;

    if (result.IsColorParam) {
        result.ValueVector /= 255.0f;
    }

    return result;
}

GUIInitParam GUIInitParam::read_mhgu(BinaryReader& reader, const GUIHeaderMHGU& header) {
    reader.seek_relative(4); // First 4 bytes is for Func/Prop setter

    GUIInitParam result = {
        .Type = reader.read<ParamType>(),
        .Use = reader.read_skip<bool>(6),
        .Name = reader.abs_offset_read_string(static_cast<s64>(header.stringOffset) + reader.read<u32>()),
        .NameCRC = crc::crc32(result.Name.c_str(), result.Name.size()),
    };

    const auto offset = reader.read<u32>();
#ifdef GUI_FILE_ANALYSIS
    result.OrgValueOffset = offset;

    reader.seek_relative(-8); // Back to Name offset
    result.OrgStringOffset = reader.read_skip<u32>(4);

#define SET_KV_TYPE(VAR, TYPE) (VAR).ValueOffsetType = TYPE
#else
#define SET_KV_TYPE(VAR, TYPE) 
#endif

    result.ValueVector = {};

    switch (static_cast<u8>(result.Type)) {
    case 3:
        SET_KV_TYPE(result, KeyValueType::KV8);
        result.ValueBool = reader.abs_offset_read<bool>(static_cast<s64>(header.keyValue8Offset) + offset) != 0;
        break;
    case 17: [[fallthrough]];
    case 18:
        SET_KV_TYPE(result, KeyValueType::None);
        result.Value32 = offset;
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
        SET_KV_TYPE(result, KeyValueType::KV32);
        result.Value32 = reader.abs_offset_read<u32>(static_cast<s64>(header.keyValue32Offset) + offset);
        break;

    case 2: [[fallthrough]];
    case 15:
        SET_KV_TYPE(result, KeyValueType::KV32);
        result.ValueFloat = reader.abs_offset_read<float>(static_cast<s64>(header.keyValue32Offset) + offset);
        break;

    case 4:
        SET_KV_TYPE(result, KeyValueType::KV128);
        result.ValueVector = reader.abs_offset_read<vector4>(static_cast<s64>(header.keyValue128Offset) + offset);
        break;

    case 6:
        SET_KV_TYPE(result, KeyValueType::KV32);
        result.ValueString = reader.abs_offset_read_string(static_cast<s64>(header.stringOffset) +
            reader.abs_offset_read<s64>(static_cast<s64>(header.keyValue32Offset) + offset));
        break;

    default:
        SET_KV_TYPE(result, KeyValueType::KV32);
        spdlog::warn("Unknown param type: {}\n", static_cast<u8>(result.Type));
        result.Value32 = reader.abs_offset_read<u32>(static_cast<s64>(header.keyValue32Offset) + offset);
        break;
    }

    result.IsColorParam = result.Name.contains("Color") && result.Name != "ColorScale";
    result.IsColorParam |= result.Name == "RGB";
    result.IsColorParam &= result.Type == ParamType::VECTOR;

    if (result.IsColorParam) {
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
    case ParamType::BOOL:
        writer.write(kvbuffers.insert8(Value8));
        break;
    case ParamType::INIT_BOOL: [[fallthrough]];
    case ParamType::INIT_INT:
        writer.write<size_t>(Value32);
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
    case ParamType::GENERALRESOURCE: [[fallthrough]];
    case ParamType::INIT_INT32:
        writer.write(kvbuffers.insert32(Value32));
        break;
    case ParamType::STRING: {
        writer.write(kvbuffers.insert64(buffer.append_no_duplicate(ValueString)));
        break;
    }
    case ParamType::VECTOR:
        if (IsColorParam) {
            writer.write(kvbuffers.insert128(ValueVector * 255.0f));
        } else {
            writer.write(kvbuffers.insert128(ValueVector));
        }
        break;
    default:
        writer.write(kvbuffers.insert32(Value32));
        break;
    }
}

void GUIInitParam::write_mhgu(BinaryWriter& writer, StringBuffer& buffer, KeyValueBuffers& kvbuffers) const {
    writer.write<u32>(0); // For Func/Prop setter
    writer.write(Type);
    writer.write(Use);
    writer.write<u16>(0);
    writer.write<u32>(0);
    writer.write((u32)buffer.append_no_duplicate(Name));

    switch (Type) {
    case ParamType::BOOL:
        writer.write((u32)kvbuffers.insert8(Value8));
        break;
    case ParamType::INIT_BOOL: [[fallthrough]];
    case ParamType::INIT_INT:
        writer.write<u32>(Value32);
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
    case ParamType::GENERALRESOURCE: [[fallthrough]];
    case ParamType::INIT_INT32:
        writer.write((u32)kvbuffers.insert32(Value32));
        break;
    case ParamType::STRING:
        writer.write((u32)kvbuffers.insert32((u32)buffer.append_no_duplicate(ValueString)));
        break;
    case ParamType::VECTOR:
        if (IsColorParam) {
            writer.write((u32)kvbuffers.insert128(ValueVector * 255.0f));
        } else {
            writer.write((u32)kvbuffers.insert128(ValueVector));
        }
        break;
    default:
        writer.write((u32)kvbuffers.insert32(Value32));
        break;
    }
}

std::string GUIInitParam::get_preview(u32 index) const {
    if (index == 0xFFFFFFFF) {
        return fmt::format("InitParam: <C FFB0C94E>{}</C> <C FFFEDC9C>{}</C>{}", enum_to_string(Type), Name, !Use ? " <C FF3030EE>(Unused)</C>" : "");
    }

    return fmt::format("[<C FFA3D7B8>{}</C>] InitParam: <C FFB0C94E>{}</C> <C FFFEDC9C>{}</C>{}", Index, enum_to_string(Type), Name, !Use ? " <C FF3030EE>(Unused)</C>" : "");
}
