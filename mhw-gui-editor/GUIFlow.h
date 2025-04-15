#pragma once

#include "BinaryReader.h"
#include "BinaryWriter.h"
#include "StringBuffer.h"
#include "GUITypes.h"
#include "dti_types.h"

#include <string>

struct GUIFlow {
	static constexpr size_t size = 32;
	static GUIFlow read(BinaryReader& reader, const GUIHeader& header);
	static GUIFlow read_mhgu(BinaryReader& reader, const GUIHeaderMHGU& header);

	void write(BinaryWriter& writer, StringBuffer& buffer) const;
    void write_mhgu(BinaryWriter& writer, StringBuffer& buffer) const;

	u32 ID;
	FlowType Type;
	u32 Attr;
	std::string Name;
	u32 FlowProcessIndex;
};

