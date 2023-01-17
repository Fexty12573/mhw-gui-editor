#include "pch.h"
#include "GUIObject.h"

#include <format>

GUIObject GUIObject::read(BinaryReader& reader, const GUIHeader& header) {
	return {
		.ID = reader.read<u32>(),
		.InitParamNum = reader.read<u8>(),
		.AnimateParamNum = reader.read_skip<u8>(2),
		.NextIndex = reader.read<s32>(),
		.ChildIndex = reader.read<s32>(),
		.Name = reader.abs_offset_read_string(header.stringOffset + reader.read_skip<u32>(4)),
		.Type = reader.read_skip<ObjectType>(4),
		.InitParamIndex = reader.read_skip<u32>(4),
		.ObjectSequenceIndex = reader.read_skip<u32>(4),
		.ExtendDataOffset = reader.read_skip<s32>(4)
	};
}

std::string GUIObject::get_preview(u32 index) const {
	if (index == 0xFFFFFFFF) {
		return std::format("Object<<C FFA3D7B8>{}</C>>: <C FFB0C94E>{} </C><C FFFEDC9C>{}</C>", ID, enum_to_string(Type), Name);
	}

    return std::format("[<C FFA3D7B8>{}</C>] Object<<C FFA3D7B8>{}</C>>: <C FFB0C94E>{} </C><C FFFEDC9C>{}</C>", index, ID, enum_to_string(Type), Name);
}
