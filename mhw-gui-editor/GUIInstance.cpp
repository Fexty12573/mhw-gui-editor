#include "pch.h"
#include "GUIInstance.h"

#include <format>

GUIInstance GUIInstance::read(BinaryReader& reader, const GUIHeader& header) {
	GUIInstance inst = {
		.ID = reader.read<u32>(),
		.Attr = reader.read<u32>(),
		.NextIndex = reader.read<s32>(),
		.ChildIndex = reader.read<s32>(),
		.InitParamNum = reader.read_skip<u32>(4),
		.Name = reader.abs_offset_read_string(header.stringOffset + reader.read_skip<u32>(4)),
		.Type = reader.read_skip<ObjectType>(4),
		.InitParamIndex = reader.read_skip<u32>(4),
		.ExtendDataOffset = reader.read_skip<s32>(4),
	};

    inst.ExtendData = GUIExtendData::read(reader, inst.Type, inst.ExtendDataOffset, header);

    return inst;
}

void GUIInstance::write(BinaryWriter& writer, StringBuffer& buffer, KeyValueBuffers& kv_buffers) const {
	writer.write(ID);
    writer.write(Attr);
    writer.write(NextIndex);
    writer.write(ChildIndex);
    writer.write(InitParamNum);
	writer.write<u32>(0);
    writer.write(buffer.append_no_duplicate(Name));
    writer.write(static_cast<u64>(Type));
    writer.write<u64>(InitParamIndex);

    if (ExtendDataOffset != -1) {
        writer.write(kv_buffers.ExtendData.size());
        ExtendData.write(kv_buffers, Type);
    } else {
        writer.write_tuple<s32, s32>({ -1, 0 });
    }

}

std::string GUIInstance::get_preview(u32 index) const {
    if (index == -1) {
        return std::format("Instance<<C FFA3D7B8>{}</C>>: <C FFB0C94E>{} </C><C FFFEDC9C>{}</C>", ID, enum_to_string(Type), Name);
    }

    return std::format("[<C FFA3D7B8>{}</C>] Instance<<C FFA3D7B8>{}</C>>: <C FFB0C94E>{} </C><C FFFEDC9C>{}</C>", index, ID, enum_to_string(Type), Name);
}
