#pragma once
#include "dti_types.h"

#include <string>

struct GUIHeader {
    char fileType[4];
    u32 guiVersion;
    u32 fileSize;
    u32 attr;
    time_t revisionDate;
    u32 instanceID;
    u32 flowID;
    u32 variableID;
    u32 startInstanceIndex;
    u32 animationNum;
    u32 sequenceNum;
    u32 objectNum;
    u32 objSequenceNum;
    u32 initParamNum;
    u32 paramNum;
    u32 keyNum;
    u32 instanceNum;
    u32 flowNum;
    u32 flowProcessNum;
    u32 flowInputNum; // Always 0
    u32 flowSwitchNum; // Always 0
    u32 flowFunctionNum; // Always 0
    u32 actionNum; // Always 0
    u32 inputConditionNum; // Always 0
    u32 switchConditionNum; // Always 0
    u32 switchOperatorNum; // Always 0
    u32 variableNum; // Always 0
    u32 textureNum;
    u32 fontNum; // Always 0
    u32 fontFilterNum;
    u32 messageNum; // Always 0
    u32 guiResourceNum;
    u32 generalResourceNum;
    u32 cameraSettingNum; // Always 0
    u32 instExeParamNum;
    u32 vertexBufferSize;
    u32 optionBitFlag; // baseZ :2,framerateMode:1,languageSettingNo:2,padding:27
    u32 viewSize_Width;
    u32 viewSize_Height;
    u64 startFlowIndex;
    u64 animationOffset;
    u64 sequenceOffset;
    u64 objectOffset;
    u64 objSequenceOffset;
    u64 initParamOffset;
    u64 paramOffset;
    u64 instanceOffset;
    u64 flowOffset;
    u64 flowProcessOffset;
    u64 flowInputOffset;
    u64 flowSwitchOffset;
    u64 flowFunctionOffset;
    u64 actionOffset;
    u64 inputConditionOffset;
    u64 switchOperatorOffset;
    u64 switchConditionOffset;
    u64 variableOffset;
    u64 textureOffset;
    u64 fontOffset;
    u64 fontFilterOffset;
    u64 messageOffset;
    u64 guiResourceOffset;
    u64 generalResourceOffset;
    u64 cameraSettingOffset;
    u64 stringOffset;
    u64 keyOffset;
    u64 keyValue8Offset;
    u64 keyValue32Offset;
    u64 keyValue128Offset;
    u64 extendDataOffset;
    u64 instanceParamEntryStartIndex;//instExeParamOffset
    u64 vertexOffset;
    u64 padding;
};

enum class ObjectType : u32
{
    cGUIObjRoot = 473969240,
    cGUIObjColorAdjust = 122938906,
    cGUIObj2D = 351760238,
    cGUIObjScissorMask = 229195642,
    cGUIObjNull = 796866380,
    cGUIObjChildAnimationRoot = 663214884,
    cGUIObjPolygon = 1587625923,
    cGUIObjTexture = 459477923,
    cGUIObjTextureSet = 1332881660,
    cGUIObjMaterial = 1601273156,
    cGUIObjMessage = 800599726,
    cGUIObjText = 826240196,
    cGUIObjEffect = 1344422210,
    cGUIObjBaseModel = 2129076721,
    cGUIObjHitRect = 691640339,
    cGUIObjModel = 471103525,
    cGUIObjSizeAdjustMessage = 304564904,

    //inst hashes
    cGUIInstAnimVariable = 1916600852,
    cGUIInstAnimControl = 2084773514,
    cGUIInstRoot = 1923362099,
    cGUIInstNull = 1100687399,
    cGUIInstScissorMask = 1882551812,
    cGUIInstAnimation = 497711579,
    cGUIInstGauge = 981162009,
    cGUIInstAutoAnimation = 1252007991,
    cGUIInstInput = 1235707108,
    cGUIInstButtonList = 1786326969,
    cGUIInstScrollBar = 2133789891,
    cGUIInstSlider = 579080650,
    cGUIInstText = 1608217519,
    cGUIInstButtonGrid = 9506454,
    cGUIInstButtonGridLink = 546128602,
    cGUIInstButtonTree = 428837245,
    cGUIInstChangeNumInput = 1108543649,
    cGUIInstColorPallet = 1352762334,
    cGUIInstCursor = 675048259,
    cGUIInstFreeCursor = 524580423,
    cGUIInstItemGrid = 1827074792,
    cGUIInstMessage = 17244926,
    cGUIInstMouseOverFilter = 47863894,

