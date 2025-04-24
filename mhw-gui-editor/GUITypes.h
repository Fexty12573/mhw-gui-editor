#pragma once
#include "dti_types.h"

#include <array>
#include <map>
#include <numeric>
#include <span>
#include <type_traits>
#include <vector>

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

struct GUIHeaderMHGU {
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
    u32 startFlowIndex;
    u32 animationOffset;
    u32 sequenceOffset;
    u32 objectOffset;
    u32 objSequenceOffset;
    u32 initParamOffset;
    u32 paramOffset;
    u32 instanceOffset;
    u32 flowOffset;
    u32 flowProcessOffset;
    u32 flowInputOffset;
    u32 flowSwitchOffset;
    u32 flowFunctionOffset;
    u32 actionOffset;
    u32 inputConditionOffset;
    u32 switchOperatorOffset;
    u32 switchConditionOffset;
    u32 variableOffset;
    u32 textureOffset;
    u32 fontOffset;
    u32 fontFilterOffset;
    u32 messageOffset;
    u32 guiResourceOffset;
    u32 generalResourceOffset;
    u32 cameraSettingOffset;
    u32 stringOffset;
    u32 keyOffset;
    u32 keyValue8Offset;
    u32 keyValue32Offset;
    u32 keyValue128Offset;
    u32 extendDataOffset;
    u32 instExeParamOffset;
    u32 vertexOffset;
};

struct KeyValueBuffers {
    std::vector<u8> KeyValue8;
    std::vector<u32> KeyValue32;
    std::vector<vector4> KeyValue128;
    std::vector<u8> ExtendData;

    struct {
        bool MultipleKV8Refs = false;
        bool MultipleKV32Refs = false;
        bool MultipleKV128Refs = false;
    } Config;

    size_t insert8(u8 value) {
        if (Config.MultipleKV8Refs) {
            const auto it = std::ranges::find(KeyValue8, value);

            if (it != KeyValue8.end()) {
                return std::distance(KeyValue8.begin(), it) * sizeof(u8);
            }
        }

        const auto idx = KeyValue8.size();
        KeyValue8.push_back(value);

        return idx * sizeof(u8);
    }

    size_t insert8(std::span<const u8> values) {
        if (Config.MultipleKV8Refs) {
            const auto it = std::ranges::search(KeyValue8, values).begin();

            if (it != KeyValue8.end()) {
                return std::distance(KeyValue8.begin(), it) * sizeof(u8);
            }
        }

        const auto idx = KeyValue8.size();
        KeyValue8.insert_range(KeyValue8.end(), values);

        return idx * sizeof(u8);
    }

    size_t insert32(u32 value) {
        if (Config.MultipleKV32Refs) {
            const auto it = std::ranges::find(KeyValue32, value);

            if (it != KeyValue32.end()) {
                return std::distance(KeyValue32.begin(), it) * sizeof(u32);
            }
        }

        const auto idx = KeyValue32.size();
        KeyValue32.push_back(value);

        return idx * sizeof(u32);
    }

    size_t insert32(float value) {
        return insert32(*reinterpret_cast<u32*>(&value));
    }

    template<typename T> size_t insert32(std::span<const T> values) requires std::is_convertible_v<T, u32> {
        if (Config.MultipleKV8Refs) {
            auto it = KeyValue32.begin();

            if constexpr (std::is_same_v<u32, T>) {
                it = std::ranges::search(KeyValue32, values).begin();
            } else {
                it = std::ranges::search(KeyValue32, values, [](u32 x, float y) {return x == *reinterpret_cast<u32*>(&y); }).begin();
            }

            if (it != KeyValue32.end()) {
                return std::distance(KeyValue32.begin(), it) * sizeof(u32);
            }
        }

        const auto idx = KeyValue32.size();
        for (const auto& v : values) {
            if constexpr (std::is_same_v<u32, T>) {
                KeyValue32.push_back(v);
            } else {
                KeyValue32.push_back(*reinterpret_cast<const u32*>(&v));
            }
        }

        return idx * sizeof(u32);
    }

