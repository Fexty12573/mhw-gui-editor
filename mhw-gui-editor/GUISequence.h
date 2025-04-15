#pragma once

#include "dti_types.h"
#include "BinaryReader.h"
#include "BinaryWriter.h"
#include "StringBuffer.h"

#include <string>


struct GUISequence {
	static constexpr size_t size = 0x10;
	static GUISequence read(BinaryReader& reader, std::streamoff text_offset);
	static GUISequence read_mhgu(BinaryReader& reader, std::streamoff text_offset);

    void write(BinaryWriter& writer, StringBuffer& buffer) const;
    void write_mhgu(BinaryWriter& writer, StringBuffer& buffer) const;
	[[nodiscard]] std::string get_preview(u32 index = 0xFFFFFFFF) const;

	u32 ID;
	u32 FrameCount;
	std::string Name;

	u32 Index = 0;
};

