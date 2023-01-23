#pragma once

#include "BinaryReader.h"
#include "BinaryWriter.h"
#include "StringBuffer.h"
#include "GUITypes.h"
#include "dti_types.h"

#include <string>

struct GUIResource {
	static constexpr size_t size = 24;
	static GUIResource read(BinaryReader& reader, const GUIHeader& header);

	void write(BinaryWriter& writer, StringBuffer& buffer) const;

	u32 ID;
	std::string Path;
};

