#pragma once

#include "BinaryReader.h"
#include "BinaryWriter.h"
#include "StringBuffer.h"
#include "GUITypes.h"
#include "dti_types.h"

#include <string>

struct GUIMessage {
	static constexpr size_t size = 24;
	static GUIMessage read(BinaryReader& reader, const GUIHeader& header);

	void write(BinaryWriter& writer, StringBuffer& buffer) const;

	u32 ID;
	std::string Path;
};

