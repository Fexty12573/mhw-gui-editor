#include "pch.h"
#include "GUIKey.h"

GUIKey GUIKey::read(BinaryReader& reader) {
	return {
		.Data = { .Full = reader.read<u32>() },
		.CurveOffset = reader.read_skip<u32>(8)
	};
}
