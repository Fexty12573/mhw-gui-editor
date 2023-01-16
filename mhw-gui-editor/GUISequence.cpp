#include "pch.h"
#include "GUISequence.h"

GUISequence GUISequence::read(BinaryReader& reader, std::streamoff text_offset) {
	return {
		.ID = reader.read<u32>(),
		.FrameCount = reader.read<u32>(),
		.Name = reader.abs_offset_read_string(text_offset + reader.read_skip<u32>(4))
	};
}
