#pragma once

#include "dti_types.h"
#include "BinaryReader.h"

#include <string>

struct GUISequence {
	static constexpr size_t size = 0x10;
	static GUISequence read(BinaryReader& reader, std::streamoff text_offset);

	u32 ID;
	u32 FrameCount;
	std::string Name;
};

