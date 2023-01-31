#pragma once

#include <imgui.h>

#include "dti_types.h"

namespace ImGui {

bool HermiteCurve(std::string_view label, MtHermiteCurve* curve, const ImVec2& size = ImVec2(0, 0));

}