    //font hashes
    cGUIFontFilterShadow = 1601192516,
    cGUIFontFilterBorder = 1844685598,
    cGUIFontFilterShading = 1026938010,
    cGUIFontFilterGradationOverlay = 1998668322,
    cGUIFontFilterTextureBlend = 1482942597,
    cGUIFontFilterDistanceField = 1670580032,

    //misc hashes
    rEffectAsset = 795748986,
};

enum class FlowType : u32
{
    START = 0x0,
    END_0 = 0x1,
    PROCESS = 0x2,
    INPUT = 0x3,
    SWITCH = 0x4,
    FUNCTION = 0x5
};

enum class BlendState : u8
{
    DEFAULT = 0x0,
    BlendAlpha = 0x1,
    BlendInvAlpha = 0x2,
    Add = 0x3,
    Mul = 0x4,
    AddAlpha = 0x5,
    AddInvAlpha = 0x6,
    BlendFactor = 0x7,
    BlendFactorAlpha = 0x8,
    Max = 0x9,
    AddColor = 0xA,
    BlendColor = 0xB,
    RevSubAlpha = 0xC,
    RevSubInvAlpha = 0xD,
    RevSubBlendAlpha = 0xE,
    RevSubColor = 0xF,
    RevSubBlendColor = 0x10,
    RevSub = 0x11,
    ColorBlendAlphaAdd = 0x12,
    AddRGB = 0x13,
    AddInvColor = 0x14
};

enum class SamplerState : u8
{
    WrapLinear = 0x0,
    ClampLinear = 0x1,
    WrapPoint = 0x2,
    ClampPoint = 0x3
};

enum class Alignment : u8
{
    LT = 0x1,
    CT = 0x2,
    RT = 0x3,
    LC = 0x4,
    CC = 0x5,
    RC = 0x6,
    LB = 0x7,
    CB = 0x8,
    RB = 0x9
};

enum class ResolutionAdjust : u8
{
    FOLLOW = 0x0,
    NONE = 0x1,
    VARIABLE = 0x2,
    VARIABLE_STRETCH = 0x3,
    VARIABLE_SHRINK = 0x4,
    SMALLPROP = 0x5,
    SMALLPROP_STRETCH = 0x6,
    SMALLPROP_SHRINK = 0x7,
    BIGPROP = 0x8,
    BIGPROP_STRETCH = 0x9,
    BIGPROP_SHRINK = 0xA
};

enum class AutoWrap : u8
{
    NONE = 0x0,
    WIDTH = 0x1,
    POINT = 0x2
};

enum class ColorControl : u8
{
    SCALING = 0x0,
    ATTRIBUTE = 0x1,
    NONE = 0x2
};

enum class EndConditionType : u32
{
    FLOW_ANIMATION_END = 0x0,
    FRAME_COUNT = 0x1,
    INFINITE_ = 0x2,
    CHANGE_VARIABLE = 0x3,
    ANIMATION_END = 0x4
};

enum class LetterHAlign : u8
{
    LEFT = 0x0,
    CENTER = 0x1,
    RIGHT = 0x2
};

enum class LetterVAlign : u8
{
    TOP = 0x0,
    CENTER = 0x1,
    BOTTOM = 0x2,
    BASELINE = 0x3
};

enum class DepthState : u8
{
    FOLLOW = 0x0,
    DISABLE = 0x1,
    TEST = 0x2,
    WRITE = 0x3,
    TEST_WRITE = 0x4
};

enum class Billboard : u8
{
    NONE = 0x0,
    XYZ = 0x1,
    Y = 0x2
};

enum class DrawPass : u8
{
    FOLLOW = 0x0,
    SOLID = 0x1,
    SCREEN = 0x2,
    TRANSPARENT_ = 0x3,
    NUM = 0x4,
    USER_OFFSET = 0x10,
};

enum class MaskType : u8
{
    NONE = 0x0,
    ON = 0x1,
    REVERSE = 0x2,
    ALPHA = 0x3,
    ON_ADD = 0x4,
    REVERSE_ADD = 0x5,
    ALPHA_ADD = 0x6
};

enum class TilingMode : u8
{
    TILING_NONE = 0x0,
    TILING_SCALE = 0x1,
    TILING_NO_SCALE = 0x2,
    TILING_NUM = 0x3,
};

enum class KeyMode : u8
{
    CONSTANT_0 = 0x0,
    OFFSET_0 = 0x1,
    TRIGGER_0 = 0x2,
    LINEAR_0 = 0x3,
    OFFSET_F_0 = 0x4,
    HERMITE_0 = 0x5,
    EASEIN = 0x6,
    EASEOUT = 0x7,
    HERMITE2 = 0x8,
    NUM_8 = 0x9,
    SUMMARY = 0xA,
    DEFAULT_10 = 0xB,
};

