#include "pch.h"
#include "GUIFlowProcess.h"

GUIFlowProcess GUIFlowProcess::read(BinaryReader& reader, const GUIHeader& header) {
	return {
		.LoopData = { .Raw = reader.read<u32>() },
		.TotalFrameCount = reader.read<u32>(),
		.ParamNum = reader.read<u32>(),
		.ParamIndex = reader.read<u32>(),
		.ActionNum = reader.read<u32>(),
		.EndCondition = reader.read<EndConditionType>(),
		.EndConditionParam = reader.read_skip<u32>(4),
		.FlowIndex = reader.read_skip<u32>(4),
		.ActionIndex = reader.read_skip<u32>(4)
	};
}

void GUIFlowProcess::write(BinaryWriter& writer, StringBuffer& buffer) const {
    writer.write(LoopData.Raw);
    writer.write(TotalFrameCount);
    writer.write(ParamNum);
    writer.write(ParamIndex);
    writer.write(ActionNum);
    writer.write(EndCondition);
    writer.write<u64>(EndConditionParam);
    writer.write<u64>(FlowIndex);
    writer.write<u64>(ActionIndex);
}
