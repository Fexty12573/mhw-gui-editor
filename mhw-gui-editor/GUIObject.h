#pragma once

#include "BinaryReader.h"
#include "GUITypes.h"
#include "dti_types.h"

#include <string>

struct GUIObject {
	static constexpr size_t size = 56;
	static GUIObject read(BinaryReader& reader, const GUIHeader& header);

	u32 ID;
	u8 InitParamNum;
	u8 AnimateParamNum;
	u16 Padding;
	s32 PassParentIndex;
	s32 ChildIndex;
	std::string Name;
    ObjectType Type;
    u32 InitParamIndex;
    u32 ObjectSequenceIndex;
	s64 ExtendDataOffset;
};