enum class ParamType : u32 {
    UNKNOWN = 0x0,
    INT = 0x1,
    FLOAT = 0x2,
    BOOL = 0x3,
    VECTOR = 0x4,
    RESOURCE = 0x5,
    STRING = 0x6,
    TEXTURE = 0x7,
    FONT = 0x8,
    MESSAGE = 0x9,
    VARIABLE = 0xA,
    ANIMATION = 0xB,
    EVENT = 0xC,
    GUIRESOURCE = 0xD,
    FONT_FILTER = 0xE,
    ANIMEVENT = 0xF,
    SEQUENCE = 0x10,
    INIT_BOOL = 0x11,
    INIT_INT = 0x12,
    GENERALRESOURCE = 0x13
};

enum class FontStyle : u32 {
    MOJI_WHITE_DEFAULT = 1,
    MOJI_WHITE_SELECTED = 2,
    MOJI_WHITE_SELECTED2 = 4,
    MOJI_WHITE_DISABLE = 5,
    MOJI_WHITE_DEFAULT2 = 6,
    MOJI_BLACK_DEFAULT = 7,
    MOJI_RED_DEFAULT = 8,
    MOJI_RED_SELECTED = 9,
    MOJI_RED_SELECTED2 = 10,
    MOJI_RED_DISABLE = 11,
    MOJI_YELLOW_DEFAULT = 12,
    MOJI_YELLOW_SELECTED = 13,
    MOJI_YELLOW_SELECTED2 = 14,
    MOJI_YELLOW_DISABLE = 15,
    MOJI_ORANGE_DEFAULT = 16,
    MOJI_ORANGE_SELECTED = 18,
    MOJI_ORANGE_SELECTED2 = 17,
    MOJI_ORANGE_DISABLE = 19,
    MOJI_GREEN_DEFAULT = 21,
    MOJI_GREEN_SELECTED = 20,
    MOJI_GREEN_SELECTED2 = 22,
    MOJI_GREEN_DISABLE = 23,
    MOJI_SLGREEN_DEFAULT = 53,
    MOJI_SLGREEN_SELECTED = 54,
    MOJI_SLGREEN_SELECTED2 = 55,
    MOJI_SLGREEN_DISABLE = 56,
    MOJI_SLGREEN2_DEFAULT = 58,
    MOJI_SLGREEN2_SELECTED = 59,
    MOJI_SLGREEN2_SELECTED2 = 60,
    MOJI_SLGREEN2_DISABLE = 61,
    MOJI_LIGHTBLUE_DEFAULT = 25,
    MOJI_LIGHTBLUE_SELECTED2 = 26,
    MOJI_LIGHTBLUE_SELECTED = 27,
    MOJI_LIGHTBLUE_DISABLE = 43,
    MOJI_LIGHTBLUE2_DEFAULT = 28,
    MOJI_LIGHTBLUE2_SELECTED2 = 29,
    MOJI_LIGHTBLUE2_SELECTED = 30,
    MOJI_LIGHTBLUE2_DISABLE = 44,
    MOJI_LIGHTGREEN_DEFAULT = 31,
    MOJI_LIGHTGREEN_SELECTED = 32,
    MOJI_LIGHTGREEN_SELECTED2 = 57,
    MOJI_LIGHTGREEN_DISABLE = 33,
    MOJI_LIGHTYELLOW_DEFAULT = 49,
    MOJI_LIGHTYELLOW_SELECTED = 50,
    MOJI_LIGHTYELLOW_SELECTED2 = 51,
    MOJI_LIGHTYELLOW_DISABLE = 52,
    MOJI_LIGHTYELLOW2_DEFAULT = 62,
    MOJI_LIGHTYELLOW2_SELECTED = 63,
    MOJI_LIGHTYELLOW2_SELECTED2 = 64,
    MOJI_LIGHTYELLOW2_DISABLE = 65,
    MOJI_BROWN_DEFAULT = 34,
    MOJI_BROWN_SELECTED2 = 35,
    MOJI_BROWN_SELECTED = 36,
    MOJI_BROWN_DISABLE = 46,
    MOJI_YELLOW2_DEFAULT = 37,
    MOJI_YELLOW2_SELECTED = 47,
    MOJI_YELLOW2_SELECTED2 = 38,
    MOJI_YELLOW2_DISABLE = 48,
    MOJI_ORENGE2_DEFAULT = 39,
    MOJI_ORENGE2_SELECTED = 42,
    MOJI_ORENGE2_SELECTED2 = 40,
    MOJI_ORENGE2_DISABLE = 41,
    MOJI_PURPLE_DEFAULT = 67,
    MOJI_PURPLE_SELECTED = 69,
    MOJI_PURPLE_SELECTED2 = 70,
    MOJI_PURPLE_DISABLE = 68,
    MOJI_RED2_DEFAULT = 71,
    MOJI_RED2_SELECTED = 72,
    MOJI_RED2_SELECTED2 = 73,
    MOJI_RED2_DISABLE = 74,
    MOJI_BLUE_DEFAULT = 78,
    MOJI_BLUE_SELECTED = 77,
    MOJI_BLUE_SELECTED2 = 76,
    MOJI_BLUE_DISABLE = 75,
    MOJI_PALEBLUE_DEFAULT = 79,
    MOJI_PALEBLUE_SELECTED = 80,
    MOJI_PALEBLUE_SELECTED2 = 81,
    MOJI_PALEBLUE_DISABLE = 82
};

