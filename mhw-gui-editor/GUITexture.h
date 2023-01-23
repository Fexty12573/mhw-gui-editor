#pragma once

#include "BinaryReader.h"
#include "BinaryWriter.h"
#include "StringBuffer.h"
#include "GUITypes.h"
#include "dti_types.h"
#include "Texture.h"

#include <string>

struct GUITexture {
	static constexpr size_t size = 64;
	static GUITexture read(BinaryReader& reader, const GUIHeader& header);

	void write(BinaryWriter& writer, StringBuffer& buffer) const;

	u32 ID;
	union {
		struct {
			u32 RenderTargetType : 2;
			u32 Resize : 1;
			u32 : 29;
		};

		u32 Raw;
	} Meta;

	u16 Left;
	u16 Top;
	u16 Width;
	u16 Height;
	float4 Clamp;
	vector2 InvSize;
	std::string Path;
	std::string Name;

    Texture RenderTexture;
};

