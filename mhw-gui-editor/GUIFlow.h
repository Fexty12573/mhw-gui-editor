#pragma once

#include "BinaryReader.h"
#include "GUITypes.h"
#include "dti_types.h"

#include <string>

struct GUIFlow {
	static constexpr size_t size = 32;
	static GUIFlow read(BinaryReader& reader, const GUIHeader& header);

	u32 ID;
	FlowType Type;
	u32 Attr;
	std::string Name;
	u32 FlowProcessIndex;
};

