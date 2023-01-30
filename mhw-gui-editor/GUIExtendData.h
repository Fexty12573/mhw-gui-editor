#pragma once

#include "GUITypes.h"
#include "BinaryReader.h"
#include "BinaryWriter.h"
#include "dti_types.h"

struct GUIExtendData {
    static GUIExtendData read(BinaryReader& reader, ObjectType type, s64 offset, const GUIHeader& header);

    void write(KeyValueBuffers& kv_buffers, ObjectType type) const;

    union {
        struct {
            s32 GUIResourceId;
            s32 AnimationId;
            u64 _pad;

            template<typename T> [[nodiscard]] T* begin() { return reinterpret_cast<T*>(&GUIResourceId); }
            template<typename T> [[nodiscard]] const T* begin() const { return reinterpret_cast<const T*>(&GUIResourceId); }
            template<typename T> [[nodiscard]] T* end() { return reinterpret_cast<T*>(&AnimationId) + 1; }
            template<typename T> [[nodiscard]] const T* end() const { return reinterpret_cast<const T*>(&AnimationId) + 1; }
        } ChildAnimationRoot;

        struct {
            u32 DataNum;
            u32 VertexIndex;
            float Data[2];
            u32 _pad;

            template<typename T> [[nodiscard]] T* begin() { return reinterpret_cast<T*>(DataNum); }
            template<typename T> [[nodiscard]] const T* begin() const { return reinterpret_cast<const T*>(&DataNum); }
            template<typename T> [[nodiscard]] T* end() { return reinterpret_cast<T*>(Data) + 2; }
            template<typename T> [[nodiscard]] const T* end() const { return reinterpret_cast<const T*>(&Data) + 2; }
        } TextureSet;

        u64 FullData[2];
    };

    std::vector<rectf> UVs;
};

