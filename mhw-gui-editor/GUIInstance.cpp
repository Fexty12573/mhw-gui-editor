#include "pch.h"
#include "GUIInstance.h"

GUIInstance GUIInstance::read(BinaryReader& reader, const GUIHeader& header) {
	return {
		.ID = reader.read<u32>(),
		.Attr = reader.read<u32>(),
		.ParentPassIndex = reader.read<s32>(),
		.ChildIndex = reader.read<s32>(),
		.InitParamNum = reader.read_skip<u32>(4),
		.Name = reader.abs_offset_read_string(header.stringOffset + reader.read_skip<u32>(4)),
		.Type = reader.read_skip<ObjectType>(4),
		.InitParamIndex = reader.read_skip<u32>(4),
		.ExtendDataOffset = reader.read_skip<s32>(4),
	};
}
