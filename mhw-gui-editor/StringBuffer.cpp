#include "pch.h"
#include "StringBuffer.h"

#include <algorithm>

StringBuffer::StringBuffer(size_type initial_capacity) : m_size(0) {
    m_buffer.reserve(initial_capacity);
}

StringBuffer::size_type StringBuffer::append(std::string_view str) {
    m_buffer.insert(m_buffer.end(), str.begin(), str.end());
    m_buffer.push_back('\0');
    
    const auto pos = m_size;
    m_size += str.size() + 1; // +1 for the null terminator
    return pos;
}

StringBuffer::size_type StringBuffer::append(const char* str, size_type len) {
    m_buffer.insert(m_buffer.end(), str, str + len);

    const auto pos = m_size;
    m_size += len;
    return pos;
}

StringBuffer::size_type StringBuffer::append_no_duplicate(std::string_view str) {
    if (const auto it = find(str); it != end()) {
        return std::distance(begin(), it);
    }

    return append(str);
}

std::string_view StringBuffer::view() const {
    return { m_buffer.data(), m_size };
}

const char* StringBuffer::data() const {
    return m_buffer.data();
}

StringBuffer::size_type StringBuffer::size() const {
    return m_size;
}

bool StringBuffer::contains(std::string_view str) const {
    return find(str) != end();
}

StringBuffer::const_iterator StringBuffer::find(std::string_view str) const {
    const auto is_valid_match = [&](const auto& it) -> bool {
        const auto size = static_cast<const_iterator::difference_type>(str.size());
        return (it == begin() || *(it - 1) == '\0') && 
            (it + size == end() || *(it + size) == '\0');
    };

    auto it = std::search(begin(), end(), str.begin(), str.end());
    while (it != end() && !is_valid_match(it)) {
        it = std::search(it + 1, end(), str.begin(), str.end());
    }

    return it;
}

StringBuffer::iterator StringBuffer::find(std::string_view str) {
    const auto is_valid_match = [&](const auto& it) -> bool {
        const auto size = static_cast<iterator::difference_type>(str.size());
        return (it == begin() || *(it - 1) == '\0') &&
            (it + size == end() || *(it + size) == '\0');
    };

    auto it = std::search(begin(), end(), str.begin(), str.end());
    while (it != end() && !is_valid_match(it)) {
        it = std::search(it + 1, end(), str.begin(), str.end());
    }

    return it;
}
