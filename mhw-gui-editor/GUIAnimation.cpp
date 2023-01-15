#include "pch.h"
#include "GUIAnimation.h"

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