constexpr const char* enum_to_string(ObjectType v) {
    switch (v) {
    case ObjectType::cGUIObjRoot: return "cGUIObjRoot";
    case ObjectType::cGUIObjColorAdjust: return "cGUIObjColorAdjust";
    case ObjectType::cGUIObj2D: return "cGUIObj2D";
    case ObjectType::cGUIObjScissorMask: return "cGUIObjScissorMask";
    case ObjectType::cGUIObjNull: return "cGUIObjNull";
    case ObjectType::cGUIObjChildAnimationRoot: return "cGUIObjChildAnimationRoot";
    case ObjectType::cGUIObjPolygon: return "cGUIObjPolygon";
    case ObjectType::cGUIObjTexture: return "cGUIObjTexture";
    case ObjectType::cGUIObjTextureSet: return "cGUIObjTextureSet";
    case ObjectType::cGUIObjMaterial: return "cGUIObjMaterial";
    case ObjectType::cGUIObjMessage: return "cGUIObjMessage";
    case ObjectType::cGUIObjText: return "cGUIObjText";
    case ObjectType::cGUIObjEffect: return "cGUIObjEffect";
    case ObjectType::cGUIObjBaseModel: return "cGUIObjBaseModel";
    case ObjectType::cGUIObjHitRect: return "cGUIObjHitRect";
    case ObjectType::cGUIObjModel: return "cGUIObjModel";
    case ObjectType::cGUIObjSizeAdjustMessage: return "cGUIObjSizeAdjustMessage";
    case ObjectType::cGUIInstAnimVariable: return "cGUIInstAnimVariable";
    case ObjectType::cGUIInstAnimControl: return "cGUIInstAnimControl";
    case ObjectType::cGUIInstRoot: return "cGUIInstRoot";
    case ObjectType::cGUIInstNull: return "cGUIInstNull";
    case ObjectType::cGUIInstScissorMask: return "cGUIInstScissorMask";
    case ObjectType::cGUIInstAnimation: return "cGUIInstAnimation";
    case ObjectType::cGUIInstGauge: return "cGUIInstGauge";
    case ObjectType::cGUIInstAutoAnimation: return "cGUIInstAutoAnimation";
    case ObjectType::cGUIInstInput: return "cGUIInstInput";
    case ObjectType::cGUIInstButtonList: return "cGUIInstButtonList";
    case ObjectType::cGUIInstScrollBar: return "cGUIInstScrollBar";
    case ObjectType::cGUIInstSlider: return "cGUIInstSlider";
    case ObjectType::cGUIInstText: return "cGUIInstText";
    case ObjectType::cGUIInstButtonGrid: return "cGUIInstButtonGrid";
    case ObjectType::cGUIInstButtonGridLink: return "cGUIInstButtonGridLink";
    case ObjectType::cGUIInstButtonTree: return "cGUIInstButtonTree";
    case ObjectType::cGUIInstChangeNumInput: return "cGUIInstChangeNumInput";
    case ObjectType::cGUIInstColorPallet: return "cGUIInstColorPallet";
    case ObjectType::cGUIInstCursor: return "cGUIInstCursor";
    case ObjectType::cGUIInstFreeCursor: return "cGUIInstFreeCursor";
    case ObjectType::cGUIInstItemGrid: return "cGUIInstItemGrid";
    case ObjectType::cGUIInstMessage: return "cGUIInstMessage";
    case ObjectType::cGUIInstMouseOverFilter: return "cGUIInstMouseOverFilter";
    case ObjectType::cGUIFontFilterShadow: return "cGUIFontFilterShadow";
    case ObjectType::cGUIFontFilterBorder: return "cGUIFontFilterBorder";
    case ObjectType::cGUIFontFilterShading: return "cGUIFontFilterShading";
    case ObjectType::cGUIFontFilterGradationOverlay: return "cGUIFontFilterGradationOverlay";
    case ObjectType::cGUIFontFilterTextureBlend: return "cGUIFontFilterTextureBlend";
    case ObjectType::cGUIFontFilterDistanceField: return "cGUIFontFilterDistanceField";
    case ObjectType::rEffectAsset: return "rEffectAsset";
    }

    return "INVALID";
}

