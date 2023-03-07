#pragma once

#include "BinaryReader.h"
#include "BinaryWriter.h"
#include "StringBuffer.h"
#include "GUITypes.h"
#include "GUIExtendData.h"
#include "dti_types.h"

#include <string>

struct GUIInitParam;
struct GUIObjectSequence;

struct GUIObject {
	static constexpr size_t size = 56;
	static GUIObject read(BinaryReader& reader, const GUIHeader& header);

	void write(BinaryWriter& writer, StringBuffer& buffer, KeyValueBuffers& kv_buffers) const;
	[[nodiscard]] std::string get_preview(u32 index = 0xFFFFFFFF) const;
	void resolve(
		const std::vector<GUIObject>& objects, 
		const std::vector<GUIInitParam>& init_params, 
		const std::vector<GUIObjectSequence>& sequences,
		u32 objseq_count
	);

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

	std::vector<GUIObject> Children;
	std::vector<GUIInitParam> InitParams;
	std::vector<GUIObjectSequence> ObjectSequences;

	u32 Index = 0;
};

