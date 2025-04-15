#include "pch.h"
#include "GUIVertex.h"

#define VEC4_TO_COLOR(vec) ((u32)((vec).x * 255) | ((u32)((vec).y * 255) << 8) | ((u32)((vec).z * 255) << 16) | ((u32)((vec).w * 255) << 24))

GUIVertex GUIVertex::read(BinaryReader& reader) {
    return {
        .V = reader.read<vector4>()
    };

    /*GUIVertex vertex{};

    
    vertex.Position = reader.read<vector3>();
    vertex.Color32 = reader.read<color>();
    vertex.UV = reader.read<vector2>();

    vertex.Color = {
        .x = (vertex.Color32 & 0xFF) / 255.0f,
        .y = ((vertex.Color32 >> 8) & 0xFF) / 255.0f,
        .z = ((vertex.Color32 >> 16) & 0xFF) / 255.0f,
        .w = ((vertex.Color32 >> 24) & 0xFF) / 255.0f
    };

    return vertex;*/
}

GUIVertex GUIVertex::read_mhgu(BinaryReader& reader) {
    return  {
        .V = reader.read<vector4>()
    };
}

void GUIVertex::write(BinaryWriter& writer, StringBuffer& buffer) const {
    writer.write(V);
}

void GUIVertex::write_mhgu(BinaryWriter& writer, StringBuffer& buffer) const {
    writer.write(V);
}
