#pragma once

#include "dti_types.h"
#include "BinaryReader.h"

#include <string>

struct GUISequence {
	static constexpr size_t size = 0x10;
	static GUISequence read(BinaryReader& reader, std::streamoff text_offset);

	[[nodiscard]] std::string get_preview(u32 index = 0xFFFFFFFF) const;

	u32 ID;
	u32 FrameCount;
	std::string Name;

	u32 Index = 0;
};

