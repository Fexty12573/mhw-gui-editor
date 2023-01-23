#pragma once

#include "BinaryReader.h"
#include "BinaryWriter.h"
#include "StringBuffer.h"
#include "GUITypes.h"
#include "dti_types.h"

#include <string>

struct GUIGeneralResource {
	static constexpr size_t size = 24;
	static GUIGeneralResource read(BinaryReader& reader, const GUIHeader& header);

	void write(BinaryWriter& writer, StringBuffer& buffer) const;

	u32 ID;
	ObjectType Type;
	std::string Path;
};

