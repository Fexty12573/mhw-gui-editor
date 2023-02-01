#pragma once

#include <string>

#define dti_size_assert(T, size) static_assert(sizeof(T) == size, "Size of " #T " is not " #size " bytes")
#define dti_offset_assert(T, member, offset) static_assert(offsetof(T, member) == offset, "Offset of " #T "::" #member " is not at offset " #offset)

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;
typedef float f32;
typedef double f64;

typedef u32 color; // 0xRRGGBBAA
typedef f32 matrix44[4][4]; // [R][C]
typedef f32 float2[2];
typedef f32 float3[3];
typedef f32 float4[4];
typedef char* cstring;
typedef char* string;
typedef f32 float3x3[3][3];
typedef f32 float4x3[4][3];
typedef f32 float3x4[3][4];
typedef f32 float4x4[4][4];
typedef f32 matrix33[3][3]; // [R][C]
typedef void* classref;
typedef u64 time_;

struct MtObject;
typedef void(*MT_MFUNC)(MtObject* pthis);
typedef void(*MT_MFUNC32)(MtObject* pthis, u32 param);
typedef void(*MT_MFUNC64)(MtObject* pthis, u64 param);
typedef void(*MT_MFUNC32X2)(MtObject* pthis, u32 param1, u32 param2);
typedef void(*MT_MFUNC64X2)(MtObject* pthis, u64 param1, u64 param2);
typedef void(*MT_MFUNCPTR)(MtObject* pthis, void* param);
typedef void(*MT_MFUNCPTRX2)(MtObject* pthis, void* param1, void* param2);
typedef void(*MT_MFUNCPTRU32)(MtObject* pthis, void* param1, u32 param2);

struct point
{
    u32 x, y;
};
struct size
{
    u32 x, y;
};
struct rect
{
    u32 x, y, w, h;
};
struct vector3
{
    f32 x, y, z;
    vector3& operator+=(const vector3& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }
    vector3& operator-=(const vector3& v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }
    vector3& operator*=(float v) {
        x *= v;
        y *= v;
        z *= v;
        return *this;
    }
    vector3& operator/=(float v) {
        x /= v;
        y /= v;
        z /= v;
        return *this;
    }
    vector3& operator+=(float v) {
        x += v;
        y += v;
        z += v;
        return *this;
    }
    vector3& operator-=(float v) {
        x -= v;
        y -= v;
        z -= v;
        return *this;
    }

    vector3 operator+(const vector3& v) const { return vector3(*this) += v; }
    vector3 operator-(const vector3& v) const { return vector3(*this) -= v; }
    vector3 operator+(float v) const { return vector3(*this) += v; }
    vector3 operator-(float v) const { return vector3(*this) -= v; }
    vector3 operator*(float v) const { return vector3(*this) *= v; }
    vector3 operator/(float v) const { return vector3(*this) /= v; }
    vector3 operator-() const { return { -x, -y, -z }; }
    float operator*(const vector3& v) const { return dot(v); }

    bool operator==(const vector3& v) const { return x == v.x && y == v.y && z == v.z; }
    bool operator!=(const vector3& v) const { return !((*this) == v); }

    float lensq() const { return x * x + y * y + z * z; }
    float len() const { return sqrtf(lensq()); }

    float dot(const vector3& v) const { return x * v.x + y * v.y + z * v.z; }
    vector3 cross(const vector3& v) const { return { y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x }; }

    vector3& normalize() { return (*this) /= len(); }
    vector3 normalized() const { return vector3(*this) /= len(); }

    vector3& set_length(float length) { return normalize() *= length; }
    vector3& limit(float length) { return lensq() > (length * length) ? set_length(length) : *this; }
    float* data() { return &x; }
};
struct vector4
{
    f32 x, y, z, w;
    vector4& operator+=(const vector4& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        w += v.w;
        return *this;
    }
    vector4& operator-=(const vector4& v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        w -= v.w;
        return *this;
    }
    vector4& operator*=(float v) {
        x *= v;
        y *= v;
        z *= v;
        w *= v;
        return *this;
    }
    vector4& operator/=(float v) {
        x /= v;
        y /= v;
        z /= v;
        w /= v;
        return *this;
    }
    vector4& operator+=(float v) {
        x += v;
        y += v;
        z += v;
        w += v;
        return *this;
    }
    vector4& operator-=(float v) {
        x -= v;
        y -= v;
        z -= v;
        w -= v;
        return *this;
    }

    vector4 operator+(const vector4& v) const { return vector4(*this) += v; }
    vector4 operator-(const vector4& v) const { return vector4(*this) -= v; }
    vector4 operator+(float v) const { return vector4(*this) += v; }
    vector4 operator-(float v) const { return vector4(*this) -= v; }
    vector4 operator*(float v) const { return vector4(*this) *= v; }
    vector4 operator/(float v) const { return vector4(*this) /= v; }
    vector4 operator-() const { return { -x, -y, -z, -w }; }
    float operator*(const vector4& v) const { return dot(v); }

