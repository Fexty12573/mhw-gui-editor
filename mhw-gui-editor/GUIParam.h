#pragma once

#include "BinaryReader.h"
#include "BinaryWriter.h"
#include "StringBuffer.h"
#include "GUITypes.h"
#include "dti_types.h"

#include <functional>
#include <string>
#include <variant>

struct GUIParam {
	using ParamVariant = std::variant<
		std::vector<u8>,
		std::vector<u32>,
		std::vector<f32>,
		std::vector<vector4>,
		std::vector<std::string>
	>;
	template<typename T> using OpFunc = std::function<void(std::vector<T>*, GUIParam&)>;

	static constexpr size_t size = 48;
	static GUIParam read(BinaryReader& reader, const GUIHeader& header);
	static GUIParam read_mhgu(BinaryReader& reader, const GUIHeaderMHGU& header);

    void write(BinaryWriter& writer, StringBuffer& buffer, KeyValueBuffers& kvbuffers) const;
    void write_mhgu(BinaryWriter& writer, StringBuffer& buffer, KeyValueBuffers& kvbuffers) const;

	[[nodiscard]] std::string get_preview(u32 index = -1) const;
	void perform_value_operation(
		OpFunc<u8>&& u8op, 
		OpFunc<u32>&& u32op, 
		OpFunc<f32>&& f32op, 
		OpFunc<vector4>&& vec4op, 
		OpFunc<std::string>&& strop,
        bool pass_vector = true
	);

    template<typename T> T& get_value(u32 index) {
        return std::get<std::vector<T>>(Values)[index];
    }

	ParamType Type;
	u8 ValueCount;
	u32 ParentID;
	std::string Name;
	u32 KeyIndex;

	ParamVariant Values;

	// Meta values
	u32 NameCRC;
	u32 Index = 0;
	bool IsColorParam = false;

#ifdef GUI_FILE_ANALYSIS
	u32 OrgValueOffset;
	u32 OrgStringOffset;
	KeyValueType ValueOffsetType;
#endif
};

