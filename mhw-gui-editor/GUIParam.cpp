#include "pch.h"
#include "GUIParam.h"
#include "crc32.h"
#include "Console.h"

#include <format>

GUIParam GUIParam::read(BinaryReader& reader, const GUIHeader& header) {
	GUIParam result = {
		.Type = reader.read<ParamType>(),
		.ValueCount = reader.read_skip<u8>(14),
		.ParentID = reader.read_skip<u32>(4),
		.Name = reader.abs_offset_read_string(header.stringOffset + reader.read_skip<u32>(4)),
		.KeyIndex = reader.read_skip<u32>(4),
        .NameCRC = crc::crc32(result.Name.c_str(), result.Name.size())
	};

	const auto offset = reader.read_skip<u32>(4);
#ifdef GUI_FILE_ANALYSIS
	result.OrgValueOffset = offset;

	reader.seek_relative(-24); // Back to Name offset
	result.OrgStringOffset = reader.read_skip<u32>(20);

#define SET_KV_TYPE(VAR, TYPE) (VAR).ValueOffsetType = TYPE
#else
#define SET_KV_TYPE(VAR, TYPE) 
#endif

	switch (result.Type) {
	case ParamType::UNKNOWN:
		spdlog::warn("Unknown ParamType encountered: {}\n", result.Name);
		return result;
	case ParamType::FLOAT: [[fallthrough]];
	case ParamType::ANIMEVENT: 
		SET_KV_TYPE(result, KeyValueType::KV32);
		result.Values = std::vector<f32>{};
		break;
	case ParamType::VECTOR: 
		SET_KV_TYPE(result, KeyValueType::KV128);
		result.Values = std::vector<vector4>{};
		break;
	case ParamType::STRING: 
		SET_KV_TYPE(result, KeyValueType::KV32);
		result.Values = std::vector<std::string>{};
		break;
	case ParamType::INT: [[fallthrough]];
	case ParamType::RESOURCE: [[fallthrough]];
	case ParamType::TEXTURE: [[fallthrough]];
	case ParamType::FONT: [[fallthrough]];
	case ParamType::MESSAGE: [[fallthrough]];
	case ParamType::VARIABLE: [[fallthrough]];
	case ParamType::ANIMATION: [[fallthrough]];
	case ParamType::EVENT: [[fallthrough]];
	case ParamType::GUIRESOURCE: [[fallthrough]];
	case ParamType::FONT_FILTER: [[fallthrough]];
	case ParamType::SEQUENCE: [[fallthrough]];
	case ParamType::GENERALRESOURCE: [[fallthrough]];
	case ParamType::INIT_INT32: 
		SET_KV_TYPE(result, KeyValueType::KV32);
		result.Values = std::vector<u32>{};
		break;
	case ParamType::BOOL: [[fallthrough]];
	case ParamType::INIT_BOOL: [[fallthrough]];
	case ParamType::INIT_INT: 
		SET_KV_TYPE(result, KeyValueType::KV8);
		result.Values = std::vector<u8>{};
		break;
	default: 
		SET_KV_TYPE(result, KeyValueType::KV32);
		spdlog::warn("Unknown param type: {}\n", static_cast<u8>(result.Type));
		break;
	}

#define GET_VEC(TT, v) std::get<std::vector<TT>>(v)

	const auto kv8offset = static_cast<s64>(header.keyValue8Offset);
	const auto kv32offset = static_cast<s64>(header.keyValue32Offset);
	const auto kv128offset = static_cast<s64>(header.keyValue128Offset);

	for (auto i = 0u; i < result.ValueCount; ++i) {
		switch (static_cast<u8>(result.Type)) {
		case 3: [[fallthrough]];
		case 17: [[fallthrough]];
		case 18:
			GET_VEC(u8, result.Values).push_back(reader.abs_offset_read<u8>(kv8offset + offset + i * sizeof(u8)));
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
			GET_VEC(u32, result.Values).push_back(reader.abs_offset_read<u32>(kv32offset + offset + i * sizeof(u32)));
			break;

		case 2: [[fallthrough]];
		case 15:
			GET_VEC(float, result.Values).push_back(reader.abs_offset_read<float>(kv32offset + offset + i * sizeof(float)));
			break;

		case 4:
			GET_VEC(vector4, result.Values).push_back(reader.abs_offset_read<vector4>(kv128offset + offset + i * sizeof(vector4)));
			break;

		case 6:
			GET_VEC(std::string, result.Values).emplace_back(reader.abs_offset_read_string(static_cast<s64>(header.stringOffset) +
				reader.abs_offset_read<s64>(kv32offset + offset + i * sizeof(s64))));
			break;

		default:
			GET_VEC(u32, result.Values).push_back(reader.abs_offset_read<u32>(kv32offset + offset + i * sizeof(u32)));
			break;
		}
	}


    if (result.Name.contains("Color") && result.Name != "ColorScale" && result.Type == ParamType::VECTOR) {
        // Normalize color values
		auto& vec = GET_VEC(vector4, result.Values);
		for (auto& val : vec) {
			val /= 255.0f;
		}
    }

	return result;
}

