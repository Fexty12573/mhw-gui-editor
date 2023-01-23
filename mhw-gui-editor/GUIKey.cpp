#include "pch.h"
#include "GUIKey.h"

GUIKey GUIKey::read(BinaryReader& reader, const GUIHeader& header) {
	GUIKey key{};

    key.Data = { .Full = reader.read<u32>() };
    key.CurveOffset = reader.read_skip<u32>(8);
    key.Curve = reader.abs_offset_read<hermitecurve>(header.extendDataOffset + key.CurveOffset);

    return key;
}

void GUIKey::write(BinaryWriter& writer, StringBuffer& buffer, KeyValueBuffers& kv_buffers) const {
	writer.write(Data.Full);
    writer.write(kv_buffers.ExtendData.size());
    kv_buffers.ExtendData.insert(kv_buffers.ExtendData.end(), reinterpret_cast<const u8*>(&Curve), reinterpret_cast<const u8*>(&Curve) + sizeof(hermitecurve));
	writer.write<u32>(0);
}
