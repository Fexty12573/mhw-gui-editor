#pragma once

#include "BinaryReader.h"
#include "GUITypes.h"
#include "dti_types.h"

#include <string>

struct GUIParam {
	static constexpr size_t size = 48;
	static GUIParam read(BinaryReader& reader, const GUIHeader& header);

	[[nodiscard]] std::string get_preview(u32 index = -1) const;

	ParamType Type;
	u8 ValueCount;
	u32 ParentID;
	std::string Name;
	u32 KeyIndex;
	union {
		u8 Value8;
		u16 Value16;
		u32 Value32;
		u64 Value64;
		f32 ValueFloat;
		f64 ValueDouble;
		vector2 ValueFloat2;
		vector3 ValueFloat3;
		vector4 ValueFloat4;
	};

	u32 Index = 0;
};

