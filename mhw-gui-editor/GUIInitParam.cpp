#include "pch.h"
#include "GUIInitParam.h"

#include <fmt/format.h>

GUIInitParam GUIInitParam::read(BinaryReader& reader, const GUIHeader& header) {
	GUIInitParam result = {
		.Type = reader.read<u8>(),
		.Use = reader.read_skip<bool>(14),
		.Name = reader.abs_offset_read_string(static_cast<s64>(header.stringOffset) + reader.read_skip<u32>(4))
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

std::string GUIInitParam::get_preview(u32 index) const {
	if (index == 0xFFFFFFFF) {
		return fmt::format("InitParam: <C FFB0C94E>{}</C> <C FFFEDC9C>{}</C>{}", Type, Name, !Use ? " <C FF3030EE>(Unused)</C>" : "");
	}

	return fmt::format("[<C FFA3D7B8>{}</C>] InitParam: <C FFB0C94E>{}</C> <C FFFEDC9C>{}</C>{}", Index, Type, Name, !Use ? " <C FF3030EE>(Unused)</C>" : "");
}
