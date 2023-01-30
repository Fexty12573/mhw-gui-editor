#pragma once

#include <cstdlib>
#include <cstdint>

#include <type_traits>

namespace detail {
namespace crc {

template <unsigned c, int k = 8> struct f : f<((c & 1) ? 0xedb88320 : 0) ^ (c >> 1), k - 1> {};
template <unsigned c> struct f<c, 0> {
    enum { value = c };
};

#define CRC_TABLE_GEN_A(x) CRC_TABLE_GEN_B(x) CRC_TABLE_GEN_B(x + 128)
#define CRC_TABLE_GEN_B(x) CRC_TABLE_GEN_C(x) CRC_TABLE_GEN_C(x + 64)
#define CRC_TABLE_GEN_C(x) CRC_TABLE_GEN_D(x) CRC_TABLE_GEN_D(x + 32)
#define CRC_TABLE_GEN_D(x) CRC_TABLE_GEN_E(x) CRC_TABLE_GEN_E(x + 16)
#define CRC_TABLE_GEN_E(x) CRC_TABLE_GEN_F(x) CRC_TABLE_GEN_F(x + 8)
#define CRC_TABLE_GEN_F(x) CRC_TABLE_GEN_G(x) CRC_TABLE_GEN_G(x + 4)
#define CRC_TABLE_GEN_G(x) CRC_TABLE_GEN_H(x) CRC_TABLE_GEN_H(x + 2)
#define CRC_TABLE_GEN_H(x) CRC_TABLE_GEN_I(x) CRC_TABLE_GEN_I(x + 1)
#define CRC_TABLE_GEN_I(x) f<x>::value,

constexpr unsigned crc_table[] = {CRC_TABLE_GEN_A(0)};

// Constexpr implementation and helpers
constexpr uint32_t crc32_impl(const char* p, size_t len, uint32_t crc) {
    return len ? crc32_impl(p + 1, len - 1, (crc >> 8) ^ crc_table[(crc & 0xFF) ^ *p]) : crc;
}

template <class T, typename = std::enable_if_t<sizeof(T) == 1>>
constexpr uint32_t crc32(const T* data, size_t length) {
    return crc32_impl(data, length, ~0);
}

}
}

namespace crc {

constexpr uint32_t crc32(const char* data, size_t length) {
    return detail::crc::crc32(data, length) & 0x7FFFFFFF;
}

namespace literals {

constexpr uint32_t operator""_crc(const char* s, size_t len) {
    return detail::crc::crc32(s, len) & 0x7FFFFFFF;
}

} // namespace literals

} // namespace crc

#undef CRC_TABLE_GEN_A
#undef CRC_TABLE_GEN_B
#undef CRC_TABLE_GEN_C
#undef CRC_TABLE_GEN_D
#undef CRC_TABLE_GEN_E
#undef CRC_TABLE_GEN_F
#undef CRC_TABLE_GEN_G
#undef CRC_TABLE_GEN_H
#undef CRC_TABLE_GEN_I
