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
        data.ChildAnimationRoot._pad = reader.read<u64>();
        break;
    case ObjectType::cGUIObjTextureSet:
        data.TextureSet.DataNum = reader.read<u32>();
        data.TextureSet.VertexIndex = reader.read<u32>();
        data.TextureSet.Data[0] = reader.read<float>();
        data.TextureSet.Data[1] = reader.read<float>();

        data.UVs.resize(data.TextureSet.DataNum);
        for (auto& uv : data.UVs) {
            uv = reader.read<rectf>();
        }
        break;
    default:
        data.FullData[0] = reader.read<u64>();
        data.FullData[1] = reader.read<u64>();
        break;
    }

    reader.seek_absolute(old_pos);

    return data;
}

GUIExtendData GUIExtendData::read_mhgu(BinaryReader& reader, ObjectType type, s64 offset, const GUIHeaderMHGU& header) {
    if (offset == -1) {
        return {};
    }

    const auto old_pos = reader.tell();
    reader.seek_absolute(header.extendDataOffset + offset);

    GUIExtendData data{};

    switch (type) {
    case ObjectType::cGUIObjChildAnimationRoot:
        data.ChildAnimationRoot.AnimationId = reader.read<s32>();
        break;
    case ObjectType::cGUIObjTextureSet:
        data.TextureSet.DataNum = reader.read<u32>();
        data.TextureSet.VertexIndex = reader.read<u32>();

        data.UVs.resize(data.TextureSet.DataNum);
        for (auto& uv : data.UVs) {
            uv = reader.read<rectf>();
        }
        break;
    default:
        data.FullData[0] = reader.read<u64>();
        data.FullData[1] = reader.read<u64>();
        break;
    }

    reader.seek_absolute(old_pos);

    return data;
}

void GUIExtendData::write(KeyValueBuffers& kv_buffers, ObjectType type) const {
    constexpr auto as_bytes = [](const auto& data) {
        return std::span(reinterpret_cast<const u8*>(&data), sizeof data);
    };
    
    switch (type) {
    case ObjectType::cGUIObjChildAnimationRoot:
        kv_buffers.ExtendData.insert_range(kv_buffers.ExtendData.end(), as_bytes(ChildAnimationRoot.GUIResourceId));
        kv_buffers.ExtendData.insert_range(kv_buffers.ExtendData.end(), as_bytes(ChildAnimationRoot.AnimationId));
        kv_buffers.ExtendData.insert_range(kv_buffers.ExtendData.end(), as_bytes(ChildAnimationRoot._pad));
        break;
    case ObjectType::cGUIObjTextureSet:
        kv_buffers.ExtendData.insert_range(kv_buffers.ExtendData.end(), as_bytes(TextureSet.DataNum));
        kv_buffers.ExtendData.insert_range(kv_buffers.ExtendData.end(), as_bytes(TextureSet.VertexIndex));
        kv_buffers.ExtendData.insert_range(kv_buffers.ExtendData.end(), as_bytes(TextureSet.Data[0]));
        kv_buffers.ExtendData.insert_range(kv_buffers.ExtendData.end(), as_bytes(TextureSet.Data[1]));

        for (const auto& uv : UVs) {
            kv_buffers.ExtendData.insert_range(kv_buffers.ExtendData.end(), as_bytes(uv));
        }
        break;
    default:
        break;
    }
}

void GUIExtendData::write_mhgu(KeyValueBuffers& kv_buffers, ObjectType type) const {
    constexpr auto as_bytes = [](const auto& data) {
        return std::span(reinterpret_cast<const u8*>(&data), sizeof data);
    };

    switch (type)
    {
    case ObjectType::cGUIObjChildAnimationRoot:
        kv_buffers.ExtendData.insert_range(kv_buffers.ExtendData.end(), as_bytes(ChildAnimationRoot.AnimationId));
        break;
    case ObjectType::cGUIObjTextureSet:
        kv_buffers.ExtendData.insert_range(kv_buffers.ExtendData.end(), as_bytes(TextureSet.DataNum));
        kv_buffers.ExtendData.insert_range(kv_buffers.ExtendData.end(), as_bytes(TextureSet.VertexIndex));
        for (const auto& uv : UVs) {
            kv_buffers.ExtendData.insert_range(kv_buffers.ExtendData.end(), as_bytes(uv));
        }
        break;
    }
}
