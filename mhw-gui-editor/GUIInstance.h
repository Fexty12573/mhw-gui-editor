#pragma once

#include "BinaryReader.h"
#include "GUITypes.h"
#include "dti_types.h"

#include <string>

struct GUIInstance {
	static constexpr size_t size = 56;
	static GUIInstance read(BinaryReader& reader, const GUIHeader& header);

	u32 ID;
	u32 Attr;
	s32 ParentPassIndex;
	s32 ChildIndex;
	u32 InitParamNum;
	std::string Name;
	ObjectType Type;
	u32 InitParamIndex;
	s64 ExtendDataOffset;
};