constexpr const char* enum_to_string(FlowType v) {
    switch (v) {
    case FlowType::START: return "START";
    case FlowType::END_0: return "END_0";
    case FlowType::PROCESS: return "PROCESS";
    case FlowType::INPUT: return "INPUT";
    case FlowType::SWITCH: return "SWITCH";
    case FlowType::FUNCTION: return "FUNCTION";
    }

    return "INVALID";
}

constexpr const char* enum_to_string(BlendState v) {
    switch (v) {
    case BlendState::DEFAULT: return "DEFAULT";
    case BlendState::BlendAlpha: return "BlendAlpha";
    case BlendState::BlendInvAlpha: return "BlendInvAlpha";
    case BlendState::Add: return "Add";
    case BlendState::Mul: return "Mul";
    case BlendState::AddAlpha: return "AddAlpha";
    case BlendState::AddInvAlpha: return "AddInvAlpha";
    case BlendState::BlendFactor: return "BlendFactor";
    case BlendState::BlendFactorAlpha: return "BlendFactorAlpha";
    case BlendState::Max: return "Max";
    case BlendState::AddColor: return "AddColor";
    case BlendState::BlendColor: return "BlendColor";
    case BlendState::RevSubAlpha: return "RevSubAlpha";
    case BlendState::RevSubInvAlpha: return "RevSubInvAlpha";
    case BlendState::RevSubBlendAlpha: return "RevSubBlendAlpha";
    case BlendState::RevSubColor: return "RevSubColor";
    case BlendState::RevSubBlendColor: return "RevSubBlendColor";
    case BlendState::RevSub: return "RevSub";
    case BlendState::ColorBlendAlphaAdd: return "ColorBlendAlphaAdd";
    case BlendState::AddRGB: return "AddRGB";
    case BlendState::AddInvColor: return "AddInvColor";
    }

    return "INVALID";
}

constexpr const char* enum_to_string(SamplerState v) {
    switch (v) {
    case SamplerState::WrapLinear: return "WrapLinear";
    case SamplerState::ClampLinear: return "ClampLinear";
    case SamplerState::WrapPoint: return "WrapPoint";
    case SamplerState::ClampPoint: return "ClampPoint";
    }

    return "INVALID";
}

constexpr const char* enum_to_string(Alignment v) {
    switch (v) {
    case Alignment::LT: return "LT";
    case Alignment::CT: return "CT";
    case Alignment::RT: return "RT";
    case Alignment::LC: return "LC";
    case Alignment::CC: return "CC";
    case Alignment::RC: return "RC";
    case Alignment::LB: return "LB";
    case Alignment::CB: return "CB";
    case Alignment::RB: return "RB";
    }

    return "INVALID";
}

constexpr const char* enum_to_string(ResolutionAdjust v) {
    switch (v) {
    case ResolutionAdjust::FOLLOW: return "FOLLOW";
    case ResolutionAdjust::NONE: return "NONE";
    case ResolutionAdjust::VARIABLE: return "VARIABLE";
    case ResolutionAdjust::VARIABLE_STRETCH: return "VARIABLE_STRETCH";
    case ResolutionAdjust::VARIABLE_SHRINK: return "VARIABLE_SHRINK";
    case ResolutionAdjust::SMALLPROP: return "SMALLPROP";
    case ResolutionAdjust::SMALLPROP_STRETCH: return "SMALLPROP_STRETCH";
    case ResolutionAdjust::SMALLPROP_SHRINK: return "SMALLPROP_SHRINK";
    case ResolutionAdjust::BIGPROP: return "BIGPROP";
    case ResolutionAdjust::BIGPROP_STRETCH: return "BIGPROP_STRETCH";
    case ResolutionAdjust::BIGPROP_SHRINK: return "BIGPROP_SHRINK";
    }

    return "INVALID";
}

constexpr const char* enum_to_string(AutoWrap v) {
    switch (v) {
    case AutoWrap::NONE: return "NONE";
    case AutoWrap::WIDTH: return "WIDTH";
    case AutoWrap::POINT: return "POINT";
    }

    return "INVALID";
}

