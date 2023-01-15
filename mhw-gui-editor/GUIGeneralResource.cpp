#include "pch.h"
#include "GUIGeneralResource.h"

GUIGeneralResource GUIGeneralResource::read(BinaryReader& reader, const GUIHeader& header) {
	return {
		.ID = reader.read<u32>(),
		.Type = reader.read_skip<ObjectType>(8),
		.Path = reader.abs_offset_read_string(header.stringOffset + reader.read_skip<u32>(4))
	};
}
