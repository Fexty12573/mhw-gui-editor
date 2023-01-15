#pragma once

#include "BinaryReader.h"
#include "dti_types.h"

#include <string>

struct GUIObjectSequence {
	static constexpr size_t size = 24;
	static GUIObjectSequence read(BinaryReader& reader);

	u16 Attr;
	u8 InitParamNum;
	u8 ParamNum;
	s16 LoopStart;
	s16 FrameCount;
	u32 InitParamIndex;
	u32 ParamIndex;
};