constexpr const char* enum_to_string(ColorControl v) {
    switch (v) {
    case ColorControl::SCALING: return "SCALING";
    case ColorControl::ATTRIBUTE: return "ATTRIBUTE";
    case ColorControl::NONE: return "NONE";
    }

    return "INVALID";
}

constexpr const char* enum_to_string(EndConditionType v) {
    switch (v) {
    case EndConditionType::FLOW_ANIMATION_END: return "FLOW_ANIMATION_END";
    case EndConditionType::FRAME_COUNT: return "FRAME_COUNT";
    case EndConditionType::INFINITE_: return "INFINITE_";
    case EndConditionType::CHANGE_VARIABLE: return "CHANGE_VARIABLE";
    case EndConditionType::ANIMATION_END: return "ANIMATION_END";
    }

    return "INVALID";
}

constexpr const char* enum_to_string(LetterHAlign v) {
    switch (v) {
    case LetterHAlign::LEFT: return "LEFT";
    case LetterHAlign::CENTER: return "CENTER";
    case LetterHAlign::RIGHT: return "RIGHT";
    }

    return "INVALID";
}

constexpr const char* enum_to_string(LetterVAlign v) {
    switch (v) {
    case LetterVAlign::TOP: return "TOP";
    case LetterVAlign::CENTER: return "CENTER";
    case LetterVAlign::BOTTOM: return "BOTTOM";
    case LetterVAlign::BASELINE: return "BASELINE";
    }

    return "INVALID";
}

constexpr const char* enum_to_string(DepthState v) {
    switch (v) {
    case DepthState::FOLLOW: return "FOLLOW";
    case DepthState::DISABLE: return "DISABLE";
    case DepthState::TEST: return "TEST";
    case DepthState::WRITE: return "WRITE";
    case DepthState::TEST_WRITE: return "TEST_WRITE";
    }

    return "INVALID";
}

constexpr const char* enum_to_string(Billboard v) {
    switch (v) {
    case Billboard::NONE: return "NONE";
    case Billboard::XYZ: return "XYZ";
    case Billboard::Y: return "Y";
    }

    return "INVALID";
}

constexpr const char* enum_to_string(DrawPass v) {
    switch (v) {
    case DrawPass::FOLLOW: return "FOLLOW";
    case DrawPass::SOLID: return "SOLID";
    case DrawPass::SCREEN: return "SCREEN";
    case DrawPass::TRANSPARENT_: return "TRANSPARENT_";
    case DrawPass::NUM: return "NUM";
    case DrawPass::USER_OFFSET: return "USER_OFFSET";
    }

    return "INVALID";
}

constexpr const char* enum_to_string(MaskType v) {
    switch (v) {
    case MaskType::NONE: return "NONE";
    case MaskType::ON: return "ON";
    case MaskType::REVERSE: return "REVERSE";
    case MaskType::ALPHA: return "ALPHA";
    case MaskType::ON_ADD: return "ON_ADD";
    case MaskType::REVERSE_ADD: return "REVERSE_ADD";
    case MaskType::ALPHA_ADD: return "ALPHA_ADD";
    }

    return "INVALID";
}

constexpr const char* enum_to_string(TilingMode v) {
    switch (v) {
    case TilingMode::TILING_NONE: return "TILING_NONE";
    case TilingMode::TILING_SCALE: return "TILING_SCALE";
    case TilingMode::TILING_NO_SCALE: return "TILING_NO_SCALE";
    case TilingMode::TILING_NUM: return "TILING_NUM";
    }

    return "INVALID";
}

constexpr const char* enum_to_string(KeyMode v) {
    switch (v) {
    case KeyMode::CONSTANT_0: return "CONSTANT_0";
    case KeyMode::OFFSET_0: return "OFFSET_0";
    case KeyMode::TRIGGER_0: return "TRIGGER_0";
    case KeyMode::LINEAR_0: return "LINEAR_0";
    case KeyMode::OFFSET_F_0: return "OFFSET_F_0";
    case KeyMode::HERMITE_0: return "HERMITE_0";
    case KeyMode::EASEIN: return "EASEIN";
    case KeyMode::EASEOUT: return "EASEOUT";
    case KeyMode::HERMITE2: return "HERMITE2";
    case KeyMode::NUM_8: return "NUM_8";
    case KeyMode::SUMMARY: return "SUMMARY";
    case KeyMode::DEFAULT_10: return "DEFAULT_10";
    }

    return "INVALID";
}

