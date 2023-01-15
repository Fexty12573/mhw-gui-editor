#include "pch.h"
#include "GUIFontFilter.h"

std::shared_ptr<GUIFontFilter> GUIFontFilter::read(BinaryReader& reader, const GUIHeader& header) {
	const auto type = reader.read<ObjectType>();
	const auto id = reader.read<u32>();

	switch (type) {
	case ObjectType::cGUIFontFilterDistanceField: {
		const auto filter = std::make_shared<GUIFontFilterDistanceField>();
		filter->Type = type;
		filter->ID = id;
		filter->ParamNum = reader.read<u32>();

		for (u32 i = 0; i < 2; ++i) {
			filter->Params[i] = reader.read<std::remove_reference_t<decltype(filter->Params[i])>>();
			filter->Colors[i] = {
			(filter->Params[i].Color & 0xFF) / 255.0f,
			(filter->Params[i].Color >> 8 & 0xFF) / 255.0f,
			(filter->Params[i].Color >> 16 & 0xFF) / 255.0f,
			(filter->Params[i].Color >> 24 & 0xFF) / 255.0f,
			};
		}

		filter->Offset = reader.read<u32>();
		filter->OffsetAngle = reader.read<u32>();
		filter->Blending = reader.read_skip<BlendState>(3);

		return filter;
	}
	case ObjectType::cGUIFontFilterGradationOverlay: {
		const auto filter = std::make_shared<GUIFontFilterGradationOverlay>();
		filter->Type = type;
		filter->ID = id;
		filter->TextureIndex = reader.read<u32>();
		filter->Name = reader.abs_offset_read_string(header.stringOffset + reader.read<u32>());

		return filter;
	}
	case ObjectType::cGUIFontFilterBorder: {
		const auto filter = std::make_shared<GUIFontFilterBorder>();
		filter->Type = type;
		filter->ID = id;
		filter->BorderType = reader.read<u32>();
		filter->Distance = reader.read<s32>();
		filter->Color = reader.read<color>();

		return filter;
	}
	case ObjectType::cGUIFontFilterShadow:
	{
		const auto filter = std::make_shared<GUIFontFilterShadow>();
		filter->Type = type;
		filter->ID = id;
		filter->Distance = reader.read<s32>();
		filter->Rotation = reader.read<s32>();
		filter->Color = reader.read<color>();

		return filter;
	}
	default: break;
		
	}

	throw std::runtime_error("Unknown font filter type");
}
