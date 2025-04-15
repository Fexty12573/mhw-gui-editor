#pragma once

#include "BinaryReader.h"
#include "BinaryWriter.h"
#include "StringBuffer.h"
#include "GUITypes.h"
#include "dti_types.h"

#include <string>

struct GUIKey {
	static constexpr size_t size = 16;
    static GUIKey read(BinaryReader& reader, const GUIHeader& header);
    static GUIKey read_mhgu(BinaryReader& reader, const GUIHeaderMHGU& header);

	void write(BinaryWriter& writer, StringBuffer& buffer, KeyValueBuffers& kv_buffers) const;
    void write_mhgu(BinaryWriter& writer, StringBuffer& buffer, KeyValueBuffers& kv_buffers) const;
	[[nodiscard]] std::string get_preview(u32 index = -1) const;

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
    hermitecurve Curve;

	u32 Index;
};

