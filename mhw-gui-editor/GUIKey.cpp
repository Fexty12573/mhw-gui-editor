#include "pch.h"
#include "GUIKey.h"

#include <format>

GUIKey GUIKey::read(BinaryReader& reader, const GUIHeader& header) {
	GUIKey key{};

    key.Data = { .Full = reader.read<u32>() };
    key.CurveOffset = reader.read_skip<u32>(8);

    if (key.CurveOffset != 0) {
        key.Curve = reader.abs_offset_read<hermitecurve>(header.extendDataOffset + key.CurveOffset);
    }

    return key;
}

void GUIKey::write(BinaryWriter& writer, StringBuffer& buffer, KeyValueBuffers& kv_buffers) const {
	writer.write(Data.Full);

    if (CurveOffset != 0) {
        writer.write(kv_buffers.ExtendData.size());
        kv_buffers.ExtendData.insert(kv_buffers.ExtendData.end(),
            reinterpret_cast<const u8*>(&Curve),
            reinterpret_cast<const u8*>(&Curve) + sizeof(hermitecurve)
        );
    } else {
        writer.write<u64>(0);
    }
    
	writer.write<u32>(0);
}

std::string GUIKey::get_preview(u32 index) const {
    const auto mode = static_cast<KeyMode>(Data.Bitfield.Mode_);
    if (index == -1) {
        return std::format("KeyFrame<C FFA3D7B8>{}</C>: <C FFB0C94E>{}</C>", Data.Bitfield.Frame, enum_to_string(mode));
    }

    return std::format("[<C FFA3D7B8>{}</C>] KeyFrame<C FFA3D7B8>{}</C>: <C FFB0C94E>{}</C>", index, Data.Bitfield.Frame, enum_to_string(mode));
}
