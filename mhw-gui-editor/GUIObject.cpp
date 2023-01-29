#include "pch.h"
#include "GUIObject.h"
#include "GUIVertex.h"

#include <format>


GUIObject GUIObject::read(BinaryReader& reader, const GUIHeader& header) {
    GUIObject obj = {
        .ID = reader.read<u32>(),
        .InitParamNum = reader.read<u8>(),
        .AnimateParamNum = reader.read_skip<u8>(2),
        .NextIndex = reader.read<s32>(),
        .ChildIndex = reader.read<s32>(),
        .Name = reader.abs_offset_read_string(header.stringOffset + reader.read_skip<u32>(4)),
        .Type = reader.read_skip<ObjectType>(4),
        .InitParamIndex = reader.read_skip<u32>(4),
        .ObjectSequenceIndex = reader.read_skip<u32>(4),
        .ExtendDataOffset = reader.read_skip<s32>(4),
    };

    obj.ExtendData = GUIExtendData::read(reader, obj.Type, obj.ExtendDataOffset, header);

    return obj;
}

void GUIObject::write(BinaryWriter& writer, StringBuffer& buffer, KeyValueBuffers& kv_buffers) const {
	writer.write(ID);
    writer.write(InitParamNum);
    writer.write(AnimateParamNum);
	writer.write<u16>(0);
    writer.write(NextIndex);
    writer.write(ChildIndex);
	writer.write(buffer.append_no_duplicate(Name));
	writer.write(static_cast<u64>(Type));
	writer.write<u64>(InitParamIndex);
	writer.write<u64>(ObjectSequenceIndex);

    if (ExtendDataOffset != -1) {
        writer.write<s64>(kv_buffers.ExtendData.size());
        ExtendData.write(kv_buffers, Type);
    } else {
        writer.write<s64>(-1);
    }
}

std::string GUIObject::get_preview(u32 index) const {
	if (index == 0xFFFFFFFF) {
		return std::format("Object<<C FFA3D7B8>{}</C>>: <C FFB0C94E>{} </C><C FFFEDC9C>{}</C>", ID, enum_to_string(Type), Name);
	}

    return std::format("[<C FFA3D7B8>{}</C>] Object<<C FFA3D7B8>{}</C>>: <C FFB0C94E>{} </C><C FFFEDC9C>{}</C>", index, ID, enum_to_string(Type), Name);
}
