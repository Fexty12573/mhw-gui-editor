#pragma once

#include "BinaryReader.h"
#include "BinaryWriter.h"
#include "StringBuffer.h"
#include "GUITypes.h"
#include "dti_types.h"

#include <string>

struct GUIVertex {
	static constexpr size_t size = 24;
	static GUIVertex read(BinaryReader& reader);

	void write(BinaryWriter& writer, StringBuffer& buffer) const;

	vector3 Position;
	color Color32;
	vector2 UV;

	vector4 Color;
};

