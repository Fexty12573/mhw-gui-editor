#include "pch.h"
#include "GUIAnimation.h"

#include <format>

GUIAnimation GUIAnimation::read(BinaryReader& stream, std::streamoff text_offset) {
	return {
		.ID = stream.read<u32>(),
		.ObjectNum = stream.read<u16>(),
		.SequenceNum = stream.read<u16>(),
		.DrawableObjectNum = stream.read<u16>(),
		.AnimateParamNum = stream.read<u16>(),
		.RootObjectIndex = stream.read<u32>(),
		.Name = stream.abs_offset_read_string(text_offset + stream.read_skip<u32>(4)),
		.SequenceIndex = stream.read_skip<u32>(4),
	};
}

std::string GUIAnimation::get_preview(u32 index) const {
	if (index == 0xFFFFFFFF) {
		return std::format("Animation<<C FFA3D7B8>{}</C>> <C FFFEDC9C>{}</C>", ID, Name);
	}
    
	return std::format("[<C FFA3D7B8>{}</C>] Animation<<C FFA3D7B8>{}</C>> <C FFFEDC9C>{}</C>", index, ID, Name);
}