constexpr const char* enum_to_string(FontStyle v) {
    switch (v) {
    case FontStyle::MOJI_WHITE_DEFAULT: return "MOJI_WHITE_DEFAULT";
    case FontStyle::MOJI_WHITE_SELECTED: return "MOJI_WHITE_SELECTED";
    case FontStyle::MOJI_WHITE_SELECTED2: return "MOJI_WHITE_SELECTED2";
    case FontStyle::MOJI_WHITE_DISABLE: return "MOJI_WHITE_DISABLE";
    case FontStyle::MOJI_WHITE_DEFAULT2: return "MOJI_WHITE_DEFAULT2";
    case FontStyle::MOJI_BLACK_DEFAULT: return "MOJI_BLACK_DEFAULT";
    case FontStyle::MOJI_RED_DEFAULT: return "MOJI_RED_DEFAULT";
    case FontStyle::MOJI_RED_SELECTED: return "MOJI_RED_SELECTED";
    case FontStyle::MOJI_RED_SELECTED2: return "MOJI_RED_SELECTED2";
    case FontStyle::MOJI_RED_DISABLE: return "MOJI_RED_DISABLE";
    case FontStyle::MOJI_YELLOW_DEFAULT: return "MOJI_YELLOW_DEFAULT";
    case FontStyle::MOJI_YELLOW_SELECTED: return "MOJI_YELLOW_SELECTED";
    case FontStyle::MOJI_YELLOW_SELECTED2: return "MOJI_YELLOW_SELECTED2";
    case FontStyle::MOJI_YELLOW_DISABLE: return "MOJI_YELLOW_DISABLE";
    case FontStyle::MOJI_ORANGE_DEFAULT: return "MOJI_ORANGE_DEFAULT";
    case FontStyle::MOJI_ORANGE_SELECTED: return "MOJI_ORANGE_SELECTED";
    case FontStyle::MOJI_ORANGE_SELECTED2: return "MOJI_ORANGE_SELECTED2";
    case FontStyle::MOJI_ORANGE_DISABLE: return "MOJI_ORANGE_DISABLE";
    case FontStyle::MOJI_GREEN_DEFAULT: return "MOJI_GREEN_DEFAULT";
    case FontStyle::MOJI_GREEN_SELECTED: return "MOJI_GREEN_SELECTED";
    case FontStyle::MOJI_GREEN_SELECTED2: return "MOJI_GREEN_SELECTED2";
    case FontStyle::MOJI_GREEN_DISABLE: return "MOJI_GREEN_DISABLE";
    case FontStyle::MOJI_SLGREEN_DEFAULT: return "MOJI_SLGREEN_DEFAULT";
    case FontStyle::MOJI_SLGREEN_SELECTED: return "MOJI_SLGREEN_SELECTED";
    case FontStyle::MOJI_SLGREEN_SELECTED2: return "MOJI_SLGREEN_SELECTED2";
    case FontStyle::MOJI_SLGREEN_DISABLE: return "MOJI_SLGREEN_DISABLE";
    case FontStyle::MOJI_SLGREEN2_DEFAULT: return "MOJI_SLGREEN2_DEFAULT";
    case FontStyle::MOJI_SLGREEN2_SELECTED: return "MOJI_SLGREEN2_SELECTED";
    case FontStyle::MOJI_SLGREEN2_SELECTED2: return "MOJI_SLGREEN2_SELECTED2";
    case FontStyle::MOJI_SLGREEN2_DISABLE: return "MOJI_SLGREEN2_DISABLE";
    case FontStyle::MOJI_LIGHTBLUE_DEFAULT: return "MOJI_LIGHTBLUE_DEFAULT";
    case FontStyle::MOJI_LIGHTBLUE_SELECTED2: return "MOJI_LIGHTBLUE_SELECTED2";
    case FontStyle::MOJI_LIGHTBLUE_SELECTED: return "MOJI_LIGHTBLUE_SELECTED";
    case FontStyle::MOJI_LIGHTBLUE_DISABLE: return "MOJI_LIGHTBLUE_DISABLE";
    case FontStyle::MOJI_LIGHTBLUE2_DEFAULT: return "MOJI_LIGHTBLUE2_DEFAULT";
    case FontStyle::MOJI_LIGHTBLUE2_SELECTED2: return "MOJI_LIGHTBLUE2_SELECTED2";
    case FontStyle::MOJI_LIGHTBLUE2_SELECTED: return "MOJI_LIGHTBLUE2_SELECTED";
    case FontStyle::MOJI_LIGHTBLUE2_DISABLE: return "MOJI_LIGHTBLUE2_DISABLE";
    case FontStyle::MOJI_LIGHTGREEN_DEFAULT: return "MOJI_LIGHTGREEN_DEFAULT";
    case FontStyle::MOJI_LIGHTGREEN_SELECTED: return "MOJI_LIGHTGREEN_SELECTED";
    case FontStyle::MOJI_LIGHTGREEN_SELECTED2: return "MOJI_LIGHTGREEN_SELECTED2";
    case FontStyle::MOJI_LIGHTGREEN_DISABLE: return "MOJI_LIGHTGREEN_DISABLE";
    case FontStyle::MOJI_LIGHTYELLOW_DEFAULT: return "MOJI_LIGHTYELLOW_DEFAULT";
    case FontStyle::MOJI_LIGHTYELLOW_SELECTED: return "MOJI_LIGHTYELLOW_SELECTED";
    case FontStyle::MOJI_LIGHTYELLOW_SELECTED2: return "MOJI_LIGHTYELLOW_SELECTED2";
    case FontStyle::MOJI_LIGHTYELLOW_DISABLE: return "MOJI_LIGHTYELLOW_DISABLE";
    case FontStyle::MOJI_LIGHTYELLOW2_DEFAULT: return "MOJI_LIGHTYELLOW2_DEFAULT";
    case FontStyle::MOJI_LIGHTYELLOW2_SELECTED: return "MOJI_LIGHTYELLOW2_SELECTED";
    case FontStyle::MOJI_LIGHTYELLOW2_SELECTED2: return "MOJI_LIGHTYELLOW2_SELECTED2";
    case FontStyle::MOJI_LIGHTYELLOW2_DISABLE: return "MOJI_LIGHTYELLOW2_DISABLE";
    case FontStyle::MOJI_BROWN_DEFAULT: return "MOJI_BROWN_DEFAULT";
    case FontStyle::MOJI_BROWN_SELECTED2: return "MOJI_BROWN_SELECTED2";
    case FontStyle::MOJI_BROWN_SELECTED: return "MOJI_BROWN_SELECTED";
    case FontStyle::MOJI_BROWN_DISABLE: return "MOJI_BROWN_DISABLE";
    case FontStyle::MOJI_YELLOW2_DEFAULT: return "MOJI_YELLOW2_DEFAULT";
    case FontStyle::MOJI_YELLOW2_SELECTED: return "MOJI_YELLOW2_SELECTED";
    case FontStyle::MOJI_YELLOW2_SELECTED2: return "MOJI_YELLOW2_SELECTED2";
    case FontStyle::MOJI_YELLOW2_DISABLE: return "MOJI_YELLOW2_DISABLE";
    case FontStyle::MOJI_ORENGE2_DEFAULT: return "MOJI_ORENGE2_DEFAULT";
    case FontStyle::MOJI_ORENGE2_SELECTED: return "MOJI_ORENGE2_SELECTED";
    case FontStyle::MOJI_ORENGE2_SELECTED2: return "MOJI_ORENGE2_SELECTED2";
    case FontStyle::MOJI_ORENGE2_DISABLE: return "MOJI_ORENGE2_DISABLE";
    case FontStyle::MOJI_PURPLE_DEFAULT: return "MOJI_PURPLE_DEFAULT";
    case FontStyle::MOJI_PURPLE_SELECTED: return "MOJI_PURPLE_SELECTED";
    case FontStyle::MOJI_PURPLE_SELECTED2: return "MOJI_PURPLE_SELECTED2";
    case FontStyle::MOJI_PURPLE_DISABLE: return "MOJI_PURPLE_DISABLE";
    case FontStyle::MOJI_RED2_DEFAULT: return "MOJI_RED2_DEFAULT";
    case FontStyle::MOJI_RED2_SELECTED: return "MOJI_RED2_SELECTED";
    case FontStyle::MOJI_RED2_SELECTED2: return "MOJI_RED2_SELECTED2";
    case FontStyle::MOJI_RED2_DISABLE: return "MOJI_RED2_DISABLE";
    case FontStyle::MOJI_BLUE_DEFAULT: return "MOJI_BLUE_DEFAULT";
    case FontStyle::MOJI_BLUE_SELECTED: return "MOJI_BLUE_SELECTED";
    case FontStyle::MOJI_BLUE_SELECTED2: return "MOJI_BLUE_SELECTED2";
    case FontStyle::MOJI_BLUE_DISABLE: return "MOJI_BLUE_DISABLE";
    case FontStyle::MOJI_PALEBLUE_DEFAULT: return "MOJI_PALEBLUE_DEFAULT";
    case FontStyle::MOJI_PALEBLUE_SELECTED: return "MOJI_PALEBLUE_SELECTED";
    case FontStyle::MOJI_PALEBLUE_SELECTED2: return "MOJI_PALEBLUE_SELECTED2";
    case FontStyle::MOJI_PALEBLUE_DISABLE: return "MOJI_PALEBLUE_DISABLE";
    }

    return "INVALID";
}