#pragma once

#include "BinaryReader.h"
#include "BinaryWriter.h"
#include "StringBuffer.h"
#include "GUITypes.h"
#include "dti_types.h"

#include <string>

struct GUIVertex {
	static constexpr size_t size = 32;
	static GUIVertex read(BinaryReader& reader);

	void write(BinaryWriter& writer, StringBuffer& buffer) const;

	vector4 A;
    vector4 B;
};

