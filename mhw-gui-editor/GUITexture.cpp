#include "pch.h"
#include "GUITexture.h"

GUITexture GUITexture::read(BinaryReader& reader, const GUIHeader& header) {
	return {
		.ID = reader.read<u32>(),
		.Meta = { .Raw = reader.read<u32>() },
		.Left = reader.read<u16>(),
		.Top = reader.read<u16>(),
		.Width = reader.read<u16>(),
		.Height = reader.read<u16>(),
		.Clamp = { reader.read<float>(), reader.read<float>(), reader.read<float>(), reader.read<float>() },
		.InvSize = reader.read_skip<vector2>(8),
		.Path = reader.abs_offset_read_string(header.stringOffset + reader.read_skip<u32>(4)),
		.Name = reader.abs_offset_read_string(header.stringOffset + reader.read_skip<u32>(4))
	};
}

void GUITexture::write(BinaryWriter& writer, StringBuffer& buffer) const {
	writer.write(ID);
    writer.write(Meta.Raw);
    writer.write(Left);
    writer.write(Top);
    writer.write(Width);
    writer.write(Height);
    writer.write(Clamp[0]);
    writer.write(Clamp[1]);
    writer.write(Clamp[2]);
    writer.write(Clamp[3]);
    writer.write(InvSize);
    writer.write<u64>(0);
	writer.write(buffer.append_no_duplicate(Path));
	writer.write(buffer.append_no_duplicate(Name));
}
