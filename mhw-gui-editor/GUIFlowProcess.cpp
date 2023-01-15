#include "pch.h"
#include "GUIFlowProcess.h"

GUIFlowProcess GUIFlowProcess::read(BinaryReader& reader, const GUIHeader& header) {
	return {
		.LoopData = { .Raw = reader.read<u32>() },
		.ParamNum = reader.read<u32>(),
		.ParamIndex = reader.read<u32>(),
		.ActionNum = reader.read<u32>(),
		.EndCondition = reader.read<EndConditionType>(),
		.EndConditionParam = reader.read_skip<u32>(4),
		.NextIndex = reader.read_skip<u32>(4),
		.ActionIndex = reader.read_skip<u32>(4)
	};
}
