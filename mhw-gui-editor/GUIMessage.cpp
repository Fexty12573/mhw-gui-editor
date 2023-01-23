#include "pch.h"
#include "GUIMessage.h"

GUIMessage GUIMessage::read(BinaryReader& reader, const GUIHeader& header) {
	return {
		.ID = reader.read_skip<u32>(12),
		.Path = reader.abs_offset_read_string(header.stringOffset + reader.read_skip<u32>(4))
	};
}

void GUIMessage::write(BinaryWriter& writer, StringBuffer& buffer) const {
	writer.write(ID);
	writer.write(buffer.append_no_duplicate(Path));
}
