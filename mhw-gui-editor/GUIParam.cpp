#include "pch.h"
#include "GUIParam.h"

GUIParam GUIParam::read(BinaryReader& reader, const GUIHeader& header) {
	GUIParam result = {
		.Type = reader.read<u8>(),
		.ValueCount = reader.read_skip<u8>(14),
		.ParentID = reader.read_skip<u32>(4),
		.Name = reader.abs_offset_read_string(header.stringOffset + reader.read_skip<u32>(4)),
		.KeyIndex = reader.read_skip<u32>(4)
	};

	const auto offset = reader.read_skip<u32>(4);
	switch (result.Type) {
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
