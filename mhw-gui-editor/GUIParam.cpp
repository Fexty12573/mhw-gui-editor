#include "pch.h"
#include "GUIParam.h"

#include <format>

GUIParam GUIParam::read(BinaryReader& reader, const GUIHeader& header) {
	GUIParam result = {
		.Type = reader.read<ParamType>(),
		.ValueCount = reader.read_skip<u8>(14),
		.ParentID = reader.read_skip<u32>(4),
		.Name = reader.abs_offset_read_string(header.stringOffset + reader.read_skip<u32>(4)),
		.KeyIndex = reader.read_skip<u32>(4)
	};

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
		result.ValueFloat4 = reader.abs_offset_read<vector4>(static_cast<s64>(header.keyValue32Offset) + offset);
		break;

	default:
		result.Value32 = reader.abs_offset_read<u32>(static_cast<s64>(header.keyValue32Offset) + offset);
		break;
	}

	return result;
}

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
