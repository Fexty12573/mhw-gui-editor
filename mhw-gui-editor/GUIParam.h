#pragma once

#include "BinaryReader.h"
#include "BinaryWriter.h"
#include "StringBuffer.h"
#include "GUITypes.h"
#include "dti_types.h"

#include <string>
#include <variant>

struct GUIParam {
	static constexpr size_t size = 48;
	static GUIParam read(BinaryReader& reader, const GUIHeader& header);

    void write(BinaryWriter& writer, StringBuffer& buffer, KeyValueBuffers& kvbuffers) const;
	[[nodiscard]] std::string get_preview(u32 index = -1) const;

	ParamType Type;
	u8 ValueCount;
	u32 ParentID;
	std::string Name;
	u32 KeyIndex;

	std::variant<
		std::vector<bool>, 
		std::vector<u8>, 
		std::vector<u32>, 
		std::vector<f32>, 
		std::vector<vector4>, 
		std::vector<std::string>
	> Values;

	// Meta values
	u32 NameCRC;
	u32 Index = 0;

#ifdef GUI_FILE_ANALYSIS
	u32 OrgValueOffset;
	u32 OrgStringOffset;
	KeyValueType ValueOffsetType;
#endif
};

