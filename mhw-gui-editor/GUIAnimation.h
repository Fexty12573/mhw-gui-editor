#pragma once

#include <string>

#include "BinaryReader.h"
#include "dti_types.h"

struct GUIAnimation {
	static constexpr size_t size = 0x20;
	static GUIAnimation read(BinaryReader& stream, std::streamoff text_offset);

	[[nodiscard]] std::string get_preview(u32 index = 0xFFFFFFFF) const;

	u32 ID;
	u16 ObjectNum;
	u16 SequenceNum;
	u16 DrawableObjectNum;
	u16 AnimateParamNum;
	u32 RootObjectIndex;
	std::string Name;
	u32 SequenceIndex;
};

