#pragma once

#include "BinaryReader.h"
#include "GUITypes.h"
#include "dti_types.h"

#include <string>

struct GUIKey {
	static constexpr size_t size = 16;
	static GUIKey read(BinaryReader& reader);

	union {
		struct {
			u32 Frame : 24;
			u32 Mode_ : 8;
		} Bitfield;
		struct {
			u16 FrameLower;
			u8 FrameUpper;
			KeyMode Mode;
		} Named;

		u32 Full;
	} Data;
	u32 CurveOffset;
};

