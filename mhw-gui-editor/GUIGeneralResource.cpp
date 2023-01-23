#include "pch.h"
#include "GUIGeneralResource.h"

GUIGeneralResource GUIGeneralResource::read(BinaryReader& reader, const GUIHeader& header) {
	return {
		.ID = reader.read<u32>(),
		.Type = reader.read_skip<ObjectType>(8),
		.Path = reader.abs_offset_read_string(header.stringOffset + reader.read_skip<u32>(4))
	};
}

void GUIGeneralResource::write(BinaryWriter& writer, StringBuffer& buffer) const {
	writer.write(ID);
	writer.write(Type);
	writer.write<u64>(0);
    writer.write(buffer.append_no_duplicate(Path));
}
