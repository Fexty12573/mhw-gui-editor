#include "pch.h"
#include "GUIResource.h"

GUIResource GUIResource::read(BinaryReader& reader, const GUIHeader& header) {
	return {
		.ID = reader.read_skip<u32>(12),
		.Path = reader.abs_offset_read_string(header.stringOffset + reader.read_skip<u32>(4)),
	};
}
