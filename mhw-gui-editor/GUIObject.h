#pragma once

#include "BinaryReader.h"
#include "BinaryWriter.h"
#include "StringBuffer.h"
#include "GUITypes.h"
#include "GUIExtendData.h"
#include "dti_types.h"
#include "Sampler.h"

#include <string>


struct GUIObject {
	static constexpr size_t size = 56;
	static GUIObject read(BinaryReader& reader, const GUIHeader& header);
	static GUIObject read_mhgu(BinaryReader& reader, const GUIHeaderMHGU& header);

	void write(BinaryWriter& writer, StringBuffer& buffer, KeyValueBuffers& kv_buffers) const;
    void write_mhgu(BinaryWriter& writer, StringBuffer& buffer, KeyValueBuffers& kv_buffers) const;
	[[nodiscard]] std::string get_preview(u32 index = 0xFFFFFFFF) const;

	u32 ID;
	u8 InitParamNum;
	u8 AnimateParamNum;
	s32 NextIndex;
	s32 ChildIndex;
	std::string Name;
    ObjectType Type;
    u32 InitParamIndex;
    u32 ObjectSequenceIndex;
	s64 ExtendDataOffset;

    GUIExtendData ExtendData;

	u32 Index = 0;
};

