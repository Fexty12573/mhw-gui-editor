#include "pch.h"
#include "GUIObjectSequence.h"

#include <format>

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

GUIObjectSequence GUIObjectSequence::read_mhgu(BinaryReader& reader) {
	return {
		.Attr = reader.read<u16>(),
		.InitParamNum = reader.read<u8>(),
		.ParamNum = reader.read<u8>(),
		.LoopStart = reader.read<s16>(),
		.FrameCount = reader.read<s16>(),
		.InitParamIndex = reader.read<u32>(),
		.ParamIndex = reader.read<u32>()
	};
}

void GUIObjectSequence::write(BinaryWriter& writer, StringBuffer& buffer) const {
	writer.write(Attr);
    writer.write(InitParamNum);
    writer.write(ParamNum);
    writer.write(LoopStart);
    writer.write(FrameCount);
	writer.write<u64>(InitParamIndex);
    writer.write<u64>(ParamIndex);
}

void GUIObjectSequence::write_mhgu(BinaryWriter& writer, StringBuffer& buffer) const {
    writer.write(Attr);
    writer.write(InitParamNum);
    writer.write(ParamNum);
    writer.write(LoopStart);
    writer.write(FrameCount);
    writer.write(InitParamIndex);
    writer.write(ParamIndex);
}

std::string GUIObjectSequence::get_preview(u32 index, std::string_view name) const {
	if (index == 0xFFFFFFFF) {
		return std::format("ObjectSequence: I: <C FFA3D7B8>{}</C>, P: <C FFA3D7B8>{}</C>, LoopStart: <C FFA3D7B8>{}</C>, FrameCount: <C FFA3D7B8>{}</C>", InitParamNum, ParamNum, LoopStart, FrameCount);
	}

	if (name.empty()) {
		return std::format("[<C FFA3D7B8>{}</C>] ObjectSequence: I: <C FFA3D7B8>{}</C>, P: <C FFA3D7B8>{}</C>, LoopStart: <C FFA3D7B8>{}</C>, FrameCount: <C FFA3D7B8>{}</C>", index, InitParamNum, ParamNum, LoopStart, FrameCount);
	}

	return std::format("[<C FFA3D7B8>{}</C>] ObjectSequence: <C FFFEDC9C>{}</C>, I: <C FFA3D7B8>{}</C>, P: <C FFA3D7B8>{}</C>, LoopStart: <C FFA3D7B8>{}</C>, FrameCount: <C FFA3D7B8>{}</C>", index, name, InitParamNum, ParamNum, LoopStart, FrameCount);
}
