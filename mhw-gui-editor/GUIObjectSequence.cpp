#include "pch.h"
#include "GUIObjectSequence.h"

GUIObjectSequence GUIObjectSequence::read(BinaryReader& reader) {
	return {
		.Attr = reader.read<u16>(),
		.InitParamNum = reader.read<u8>(),
		.ParamNum = reader.read<u8>(),
		.LoopStart = reader.read<s16>(),
		.FrameCount = reader.read<s16>(),
		.InitParamIndex = reader.read_skip<u32>(4),
		.ParamIndex = reader.read_skip<u32>(4)
	};
}
