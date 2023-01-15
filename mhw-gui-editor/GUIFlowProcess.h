#pragma once

#include "BinaryReader.h"
#include "GUITypes.h"
#include "dti_types.h"

#include <string>

struct GUIFlowProcess {
	static constexpr size_t size = 48;
	static GUIFlowProcess read(BinaryReader& reader, const GUIHeader& header);

	union {
		struct {
			u32 IsLoop : 8;
			u32 LoopStart : 24;
		};
		u32 Raw;
	} LoopData;

	u32 TotalFrameCount;
	u32 ParamNum;
	u32 ParamIndex;
	u32 ActionNum;
	EndConditionType EndCondition;
	u32 EndConditionParam;
	u32 NextIndex;
	u32 ActionIndex;
};