void GUIParam::write(BinaryWriter& writer, StringBuffer& buffer, KeyValueBuffers& kvbuffers) const {
	writer.write(Type);
	writer.write(ValueCount);
	writer.write<u16>(0);
	writer.write<u32>(0);
	writer.write<u64>(0);
    writer.write<u64>(ParentID);
    writer.write(buffer.append_no_duplicate(Name));
    writer.write<u64>(KeyIndex);

	switch (Type) {
	case ParamType::BOOL: [[fallthrough]];
	case ParamType::INIT_BOOL: [[fallthrough]];
	case ParamType::INIT_INT:
		writer.write(kvbuffers.insert8<u8>(GET_VEC(u8, Values)));
		break;
	case ParamType::FLOAT: [[fallthrough]];
	case ParamType::ANIMEVENT: 
        writer.write(kvbuffers.insert32<float>(GET_VEC(float, Values)));
		break;
	case ParamType::INT: [[fallthrough]];
	case ParamType::RESOURCE: [[fallthrough]];
	case ParamType::TEXTURE: [[fallthrough]];
	case ParamType::FONT: [[fallthrough]];
	case ParamType::MESSAGE: [[fallthrough]];
	case ParamType::VARIABLE: [[fallthrough]];
	case ParamType::ANIMATION: [[fallthrough]];
	case ParamType::EVENT: [[fallthrough]];
	case ParamType::GUIRESOURCE: [[fallthrough]];
	case ParamType::FONT_FILTER: [[fallthrough]];
	case ParamType::SEQUENCE: [[fallthrough]];
	case ParamType::GENERALRESOURCE: [[fallthrough]];
	case ParamType::INIT_INT32:
        writer.write(kvbuffers.insert32<u32>(GET_VEC(u32, Values)));
		break;
	case ParamType::STRING: {
		std::vector<u64> offsets;
		for (const auto& s : GET_VEC(std::string, Values)) {
			offsets.push_back(buffer.append_no_duplicate(s));
		}
		writer.write(kvbuffers.insert64(offsets));
		break;
	}
	case ParamType::VECTOR:
		if (Name.contains("Color") && Name != "ColorScale") {
			std::vector<vector4> denormalized;
            std::ranges::transform(GET_VEC(vector4, Values), std::back_inserter(denormalized), [](const auto& v) { return v * 255.0f; });
			writer.write(kvbuffers.insert128(denormalized));
		} else {
			writer.write(kvbuffers.insert128(GET_VEC(vector4, Values)));
		}
		break;
	default:
		writer.write(kvbuffers.insert32<u32>(GET_VEC(u32, Values)));
		break;
	}
}

#undef GET_VEC

std::string GUIParam::get_preview(u32 index) const {
	const std::string fmt = "Param: <C FFB0C94E>{}</C> <C FFFEDC9C>{}</C>";

	if (index == -1) {
		if (ValueCount > 1) {
			return std::vformat(fmt + "[<C FFA3D7B8>{}</C>]", std::make_format_args(enum_to_string(Type), Name, ValueCount));
		}

		return std::vformat(fmt, std::make_format_args(enum_to_string(Type), Name));
	}

	if (ValueCount > 1) {
		return std::vformat("[<C FFA3D7B8>{}</C>] " + fmt + "[<C FFA3D7B8>{}</C>]", std::make_format_args(index, enum_to_string(Type), Name, ValueCount));
	}
	
	return std::vformat("[<C FFA3D7B8>{}</C>] " + fmt, std::make_format_args(index, enum_to_string(Type), Name));
}
