#include "pch.h"
#include "GUIFlow.h"

GUIFlow GUIFlow::read(BinaryReader& reader, const GUIHeader& header) {
	return {
		.ID = reader.read<u32>(),
		.Type = reader.read<FlowType>(),
		.Attr = reader.read_skip<u32>(4),
		.Name = reader.abs_offset_read_string(header.stringOffset + reader.read_skip<u32>(4)),
		.FlowProcessIndex = reader.read_skip<u32>(4)
	};
}

GUIFlow GUIFlow::read_mhgu(BinaryReader& reader, const GUIHeaderMHGU& header) {
	return {
        .ID = reader.read<u32>(),
        .Type = reader.read<FlowType>(),
        .Attr = reader.read<u32>(),
        .Name = reader.abs_offset_read_string(header.stringOffset + reader.read<u32>()),
        .FlowProcessIndex = reader.read<u32>()
    };
}

void GUIFlow::write(BinaryWriter& writer, StringBuffer& buffer) const {
	writer.write(ID);
	writer.write(Type);
	writer.write<u64>(Attr);
    writer.write(buffer.append_no_duplicate(Name));
    writer.write<u64>(FlowProcessIndex);
}

void GUIFlow::write_mhgu(BinaryWriter& writer, StringBuffer& buffer) const {
    writer.write(ID);
    writer.write(Type);
    writer.write(Attr);
    writer.write((u32)buffer.append_no_duplicate(Name));
    writer.write(FlowProcessIndex);
}
