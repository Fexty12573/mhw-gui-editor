#include "pch.h"
#include "GUIGeneralResource.h"

GUIGeneralResource GUIGeneralResource::read(BinaryReader& reader, const GUIHeader& header) {
	GUIGeneralResource res{
		.ID = reader.read<u32>(),
		.Type = reader.read_skip<ObjectType>(8)
	};

	const u32 offset = reader.read_skip<u32>(4);
	if (offset == -1) {
        res.Path = "";
	} else {
		res.Path = reader.abs_offset_read_string(header.stringOffset + offset);
	}

	return res;
}

GUIGeneralResource GUIGeneralResource::read_mhgu(BinaryReader& reader, const GUIHeaderMHGU& header) {
	GUIGeneralResource res{
		.ID = reader.read<u32>(),
		.Type = reader.read_skip<ObjectType>(4)
	};

	const u32 offset = reader.read<u32>();
	if (offset == -1) {
		res.Path = "";
	} else {
		res.Path = reader.abs_offset_read_string(header.stringOffset + offset);
	}

	return res;
}

void GUIGeneralResource::write(BinaryWriter& writer, StringBuffer& buffer) const {
	writer.write(ID);
	writer.write(Type);
	writer.write<u64>(0);
    writer.write(buffer.append_no_duplicate(Path));
}

void GUIGeneralResource::write_mhgu(BinaryWriter& writer, StringBuffer& buffer) const {
    writer.write(ID);
    writer.write(Type);
    writer.write<u32>(0);
    writer.write<u32>((u32)buffer.append_no_duplicate(Path));
}
