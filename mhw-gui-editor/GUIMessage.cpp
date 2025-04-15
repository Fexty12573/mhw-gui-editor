#include "pch.h"
#include "GUIMessage.h"

GUIMessage GUIMessage::read(BinaryReader& reader, const GUIHeader& header) {
	GUIMessage res{
		.ID = reader.read_skip<u32>(12)
	};

	const u32 offset = reader.read_skip<u32>(4);
	if (offset == -1) {
		res.Path = "";
	} else {
		res.Path = reader.abs_offset_read_string(header.stringOffset + offset);
	}

	return res;
}

GUIMessage GUIMessage::read_mhgu(BinaryReader& reader, const GUIHeaderMHGU& header) {
	reader.seek_relative(4); // Skip resource pointer (in-memory only)

	GUIMessage res{
		.ID = reader.read<u32>()
	};

	const u32 offset = reader.read<u32>();
	if (offset == -1) {
		res.Path = "";
	} else {
		res.Path = reader.abs_offset_read_string(header.stringOffset + offset);
	}

	return res;
}

void GUIMessage::write(BinaryWriter& writer, StringBuffer& buffer) const {
	writer.write(ID);
	writer.write(buffer.append_no_duplicate(Path));
}

void GUIMessage::write_mhgu(BinaryWriter& writer, StringBuffer& buffer) const {
    writer.write<u32>(0); // Resource pointer (in-memory only)
    writer.write(ID);
    writer.write((u32)buffer.append_no_duplicate(Path));
}