    bool operator==(const vector4& v) const { return x == v.x && y == v.y && z == v.z && w == v.w; }
    bool operator!=(const vector4& v) const { return !(*this == v); }

    [[nodiscard]] float lensq() const { return x * x + y * y + z * z + w * w; }
    [[nodiscard]] float len() const { return sqrtf(lensq()); }

    [[nodiscard]] float dot(const vector4& v) const { return x * v.x + y * v.y + z * v.z + w * v.w; }

    vector4& normalize() { return (*this) /= len(); }
    [[nodiscard]] vector4 normalized() const { return vector4(*this) /= len(); }

    vector4& set_length(float length) { return normalize() *= length; }
    vector4& limit(float length) { return lensq() > (length * length) ? set_length(length) : *this; }
    [[nodiscard]] float* data() { return &x; }
};
struct quaternion
{
    union {
        f32 x, y, z, w;
        float4 values;
    };
};

struct easecurve
{
    union {
        float2 floats;
        f64 longfloat;
    };
};
struct line
{
    vector3 from, dir;
};
typedef line ray;

struct linesegment
{
    vector3 p0, p1;
};
typedef linesegment linesegment4[4];
struct Plane
{
    vector3 normal;
};
struct sphere
{
    float3 pos;
    f32 r;
};
struct capsule
{
    vector3 p0, p1;
    f32 r;
};
struct aabb
{
    alignas(16) vector3 minpos, maxpos;

	[[nodiscard]] vector3 center() const {
		return (minpos + maxpos) * 0.5f;
	}
};
typedef aabb aabb4[4];
struct obb
{
    matrix44 coord;
    vector3 extent;
};
struct cylinder
{
    vector3 p0;
    vector3 p1;
    f32 r;
};
struct triangle
{
    vector3 p0, p1, p2;
};
struct range
{
    s32 min, max;
};
struct rangef
{
    f32 min, max;
};
struct rangeu16
{
    u16 min, max;
};
struct hermitecurve
{
    float x[8];
    float y[8];

    [[nodiscard]] static constexpr int point_count() { return 8; }
    [[nodiscard]] int effective_point_count() const {
        for (int i = 0; i < point_count(); ++i) {
            if (x[i] == 1.0f) {
                return i + 1;
            }
        }

        return point_count();
    }
    [[nodiscard]] float get(float xx) const {
        if (xx <= x[0]) {
            return y[0];
        }

        if (xx >= 1.0f) {
            for (int i = 0; i < 8; ++i) {
                if (x[i] == 1.0f) {
                    return y[i];
                }
            }
        }
        
        int n = 0;
        for (int i = 1; i < 8; ++i) {
            if (x[i] > xx) {
                n = i - 1;
                break;
            }
        }

        const float dx = x[n + 1] - x[n];
        float dy;

        if (n == 0) {
            dy = y[n + 1] - y[n];
        } else {
            const float dx0 = x[n] - x[n - 1];
            const float dy0 = y[n] - y[n - 1];
            dy = y[n + 1] - ((1.0f - dx / dx0) * dy0 + y[n - 1]) * 0.5f;
        }
        
        float _dy;
        if (x[n + 1] == 1.0f || n > 5) {
            _dy = y[n + 1] - y[n];
        } else {
            const float dx1 = x[n + 2] - x[n + 1];
            const float dy1 = y[n + 2] - y[n + 1];
            _dy = ((dx / dx1) * dy1 + y[n + 1] - y[n]) * 0.5f;
        }

        const float ratio = (xx - x[n]) / dx;
        const float rsq = ratio * ratio;
        const float rcb = rsq * ratio;

        const float o2 = (2 * (y[n] - y[n + 1]) + dy + _dy) * rcb;
        const float o3 = (3 * (y[n + 1] - y[n]) - dy - dy - _dy) * rsq;

        const float result = o2 + o3 + ratio * dy + y[n];

        return std::max(std::min(result, 1.0f), 0.0f);
    }

    [[nodiscard]] float operator[](float xx) const {
        return get(xx);
    }
};
struct vector2
{
    f32 x, y;
};
typedef vector2 pointf;
typedef vector2 sizef;
struct rectf
{
    f32 x, y, w, h;
};
struct plane_xz
{
    vector3 normal;
};
struct pthread_mutex;
typedef pthread_mutex* pthread_mutex_t;


using MtVector2 = vector2;
using MtVector3 = vector3;
using MtVector4 = vector4;
using MtQuaternion = MtVector4;
using MtFloat3 = MtVector3;
using MtFloat4 = MtVector4;
using MtHermiteCurve = hermitecurve;

