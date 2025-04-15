#pragma once

#include "BinaryReader.h"
#include "BinaryWriter.h"
#include "StringBuffer.h"
#include "GUITypes.h"
#include "dti_types.h"

#include <string>

struct GUIVertex {
	static constexpr size_t size = sizeof(vector4);
	static GUIVertex read(BinaryReader& reader);
	static GUIVertex read_mhgu(BinaryReader& reader);

	void write(BinaryWriter& writer, StringBuffer& buffer) const;
    void write_mhgu(BinaryWriter& writer, StringBuffer& buffer) const;

	vector4 V;
};

