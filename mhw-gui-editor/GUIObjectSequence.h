#pragma once

#include "BinaryReader.h"
#include "BinaryWriter.h"
#include "StringBuffer.h"
#include "dti_types.h"

#include <string>

struct GUIInitParam;
struct GUIParam;

struct GUIObjectSequence {
	static constexpr size_t size = 24;
	static GUIObjectSequence read(BinaryReader& reader);

    void write(BinaryWriter& writer, StringBuffer& buffer) const;
	[[nodiscard]] std::string get_preview(u32 index = 0xFFFFFFFF) const;

	u16 Attr;
	u8 InitParamNum;
	u8 ParamNum;
	s16 LoopStart;
	s16 FrameCount;
	u32 InitParamIndex;
	u32 ParamIndex;

	std::vector<GUIInitParam> InitParams;
	std::vector<GUIParam> Params;

	u32 Index;
};