    size_t insert64(u64 value) {
        union {
            u64 v64{};
            u32 v32[2];
        } uvalue = { .v64 = value };

        if (Config.MultipleKV32Refs) {
            const auto it = std::ranges::search(KeyValue32, uvalue.v32).begin();

            if (it != KeyValue32.end()) {
                return std::distance(KeyValue32.begin(), it) * sizeof(u32);
            }
        }

        const auto idx = KeyValue32.size();
        KeyValue32.push_back(uvalue.v32[0]);
        KeyValue32.push_back(uvalue.v32[1]);

        return idx * sizeof(u32);
    }

    size_t insert64(std::span<u64> values) {
        union U6432 {
            u64 v64{};
            u32 v32[2];
        };

        std::vector<u32> uvalues{};
        for (u64 v : values) {
            U6432 u{ .v64 = v };
            uvalues.push_back(u.v32[0]);
            uvalues.push_back(u.v32[1]);
        }

        if (Config.MultipleKV32Refs) {
            const auto it = std::ranges::search(KeyValue32, uvalues).begin();

            if (it != KeyValue32.end()) {
                return std::distance(KeyValue32.begin(), it) * sizeof(u32);
            }
        }

        const auto idx = KeyValue32.size();
        KeyValue32.insert_range(KeyValue32.end(), uvalues);

        return idx * sizeof(u32);
    }

    size_t insert128(const vector4& value) {
        if (Config.MultipleKV8Refs) {
            const auto it = std::ranges::find(KeyValue128, value);

            if (it != KeyValue128.end()) {
                return std::distance(KeyValue128.begin(), it) * sizeof(vector4);
            }
        }

        const auto idx = KeyValue128.size();
        KeyValue128.push_back(value);

        return idx * sizeof(vector4);
    }

    size_t insert128(std::span<const vector4> values) {
        if (Config.MultipleKV8Refs) {
            const auto it = std::ranges::search(KeyValue128, values).begin();

            if (it != KeyValue128.end()) {
                return std::distance(KeyValue128.begin(), it) * sizeof(vector4);
            }
        }

        const auto idx = KeyValue128.size();
        KeyValue128.insert_range(KeyValue128.end(), values);

        return idx * sizeof(vector4);
    }
};

enum class KeyValueType {
    None,
    KV8,
    KV32,
    KV128,
    String,
    Extend
};

enum class ObjectType : u32
{
    None = 0xCCCCCCCC,
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

