#pragma once

#include "BinaryReader.h"
#include "BinaryWriter.h"
#include "StringBuffer.h"
#include "GUITypes.h"
#include "dti_types.h"

#include <string>

struct GUIInitParam {
	static constexpr size_t size = 32;
	static GUIInitParam read(BinaryReader& reader, const GUIHeader& header);

    void write(BinaryWriter& writer, StringBuffer& buffer, KeyValueBuffers& kvbuffers) const;
    [[nodiscard]] std::string get_preview(u32 index = 0xFFFFFFFF) const;

	ParamType Type;
	bool Use;
	std::string Name;
	union {
        bool ValueBool;
		u8 Value8;
		u32 Value32;
		u64 Value64;
		f32 ValueFloat;
		vector4 ValueVector;
	};
	// Should be part of the union but that deletes the implicit constructor and I don't wanna deal with that
    std::string ValueString;
	

	u32 NameCRC;
	u32 Index;
	bool IsColorParam = false;

#ifdef GUI_FILE_ANALYSIS
	u32 OrgValueOffset;
	u32 OrgStringOffset;
	KeyValueType ValueOffsetType;
#endif
};

