#include "pch.h"
#include "GUIFontFilter.h"
#include "Console.h"

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
	case ObjectType::None: {
		return nullptr;
	}
	default: {
        spdlog::warn("Unknown GUIFontFilter type: {}", std::to_underlying(type));
		const auto filter = std::make_shared<GUIFontFilter>();
		filter->ID = id;
        filter->Type = type;

		return filter;
	}
		
	}
}

std::shared_ptr<GUIFontFilter> GUIFontFilter::read_mhgu(BinaryReader& reader, const GUIHeaderMHGU& header) {
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
	case ObjectType::None: {
		return nullptr;
	}
	default: {
		spdlog::warn("Unknown GUIFontFilter type: {}", std::to_underlying(type));
		const auto filter = std::make_shared<GUIFontFilter>();
		filter->ID = id;
		filter->Type = type;

		return filter;
	}

	}
}

void GUIFontFilter::write(BinaryWriter& writer, StringBuffer& buffer) const {
    writer.write(Type);
    writer.write(ID);
}

void GUIFontFilter::write_mhgu(BinaryWriter& writer, StringBuffer& buffer) const {
    writer.write(Type);
    writer.write(ID);
}

std::string GUIFontFilter::get_preview() const {
    return fmt::format("FontFilter<<C FFA3D7B8>{}</C>>: <C FFB0C94E>{}</C>", ID, enum_to_string(Type));
}

void GUIFontFilterDistanceField::write(BinaryWriter& writer, StringBuffer& buffer) const {
    GUIFontFilter::write(writer, buffer);

	writer.write(ParamNum);

    for (const auto& Param : Params) {
        writer.write(Param);
    }

    writer.write(Offset);
    writer.write(OffsetAngle);
    writer.write(Blending);
	writer.write<u16>(0);
	writer.write<u8>(0);
}

void GUIFontFilterDistanceField::write_mhgu(BinaryWriter& writer, StringBuffer& buffer) const {
    GUIFontFilter::write_mhgu(writer, buffer);

	writer.write(ParamNum);

    for (const auto& Param : Params) {
        writer.write(Param);
    }

    writer.write(Offset);
    writer.write(OffsetAngle);
    writer.write(Blending);
    writer.write<u16>(0);
    writer.write<u8>(0);
}

void GUIFontFilterGradationOverlay::write(BinaryWriter& writer, StringBuffer& buffer) const {
    GUIFontFilter::write(writer, buffer);

    writer.write(TextureIndex);
    writer.write(buffer.append_no_duplicate(Name));
}

void GUIFontFilterGradationOverlay::write_mhgu(BinaryWriter& writer, StringBuffer& buffer) const {
    GUIFontFilter::write_mhgu(writer, buffer);

    writer.write(TextureIndex);
    writer.write((u32)buffer.append_no_duplicate(Name));
}

void GUIFontFilterBorder::write(BinaryWriter& writer, StringBuffer& buffer) const {
    GUIFontFilter::write(writer, buffer);

    writer.write(BorderType);
    writer.write(Distance);
    writer.write(Color);
}

void GUIFontFilterBorder::write_mhgu(BinaryWriter& writer, StringBuffer& buffer) const{
    GUIFontFilter::write_mhgu(writer, buffer);

    writer.write(BorderType);
    writer.write(Distance);
    writer.write(Color);
}

void GUIFontFilterShadow::write(BinaryWriter& writer, StringBuffer& buffer) const {
    GUIFontFilter::write(writer, buffer);

    writer.write(Distance);
    writer.write(Rotation);
    writer.write(Color);
}

void GUIFontFilterShadow::write_mhgu(BinaryWriter& writer, StringBuffer& buffer) const {
    GUIFontFilter::write_mhgu(writer, buffer);

    writer.write(Distance);
    writer.write(Rotation);
    writer.write(Color);
}
