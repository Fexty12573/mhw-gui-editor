#pragma once

#include "BinaryReader.h"
#include "BinaryWriter.h"
#include "StringBuffer.h"
#include "GUITypes.h"
#include "GUIExtendData.h"
#include "dti_types.h"

#include <string>

struct GUIInstance {
	static constexpr size_t size = 56;
	static GUIInstance read(BinaryReader& reader, const GUIHeader& header);

    void write(BinaryWriter& writer, StringBuffer& buffer, KeyValueBuffers& kv_buffers) const;
    [[nodiscard]] std::string get_preview(u32 index = 0xFFFFFFFF) const;

	u32 ID;
	u32 Attr;
	s32 NextIndex;
	s32 ChildIndex;
	u32 InitParamNum;
	std::string Name;
	ObjectType Type;
	u32 InitParamIndex;
	s64 ExtendDataOffset;

    GUIExtendData ExtendData;

	u32 Index;
};
