#pragma once

#include "BinaryReader.h"
#include "BinaryWriter.h"
#include "StringBuffer.h"
#include "GUITypes.h"
#include "dti_types.h"

#include <string>
#include <memory>

struct GUIFontFilter {
	static constexpr size_t size = 8;
	static std::shared_ptr<GUIFontFilter> read(BinaryReader& reader, const GUIHeader& header);

	virtual void write(BinaryWriter& writer, StringBuffer& buffer) const;

	ObjectType Type;
	u32 ID;
};

struct GUIFontFilterDistanceField : GUIFontFilter {
	static constexpr size_t size = 64;

    void write(BinaryWriter& writer, StringBuffer& buffer) const override;

	u32 ParamNum;
	struct {
		color Color;
		s32 Size;
		u32 Softness;
		u32 Range;
		u32 Strength;
	} Params[2];
	u32 Offset;
	u32 OffsetAngle;
	BlendState Blending;

	vector4 Colors[2];
};

struct GUIFontFilterGradationOverlay : GUIFontFilter {
	static constexpr size_t size = 16;

	void write(BinaryWriter& writer, StringBuffer& buffer) const override;

	u32 TextureIndex;
	std::string Name;
};

struct GUIFontFilterBorder : GUIFontFilter {
	static constexpr size_t size = 20;

	void write(BinaryWriter& writer, StringBuffer& buffer) const override;

	u32 BorderType;
	s32 Distance;
	color Color;
};

struct GUIFontFilterShadow : GUIFontFilter {
	static constexpr size_t size = 20;

	void write(BinaryWriter& writer, StringBuffer& buffer) const override;

	s32 Distance;
	s32 Rotation;
	color Color;
};
