#include "pch.h"
#include "GUISequence.h"

#include <format>

GUISequence GUISequence::read(BinaryReader& reader, std::streamoff text_offset) {
	return {
		.ID = reader.read<u32>(),
		.FrameCount = reader.read<u32>(),
		.Name = reader.abs_offset_read_string(text_offset + reader.read_skip<u32>(4))
	};
}

void GUISequence::write(BinaryWriter& writer, StringBuffer& buffer) const {
	writer.write(ID);
	writer.write(FrameCount);
    writer.write(buffer.append_no_duplicate(Name));
}

std::string GUISequence::get_preview(u32 index) const {
	if (index == 0xFFFFFFFF) {
		return std::format("Sequence<<C FFA3D7B8>{}</C>>: <C FFFEDC9C>{}</C> ({} Frames)", ID, Name, FrameCount);
	}

	return std::format("[<C FFA3D7B8>{}</C>] Sequence<<C FFA3D7B8>{}</C>>: <C FFFEDC9C>{}</C> ({} Frames)", index, ID, Name, FrameCount);
}