union MtColor
{
    u32 rgba;
    struct {
        u32 r : 8;
        u32 g : 8;
        u32 b : 8;
        u32 a : 8;
    };
};
struct MtMatrix
{
    MtVector4 m[4];
	[[nodiscard]] float* ptr() { return &m[0].x; }
	[[nodiscard]] const float* ptr() const { return &m[0].x; }
};
struct MtSphere
{
    MtFloat3 pos;
    f32 r;
};
struct MtAABB
{
    MtVector3 minpos;
    MtVector3 maxpos;
};
struct MtOBB
{
    MtMatrix coord;
    MtVector3 extent;
};
struct MtColorF
{
    f32 r, g, b, a;
};
struct MtEaseCurve
{
    f32 p1, p2;
};
struct MtCriticalSection
{
    pthread_mutex_t mCSection;
};
union MtPoint
{
    u64 xy;
    struct { u32 x, y; };
};
struct MtLineSegment
{
    MtVector3 p0, p1;
};
struct MtHalf4
{
    s64 x, y, z, w;
};
struct MtHalf2
{
    s64 x, y;
};
struct MtString
{
    s32 mRefCount;
    u32 mLength;
    char mString[1];
};


template<class T = void>
class MtDTI
{
public:
    cstring mName;
    MtDTI<T>* mpNext;
    MtDTI<T>* mpChild;
    MtDTI<T>* mpParent;
    MtDTI<T>* mpLink;

    union 
    {
        struct
        {
            u32 mSize : 23;
            u32 mAllocatorIndex : 6;
            u32 mAttr : 3;
        };
        u32 mFlags;
    };
    
    u32 mCRC;

    virtual ~MtDTI() = 0;
    virtual T* NewInstance() = 0;
    virtual T* CtorInstance(T* obj) = 0;
    virtual T* CtorInstanceArray(T* obj, s64 count) = 0;

    size_t GetSize() const
    {
        return static_cast<u64>(mSize) << 2;
    }
    bool InheritsFrom(const std::string& type) const
    {
	    for(auto dti = this->mpParent; dti != nullptr; dti = dti->mpParent)
	    {
            if (type == dti->mName)
                return true;
	    }

        return false;
    }
    bool InheritsFrom(u32 crc) const
    {
        for (auto dti = this->mpParent; dti != nullptr; dti = dti->mpParent)
        {
            if (crc == dti->mCRC)
                return true;
        }

        return false;
    }
    template<class X>
    bool InheritsFrom(const MtDTI<X>* other) const
    {
        for (auto dti = this->mpParent; dti != nullptr; dti = dti->mpParent)
        {
            if (other->mCRC == dti->mCRC)
                return true;
        }

        return false;
    }
};

using AnyDTI = MtDTI<void>;

enum class DTIPropType
{
    Undefined = 0x0,
    Class = 0x1,
    Classref = 0x2,
    Bool = 0x3,
    U8 = 0x4,
    U16 = 0x5,
    U32 = 0x6,
    U64 = 0x7,
    S8 = 0x8,
    S16 = 0x9,
    S32 = 0xA,
    S64 = 0xB,
    F32 = 0xC,
    F64 = 0xD,
    String = 0xE,
    Color = 0xF,
    Point = 0x10,
    Size = 0x11,
    Rect = 0x12,
    Matrix44 = 0x13,
    Vector3 = 0x14,
    Vector4 = 0x15,
    Quaternion = 0x16,
    Property = 0x17,
    Event = 0x18,
    Group = 0x19,
    PageBegin = 0x1A,
    PageEnd = 0x1B,
    Event32 = 0x1C,
    Array = 0x1D,
    PropertyList = 0x1E,
    GroupEnd = 0x1F,
    Cstring = 0x20,
    Time = 0x21,
    Float2 = 0x22,
    Float3 = 0x23,
    Float4 = 0x24,
    Float3x3 = 0x25,
    Float4x3 = 0x26,
    Float4x4 = 0x27,
    Easecurve = 0x28,
    Line = 0x29,
    Linesegment = 0x2A,
    Ray = 0x2B,
    Plane = 0x2C,
    Sphere = 0x2D,
    Capsule = 0x2E,
    AABB = 0x2F,
    OBB = 0x30,
    Cylinder = 0x31,
    Triangle = 0x32,
    Cone = 0x33,
    Torus = 0x34,
    Ellpsoid = 0x35,
    Range = 0x36,
    RangeF = 0x37,
    RangeU16 = 0x38,
    Hermitecurve = 0x39,
    EnumList = 0x3A,
    Float3x4 = 0x3B,
    Linesegment4 = 0x3C,
    AABB4 = 0x3D,
    Oscillator = 0x3E,
    Variable = 0x3F,
    Vector2 = 0x40,
    Matrix33 = 0x41,
    Rect3DXZ = 0x42,
    Rect3D = 0x43,
    Rect3D_Collision = 0x44,
    PlaneXZ = 0x45,
    RayY = 0x46,
    PointF = 0x47,
    SizeF = 0x48,
    RectF = 0x49,
    Event64 = 0x4A,
    Bool2 = 0x4B,
    END = 0x4C,
    Custom = 0x80
};
