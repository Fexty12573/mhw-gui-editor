#pragma once

#include <vector>
#include <string_view>

class StringBuffer {
public:
    using size_type = std::vector<char>::size_type;
    using iterator = std::vector<char>::iterator;
    using const_iterator = std::vector<char>::const_iterator;
    using reverse_iterator = std::vector<char>::reverse_iterator;
    using const_reverse_iterator = std::vector<char>::const_reverse_iterator;
    using value_type = std::vector<char>::value_type;
    using reference = std::vector<char>::reference;
    using const_reference = std::vector<char>::const_reference;

public:
    explicit StringBuffer(size_type initial_capacity = 0);

    size_type append(std::string_view str);
    size_type append(const char* str, size_type len);

    // Only appends the string if it doesn't already exist in the buffer.
    size_type append_no_duplicate(std::string_view str);

    [[nodiscard]] std::string_view view() const;
    [[nodiscard]] const char* data() const;
    [[nodiscard]] size_type size() const;

    [[nodiscard]] bool contains(std::string_view str) const;
    [[nodiscard]] iterator find(std::string_view str);
    [[nodiscard]] const_iterator find(std::string_view str) const;

    [[nodiscard]] iterator begin() { return m_buffer.begin(); }
    [[nodiscard]] const_iterator begin() const { return m_buffer.begin(); }
    [[nodiscard]] iterator end() { return m_buffer.end(); }
    [[nodiscard]] const_iterator end() const { return m_buffer.end(); }
    [[nodiscard]] reverse_iterator rbegin() { return m_buffer.rbegin(); }
    [[nodiscard]] const_reverse_iterator rbegin() const { return m_buffer.rbegin(); }
    [[nodiscard]] reverse_iterator rend() { return m_buffer.rend(); }
    [[nodiscard]] const_reverse_iterator rend() const { return m_buffer.rend(); }

    reference operator[](size_type pos) { return m_buffer[pos]; }
    const_reference operator[](size_type pos) const { return m_buffer[pos]; }

private:
    std::vector<char> m_buffer;
    size_type m_size;
};
