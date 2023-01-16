#include "pch.h"
#include "GUIObject.h"

GUIObject GUIObject::read(BinaryReader& reader, std::streamoff text_offset) {
	return {
		.ID = reader.read<u32>(),
		.InitParamNum = reader.read<u8>(),
		.AnimateParamNum = reader.read_skip<u8>(2),
		.PassParentIndex = reader.read<s32>(),
		.ChildIndex = reader.read<s32>(),
		.Name = reader.abs_offset_read_string(text_offset + reader.read_skip<u32>(4)),
		.Type = reader.read_skip<ObjectType>(4),
		.InitParamIndex = reader.read_skip<u32>(4),
		.ObjectSequenceIndex = reader.read_skip<u32>(4),
		.ExtendDataOffset = reader.read<u64>()
	};
}