    // MHGU only
    cGUIObjTexWithParam = 0x413023E9,
    cGUIObjBlendTextureSample = 0x27A477BF,
    cGUIObjFreePolygon = 0x0AD8C6E9,
    cFestaGUIObjPolygon = 0x35929810,
    cFestaGUIObjTexture = 0x7050B470
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

enum class SamplerMode : u8
{
    WrapLinear = 0x0,
    ClampLinear = 0x1,
    WrapPoint = 0x2,
    ClampPoint = 0x3
};

enum class ControlPoint : u8
{
    TL = 0x0,
    TC = 0x1,
    TR = 0x2,
    UNK_3 = 0x3,
    CL = 0x4,
    CC = 0x5,
    CR = 0x6,
    UNK_7 = 0x7,
    BL = 0x8,
    BC = 0x9,
    BR = 0xA,
};

enum class Alignment : u8
{
    NONE = 0x0,
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

enum class ScalingType : u32 {
    NONE = 0x0,
    POSITION = 0x1,
    SIZE = 0x2,
    FULL = 0x3
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

enum class IconColorType : u32 {
    NOINFLUENCE = 0x0,
    ALPHA = 0x1,
    ALL = 0x2
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

enum class ParamType : u8 {
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
    GENERALRESOURCE = 0x13,
    INIT_INT32 = 0x14
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
    MOJI_ORANGE_SELECTED2 = 17,
    MOJI_ORANGE_SELECTED = 18,
    MOJI_ORANGE_DISABLE = 19,
    MOJI_GREEN_SELECTED = 20,
    MOJI_GREEN_DEFAULT = 21,
    MOJI_GREEN_SELECTED2 = 22,
    MOJI_GREEN_DISABLE = 23,
    MOJI_LIGHTBLUE_DEFAULT = 25,
    MOJI_LIGHTBLUE_SELECTED2 = 26,
    MOJI_LIGHTBLUE_SELECTED = 27,
    MOJI_LIGHTBLUE2_DEFAULT = 28,
    MOJI_LIGHTBLUE2_SELECTED2 = 29,
    MOJI_LIGHTBLUE2_SELECTED = 30,
    MOJI_LIGHTGREEN_DEFAULT = 31,
    MOJI_LIGHTGREEN_SELECTED = 32,
    MOJI_LIGHTGREEN_DISABLE = 33,
    MOJI_BROWN_DEFAULT = 34,
    MOJI_BROWN_SELECTED2 = 35,
    MOJI_BROWN_SELECTED = 36,
    MOJI_YELLOW2_DEFAULT = 37,
    MOJI_YELLOW2_SELECTED2 = 38,
    MOJI_ORENGE2_DEFAULT = 39,
    MOJI_ORENGE2_SELECTED2 = 40,
    MOJI_ORENGE2_DISABLE = 41,
    MOJI_ORENGE2_SELECTED = 42,
    MOJI_LIGHTBLUE_DISABLE = 43,
    MOJI_LIGHTBLUE2_DISABLE = 44,
    MOJI_BROWN_DISABLE = 46,
    MOJI_YELLOW2_SELECTED = 47,
    MOJI_YELLOW2_DISABLE = 48,
    MOJI_LIGHTYELLOW_DEFAULT = 49,
    MOJI_LIGHTYELLOW_SELECTED = 50,
    MOJI_LIGHTYELLOW_SELECTED2 = 51,
    MOJI_LIGHTYELLOW_DISABLE = 52,
    MOJI_SLGREEN_DEFAULT = 53,
    MOJI_SLGREEN_SELECTED = 54,
    MOJI_SLGREEN_SELECTED2 = 55,
    MOJI_SLGREEN_DISABLE = 56,
    MOJI_LIGHTGREEN_SELECTED2 = 57,
    MOJI_SLGREEN2_DEFAULT = 58,
    MOJI_SLGREEN2_SELECTED = 59,
    MOJI_SLGREEN2_SELECTED2 = 60,
    MOJI_SLGREEN2_DISABLE = 61,
    MOJI_LIGHTYELLOW2_DEFAULT = 62,
    MOJI_LIGHTYELLOW2_SELECTED = 63,
    MOJI_LIGHTYELLOW2_SELECTED2 = 64,
    MOJI_LIGHTYELLOW2_DISABLE = 65,
    MOJI_PURPLE_DEFAULT = 67,
    MOJI_PURPLE_DISABLE = 68,
    MOJI_PURPLE_SELECTED = 69,
    MOJI_PURPLE_SELECTED2 = 70,
    MOJI_RED2_DEFAULT = 71,
    MOJI_RED2_SELECTED = 72,
    MOJI_RED2_SELECTED2 = 73,
    MOJI_RED2_DISABLE = 74,
    MOJI_BLUE_DISABLE = 75,
    MOJI_BLUE_SELECTED2 = 76,
    MOJI_BLUE_SELECTED = 77,
    MOJI_BLUE_DEFAULT = 78,
    MOJI_PALEBLUE_DEFAULT = 79,
    MOJI_PALEBLUE_SELECTED = 80,
    MOJI_PALEBLUE_SELECTED2 = 81,
    MOJI_PALEBLUE_DISABLE = 82
};

constexpr const char* enum_to_string(KeyValueType v) {
    switch (v) {
    case KeyValueType::KV8: return "KV8";
    case KeyValueType::KV32: return "KV32";
    case KeyValueType::KV128: return "KV128";
    case KeyValueType::String: return "String";
    case KeyValueType::Extend: return "Extend";
    default: break;
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

constexpr const char* enum_to_string(SamplerMode v) {
    switch (v) {
    case SamplerMode::WrapLinear: return "WrapLinear";
    case SamplerMode::ClampLinear: return "ClampLinear";
    case SamplerMode::WrapPoint: return "WrapPoint";
    case SamplerMode::ClampPoint: return "ClampPoint";
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

constexpr const char* enum_to_string(ParamType v) {
    switch (v) {
    case ParamType::UNKNOWN: return "UNKNOWN";
    case ParamType::INT: return "INT";
    case ParamType::FLOAT: return "FLOAT";
    case ParamType::BOOL: return "BOOL";
    case ParamType::VECTOR: return "VECTOR";
    case ParamType::RESOURCE: return "RESOURCE";
    case ParamType::STRING: return "STRING";
    case ParamType::TEXTURE: return "TEXTURE";
    case ParamType::FONT: return "FONT";
    case ParamType::MESSAGE: return "MESSAGE";
    case ParamType::VARIABLE: return "VARIABLE";
    case ParamType::ANIMATION: return "ANIMATION";
    case ParamType::EVENT: return "EVENT";
    case ParamType::GUIRESOURCE: return "GUIRESOURCE";
    case ParamType::FONT_FILTER: return "FONT_FILTER";
    case ParamType::ANIMEVENT: return "ANIMEVENT";
    case ParamType::SEQUENCE: return "SEQUENCE";
    case ParamType::INIT_BOOL: return "INIT_BOOL";
    case ParamType::INIT_INT: return "INIT_INT";
    case ParamType::GENERALRESOURCE: return "GENERALRESOURCE";
    case ParamType::INIT_INT32: return "INIT_INT32";
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

inline constexpr std::array FlowTypeNames = {
    "START",
    "END_0",
    "PROCESS",
    "INPUT",
    "SWITCH",
    "FUNCTION",
};

inline constexpr std::array BlendStateNames = {
    "DEFAULT",
    "BlendAlpha",
    "BlendInvAlpha",
    "Add",
    "Mul",
    "AddAlpha",
    "AddInvAlpha",
    "BlendFactor",
    "BlendFactorAlpha",
    "Max",
    "AddColor",
    "BlendColor",
    "RevSubAlpha",
    "RevSubInvAlpha",
    "RevSubBlendAlpha",
    "RevSubColor",
    "RevSubBlendColor",
    "RevSub",
    "ColorBlendAlphaAdd",
    "AddRGB",
    "AddInvColor",
};

inline constexpr std::array SamplerStateNames = {
    "WrapLinear",
    "ClampLinear",
    "WrapPoint",
    "ClampPoint",
};

inline constexpr std::array AlignmentNames = {
    "N/A",
    "LT",
    "CT",
    "RT",
    "LC",
    "CC",
    "RC",
    "LB",
    "CB",
    "RB",
};

inline constexpr std::array ResolutionAdjustNames = {
    "FOLLOW",
    "NONE",
    "VARIABLE",
    "VARIABLE_STRETCH",
    "VARIABLE_SHRINK",
    "SMALLPROP",
    "SMALLPROP_STRETCH",
    "SMALLPROP_SHRINK",
    "BIGPROP",
    "BIGPROP_STRETCH",
    "BIGPROP_SHRINK",
};

inline constexpr std::array AutoWrapNames = {
    "NONE",
    "WIDTH",
    "POINT",
};

inline constexpr std::array ColorControlNames = {
    "SCALING",
    "ATTRIBUTE",
    "NONE",
};

inline constexpr std::array EndConditionTypeNames = {
    "FLOW_ANIMATION_END",
    "FRAME_COUNT",
    "INFINITE_",
    "CHANGE_VARIABLE",
    "ANIMATION_END",
};

inline constexpr std::array LetterHAlignNames = {
    "LEFT",
    "CENTER",
    "RIGHT",
};

inline constexpr std::array LetterVAlignNames = {
    "TOP",
    "CENTER",
    "BOTTOM",
    "BASELINE",
};

inline constexpr std::array DepthStateNames = {
    "FOLLOW",
    "DISABLE",
    "TEST",
    "WRITE",
    "TEST_WRITE",
};

inline constexpr std::array BillboardNames = {
    "NONE",
    "XYZ",
    "Y",
};

inline constexpr std::array DrawPassNames = {
    "FOLLOW",
    "SOLID",
    "SCREEN",
    "TRANSPARENT_",
    "NUM",
    "N/A",
    "N/A",
    "N/A",
    "N/A",
    "N/A",
    "N/A",
    "N/A",
    "N/A",
    "N/A",
    "N/A",
    "N/A",
    "USER_OFFSET",
};

inline constexpr std::array MaskTypeNames = {
    "NONE",
    "ON",
    "REVERSE",
    "ALPHA",
    "ON_ADD",
    "REVERSE_ADD",
    "ALPHA_ADD",
};

inline constexpr std::array TilingModeNames = {
    "TILING_NONE",
    "TILING_SCALE",
    "TILING_NO_SCALE",
    "TILING_NUM",
};

inline constexpr std::array KeyModeNames = {
    "CONSTANT_0",
    "OFFSET_0",
    "TRIGGER_0",
    "LINEAR_0",
    "OFFSET_F_0",
    "HERMITE_0",
    "EASEIN",
    "EASEOUT",
    "HERMITE2",
    "NUM_8",
    "SUMMARY",
    "DEFAULT_10",
};

inline constexpr std::array ParamTypeNames = {
    "UNKNOWN",
    "INT",
    "FLOAT",
    "BOOL",
    "VECTOR",
    "RESOURCE",
    "STRING",
    "TEXTURE",
    "FONT",
    "MESSAGE",
    "VARIABLE",
    "ANIMATION",
    "EVENT",
    "GUIRESOURCE",
    "FONT_FILTER",
    "ANIMEVENT",
    "SEQUENCE",
    "INIT_BOOL",
    "INIT_INT",
    "GENERALRESOURCE",
    "INIT_INT32",
};

inline constexpr std::array FontStyleNames = {
    "N/A",
    "MOJI_WHITE_DEFAULT",
    "MOJI_WHITE_SELECTED",
    "N/A",
    "MOJI_WHITE_SELECTED2",
    "MOJI_WHITE_DISABLE",
    "MOJI_WHITE_DEFAULT2",
    "MOJI_BLACK_DEFAULT",
    "MOJI_RED_DEFAULT",
    "MOJI_RED_SELECTED",
    "MOJI_RED_SELECTED2",
    "MOJI_RED_DISABLE",
    "MOJI_YELLOW_DEFAULT",
    "MOJI_YELLOW_SELECTED",
    "MOJI_YELLOW_SELECTED2",
    "MOJI_YELLOW_DISABLE",
    "MOJI_ORANGE_DEFAULT",
    "MOJI_ORANGE_SELECTED2",
    "MOJI_ORANGE_SELECTED",
    "MOJI_ORANGE_DISABLE",
    "MOJI_GREEN_SELECTED",
    "MOJI_GREEN_DEFAULT",
    "MOJI_GREEN_SELECTED2",
    "MOJI_GREEN_DISABLE",
    "N/A",
    "MOJI_LIGHTBLUE_DEFAULT",
    "MOJI_LIGHTBLUE_SELECTED2",
    "MOJI_LIGHTBLUE_SELECTED",
    "MOJI_LIGHTBLUE2_DEFAULT",
    "MOJI_LIGHTBLUE2_SELECTED2",
    "MOJI_LIGHTBLUE2_SELECTED",
    "MOJI_LIGHTGREEN_DEFAULT",
    "MOJI_LIGHTGREEN_SELECTED",
    "MOJI_LIGHTGREEN_DISABLE",
    "MOJI_BROWN_DEFAULT",
    "MOJI_BROWN_SELECTED2",
    "MOJI_BROWN_SELECTED",
    "MOJI_YELLOW2_DEFAULT",
    "MOJI_YELLOW2_SELECTED2",
    "MOJI_ORENGE2_DEFAULT",
    "MOJI_ORENGE2_SELECTED2",
    "MOJI_ORENGE2_DISABLE",
    "MOJI_ORENGE2_SELECTED",
    "MOJI_LIGHTBLUE_DISABLE",
    "MOJI_LIGHTBLUE2_DISABLE",
    "N/A",
    "MOJI_BROWN_DISABLE",
    "MOJI_YELLOW2_SELECTED",
    "MOJI_YELLOW2_DISABLE",
    "MOJI_LIGHTYELLOW_DEFAULT",
    "MOJI_LIGHTYELLOW_SELECTED",
    "MOJI_LIGHTYELLOW_SELECTED2",
    "MOJI_LIGHTYELLOW_DISABLE",
    "MOJI_SLGREEN_DEFAULT",
    "MOJI_SLGREEN_SELECTED",
    "MOJI_SLGREEN_SELECTED2",
    "MOJI_SLGREEN_DISABLE",
    "MOJI_LIGHTGREEN_SELECTED2",
    "MOJI_SLGREEN2_DEFAULT",
    "MOJI_SLGREEN2_SELECTED",
    "MOJI_SLGREEN2_SELECTED2",
    "MOJI_SLGREEN2_DISABLE",
    "MOJI_LIGHTYELLOW2_DEFAULT",
    "MOJI_LIGHTYELLOW2_SELECTED",
    "MOJI_LIGHTYELLOW2_SELECTED2",
    "MOJI_LIGHTYELLOW2_DISABLE",
    "N/A",
    "MOJI_PURPLE_DEFAULT",
    "MOJI_PURPLE_DISABLE",
    "MOJI_PURPLE_SELECTED",
    "MOJI_PURPLE_SELECTED2",
    "MOJI_RED2_DEFAULT",
    "MOJI_RED2_SELECTED",
    "MOJI_RED2_SELECTED2",
    "MOJI_RED2_DISABLE",
    "MOJI_BLUE_DISABLE",
    "MOJI_BLUE_SELECTED2",
    "MOJI_BLUE_SELECTED",
    "MOJI_BLUE_DEFAULT",
    "MOJI_PALEBLUE_DEFAULT",
    "MOJI_PALEBLUE_SELECTED",
    "MOJI_PALEBLUE_SELECTED2",
    "MOJI_PALEBLUE_DISABLE",
};

inline const std::map<ObjectType, const char*> ObjectTypeNames = {
    { ObjectType::None, "None" },
    { ObjectType::cGUIObjRoot, "cGUIObjRoot" },
    { ObjectType::cGUIObjColorAdjust, "cGUIObjColorAdjust" },
    { ObjectType::cGUIObj2D, "cGUIObj2D" },
    { ObjectType::cGUIObjScissorMask, "cGUIObjScissorMask" },
    { ObjectType::cGUIObjNull, "cGUIObjNull" },
    { ObjectType::cGUIObjChildAnimationRoot, "cGUIObjChildAnimationRoot" },
    { ObjectType::cGUIObjPolygon, "cGUIObjPolygon" },
    { ObjectType::cGUIObjTexture, "cGUIObjTexture" },
    { ObjectType::cGUIObjTextureSet, "cGUIObjTextureSet" },
    { ObjectType::cGUIObjMaterial, "cGUIObjMaterial" },
    { ObjectType::cGUIObjMessage, "cGUIObjMessage" },
    { ObjectType::cGUIObjText, "cGUIObjText" },
    { ObjectType::cGUIObjEffect, "cGUIObjEffect" },
    { ObjectType::cGUIObjBaseModel, "cGUIObjBaseModel" },
    { ObjectType::cGUIObjHitRect, "cGUIObjHitRect" },
    { ObjectType::cGUIObjModel, "cGUIObjModel" },
    { ObjectType::cGUIObjSizeAdjustMessage, "cGUIObjSizeAdjustMessage" },
    { ObjectType::cGUIInstAnimVariable, "cGUIInstAnimVariable" },
    { ObjectType::cGUIInstAnimControl, "cGUIInstAnimControl" },
    { ObjectType::cGUIInstRoot, "cGUIInstRoot" },
    { ObjectType::cGUIInstNull, "cGUIInstNull" },
    { ObjectType::cGUIInstScissorMask, "cGUIInstScissorMask" },
    { ObjectType::cGUIInstAnimation, "cGUIInstAnimation" },
    { ObjectType::cGUIInstGauge, "cGUIInstGauge" },
    { ObjectType::cGUIInstAutoAnimation, "cGUIInstAutoAnimation" },
    { ObjectType::cGUIInstInput, "cGUIInstInput" },
    { ObjectType::cGUIInstButtonList, "cGUIInstButtonList" },
    { ObjectType::cGUIInstScrollBar, "cGUIInstScrollBar" },
    { ObjectType::cGUIInstSlider, "cGUIInstSlider" },
    { ObjectType::cGUIInstText, "cGUIInstText" },
    { ObjectType::cGUIInstButtonGrid, "cGUIInstButtonGrid" },
    { ObjectType::cGUIInstButtonGridLink, "cGUIInstButtonGridLink" },
    { ObjectType::cGUIInstButtonTree, "cGUIInstButtonTree" },
    { ObjectType::cGUIInstChangeNumInput, "cGUIInstChangeNumInput" },
    { ObjectType::cGUIInstColorPallet, "cGUIInstColorPallet" },
    { ObjectType::cGUIInstCursor, "cGUIInstCursor" },
    { ObjectType::cGUIInstFreeCursor, "cGUIInstFreeCursor" },
    { ObjectType::cGUIInstItemGrid, "cGUIInstItemGrid" },
    { ObjectType::cGUIInstMessage, "cGUIInstMessage" },
    { ObjectType::cGUIInstMouseOverFilter, "cGUIInstMouseOverFilter" },
    { ObjectType::cGUIFontFilterShadow, "cGUIFontFilterShadow" },
    { ObjectType::cGUIFontFilterBorder, "cGUIFontFilterBorder" },
    { ObjectType::cGUIFontFilterShading, "cGUIFontFilterShading" },
    { ObjectType::cGUIFontFilterGradationOverlay, "cGUIFontFilterGradationOverlay" },
    { ObjectType::cGUIFontFilterTextureBlend, "cGUIFontFilterTextureBlend" },
    { ObjectType::cGUIFontFilterDistanceField, "cGUIFontFilterDistanceField" },
    { ObjectType::rEffectAsset, "rEffectAsset" },

    // MHGU Only
    { ObjectType::cGUIObjTexWithParam, "cGUIObjTexWithParam" },
    { ObjectType::cGUIObjBlendTextureSample, "cGUIObjBlendTextureSample" },
    { ObjectType::cGUIObjFreePolygon, "cGUIObjFreePolygon" },
    { ObjectType::cFestaGUIObjPolygon, "cFestaGUIObjPolygon" },
    { ObjectType::cFestaGUIObjTexture, "cFestaGUIObjTexture" },
};

inline const std::map<std::string, ObjectType> ObjectTypeNamesReversed = {
    { "None", ObjectType::None },
    { "cGUIObjRoot", ObjectType::cGUIObjRoot },
    { "cGUIObjColorAdjust", ObjectType::cGUIObjColorAdjust },
    { "cGUIObj2D", ObjectType::cGUIObj2D },
    { "cGUIObjScissorMask", ObjectType::cGUIObjScissorMask },
    { "cGUIObjNull", ObjectType::cGUIObjNull },
    { "cGUIObjChildAnimationRoot", ObjectType::cGUIObjChildAnimationRoot },
    { "cGUIObjPolygon", ObjectType::cGUIObjPolygon },
    { "cGUIObjTexture", ObjectType::cGUIObjTexture },
    { "cGUIObjTextureSet", ObjectType::cGUIObjTextureSet },
    { "cGUIObjMaterial", ObjectType::cGUIObjMaterial },
    { "cGUIObjMessage", ObjectType::cGUIObjMessage },
    { "cGUIObjText", ObjectType::cGUIObjText },
    { "cGUIObjEffect", ObjectType::cGUIObjEffect },
    { "cGUIObjBaseModel", ObjectType::cGUIObjBaseModel },
    { "cGUIObjHitRect", ObjectType::cGUIObjHitRect },
    { "cGUIObjModel", ObjectType::cGUIObjModel },
    { "cGUIObjSizeAdjustMessage", ObjectType::cGUIObjSizeAdjustMessage },
    { "cGUIInstAnimVariable", ObjectType::cGUIInstAnimVariable },
    { "cGUIInstAnimControl", ObjectType::cGUIInstAnimControl },
    { "cGUIInstRoot", ObjectType::cGUIInstRoot },
    { "cGUIInstNull", ObjectType::cGUIInstNull },
    { "cGUIInstScissorMask", ObjectType::cGUIInstScissorMask },
    { "cGUIInstAnimation", ObjectType::cGUIInstAnimation },
    { "cGUIInstGauge", ObjectType::cGUIInstGauge },
    { "cGUIInstAutoAnimation", ObjectType::cGUIInstAutoAnimation },
    { "cGUIInstInput", ObjectType::cGUIInstInput },
    { "cGUIInstButtonList", ObjectType::cGUIInstButtonList },
    { "cGUIInstScrollBar", ObjectType::cGUIInstScrollBar },
    { "cGUIInstSlider", ObjectType::cGUIInstSlider },
    { "cGUIInstText", ObjectType::cGUIInstText },
    { "cGUIInstButtonGrid", ObjectType::cGUIInstButtonGrid },
    { "cGUIInstButtonGridLink", ObjectType::cGUIInstButtonGridLink },
    { "cGUIInstButtonTree", ObjectType::cGUIInstButtonTree },
    { "cGUIInstChangeNumInput", ObjectType::cGUIInstChangeNumInput },
    { "cGUIInstColorPallet", ObjectType::cGUIInstColorPallet },
    { "cGUIInstCursor", ObjectType::cGUIInstCursor },
    { "cGUIInstFreeCursor", ObjectType::cGUIInstFreeCursor },
    { "cGUIInstItemGrid", ObjectType::cGUIInstItemGrid },
    { "cGUIInstMessage", ObjectType::cGUIInstMessage },
    { "cGUIInstMouseOverFilter", ObjectType::cGUIInstMouseOverFilter },
    { "cGUIFontFilterShadow", ObjectType::cGUIFontFilterShadow },
    { "cGUIFontFilterBorder", ObjectType::cGUIFontFilterBorder },
    { "cGUIFontFilterShading", ObjectType::cGUIFontFilterShading },
    { "cGUIFontFilterGradationOverlay", ObjectType::cGUIFontFilterGradationOverlay },
    { "cGUIFontFilterTextureBlend", ObjectType::cGUIFontFilterTextureBlend },
    { "cGUIFontFilterDistanceField", ObjectType::cGUIFontFilterDistanceField },
    { "rEffectAsset", ObjectType::rEffectAsset },

    // MHGU Only
    { "cGUIObjTexWithParam", ObjectType::cGUIObjTexWithParam },
    { "cGUIObjBlendTextureSample", ObjectType::cGUIObjBlendTextureSample },
    { "cGUIObjFreePolygon", ObjectType::cGUIObjFreePolygon },
    { "cFestaGUIObjPolygon", ObjectType::cFestaGUIObjPolygon },
    { "cFestaGUIObjTexture", ObjectType::cFestaGUIObjTexture },
};

constexpr const char* enum_to_string(ObjectType v) {  
   switch (v) {  
   case ObjectType::None: return "None";  
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
   case ObjectType::cGUIObjTexWithParam: return "cGUIObjTexWithParam";  
   case ObjectType::cGUIObjBlendTextureSample: return "cGUIObjBlendTextureSample";  
   case ObjectType::cGUIObjFreePolygon: return "cGUIObjFreePolygon";  
   case ObjectType::cFestaGUIObjPolygon: return "cFestaGUIObjPolygon";  
   case ObjectType::cFestaGUIObjTexture: return "cFestaGUIObjTexture";
   }

   return "INVALID";  
}
