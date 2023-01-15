#pragma once

#include "BinaryReader.h"
#include "GUITypes.h"
#include "dti_types.h"

#include <string>

struct GUIInitParam {
	static constexpr size_t size = 32;
	static GUIInitParam read(BinaryReader& reader, const GUIHeader& header);

	u8 Type;
	bool Use;
	std::string Name;
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
};

