#include "pch.h"
#include "GUIFlow.h"

GUIFlow GUIFlow::read(BinaryReader& reader, const GUIHeader& header) {
	return {
		.ID = reader.read<u32>(),
		.Type = reader.read<FlowType>(),
		.Attr = reader.read_skip<u32>(4),
		.Name = reader.abs_offset_read_string(header.stringOffset + reader.read_skip<u32>(4)),
		.FlowProcessIndex = reader.read_skip<u32>(4)
	};
}
