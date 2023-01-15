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
