#include "pch.h"
#include "GUIExtendData.h"
#include "GUIVertex.h"


GUIExtendData GUIExtendData::read(BinaryReader& reader, ObjectType type, s64 offset, const GUIHeader& header) {
    if (offset == -1) {
        return {};
    }

    const auto old_pos = reader.tell();
    reader.seek_absolute(header.extendDataOffset + offset);

    GUIExtendData data{};

    switch (type) {
    case ObjectType::cGUIObjChildAnimationRoot:
        data.ChildAnimationRoot.GUIResourceId = reader.read<s32>();
        data.ChildAnimationRoot.AnimationId = reader.read<s32>();
        break;
    case ObjectType::cGUIObjTextureSet:
        data.TextureSet.DataNum = reader.read<u32>();
        data.TextureSet.VertexIndex = reader.read<u32>() / GUIVertex::size;
        data.TextureSet.Data[0] = reader.read<float>();
        data.TextureSet.Data[1] = reader.read<float>();

        data.UVs.resize(data.TextureSet.DataNum);
        for (auto& uv : data.UVs) {
            uv = reader.read<rectf>();
        }
        break;
    default:
        break;
    }

    reader.seek_absolute(old_pos);

    return data;
}

void GUIExtendData::write(KeyValueBuffers& kv_buffers, ObjectType type) const {
    switch (type) {
    case ObjectType::cGUIObjChildAnimationRoot:
        kv_buffers.ExtendData.insert(kv_buffers.ExtendData.end(),
            ChildAnimationRoot.begin<u8>(),
            ChildAnimationRoot.end<u8>()
        );
        break;
    case ObjectType::cGUIObjTextureSet:
        auto as_bytes = [](const auto& data) {
            return std::span(reinterpret_cast<const u8*>(&data), sizeof data);
        };

        kv_buffers.ExtendData.insert_range(kv_buffers.ExtendData.end(), as_bytes(TextureSet.DataNum));
        kv_buffers.ExtendData.insert_range(kv_buffers.ExtendData.end(), as_bytes(TextureSet.VertexIndex * GUIVertex::size));
        kv_buffers.ExtendData.insert_range(kv_buffers.ExtendData.end(), as_bytes(TextureSet.Data[0]));
        kv_buffers.ExtendData.insert_range(kv_buffers.ExtendData.end(), as_bytes(TextureSet.Data[1]));

        kv_buffers.ExtendData.insert(kv_buffers.ExtendData.end(),
            reinterpret_cast<const u8*>(UVs.data()),
            reinterpret_cast<const u8*>(UVs.data() + UVs.size())
        );
        break;
    default:
        break;
    }
}
