#pragma once

#include "BinaryReader.h"
#include "BinaryWriter.h"
#include "StringBuffer.h"
#include "GUITypes.h"
#include "dti_types.h"

#include <string>

struct GUIParam {
	static constexpr size_t size = 48;
	static GUIParam read(BinaryReader& reader, const GUIHeader& header);

    void write(BinaryWriter& writer, StringBuffer& buffer, KeyValueBuffers& kvbuffers) const;
	[[nodiscard]] std::string get_preview(u32 index = -1) const;

	ParamType Type;
	u8 ValueCount;
	u32 ParentID;
	std::string Name;
	u32 KeyIndex;
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
	u32 Index = 0;
};

