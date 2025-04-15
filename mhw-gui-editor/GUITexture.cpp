#include "pch.h"
#include "GUITexture.h"

GUITexture GUITexture::read(BinaryReader& reader, const GUIHeader& header) {
	GUITexture tex{
		.ID = reader.read<u32>(),
		.Meta = { .Raw = reader.read<u32>() },
		.Left = reader.read<u16>(),
		.Top = reader.read<u16>(),
		.Width = reader.read<u16>(),
		.Height = reader.read<u16>(),
		.Clamp = { reader.read<float>(), reader.read<float>(), reader.read<float>(), reader.read<float>() },
		.InvSize = reader.read_skip<vector2>(8)
	};

	const u32 path_offset = reader.read_skip<u32>(4);
    const u32 name_offset = reader.read_skip<u32>(4);

	if (path_offset != -1) {
        tex.Path = reader.abs_offset_read_string(header.stringOffset + path_offset);
    } else {
        tex.Path = "";
    }

    if (name_offset != -1) {
        tex.Name = reader.abs_offset_read_string(header.stringOffset + name_offset);
    } else {
        tex.Name = "";
    }

    return tex;
}

GUITexture GUITexture::read_mhgu(BinaryReader& reader, const GUIHeaderMHGU& header) {
    GUITexture tex{
        .ID = reader.read<u32>(),
        .Meta = { .Raw = reader.read<u32>() },
        .Left = reader.read<u16>(),
        .Top = reader.read<u16>(),
        .Width = reader.read<u16>(),
        .Height = reader.read<u16>(),
        .Clamp = { reader.read<float>(), reader.read<float>(), reader.read<float>(), reader.read<float>() },
        .InvSize = reader.read_skip<vector2>(4)
    };

    const u32 path_offset = reader.read<u32>();
    const u32 name_offset = reader.read<u32>();

    if (path_offset != -1) {
        tex.Path = reader.abs_offset_read_string(header.stringOffset + path_offset);
    } else {
        tex.Path = "";
    }

    if (name_offset != -1) {
        tex.Name = reader.abs_offset_read_string(header.stringOffset + name_offset);
    } else {
        tex.Name = "";
    }

    reader.seek_relative(12); // Skip 12 bytes (padding)

    return tex;
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

void GUITexture::write_mhgu(BinaryWriter& writer, StringBuffer& buffer) const {
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
    writer.write<u32>(0);
    writer.write((u32)buffer.append_no_duplicate(Path));
    writer.write((u32)buffer.append_no_duplicate(Name));
}
