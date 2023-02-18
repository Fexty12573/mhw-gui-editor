#include "pch.h"
#include "GUIResource.h"

GUIResource GUIResource::read(BinaryReader& reader, const GUIHeader& header) {
	GUIResource res{
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

void GUIResource::write(BinaryWriter& writer, StringBuffer& buffer) const {
	writer.write(ID);
	writer.write<u32>(0);
	writer.write<u64>(0);
    writer.write(buffer.append_no_duplicate(Path));
}
