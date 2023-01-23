#include "pch.h"
#include "BinaryWriter.h"

BinaryWriter::BinaryWriter(const std::string& path) : m_file(path, std::ios::binary | std::ios::trunc) {}

BinaryWriter::BinaryWriter(const std::filesystem::path& path) : m_file(path, std::ios::binary | std::ios::trunc) {}

void BinaryWriter::write(const std::string& v, bool nullterminator) {
    m_file.write(v.c_str(), v.size() + (nullterminator ? 1 : 0));
}

void BinaryWriter::write(const char* str, size_t size) {
    m_file.write(str, size);
}

void BinaryWriter::seek_absolute(std::streamoff offset) {
    if (offset < 0) {
        m_file.seekp(offset + 1, std::ios::end); // Seek from end
    } else {
        m_file.seekp(offset, std::ios::beg);
    }
}

void BinaryWriter::seek_relative(std::streamoff offset) {
    m_file.seekp(offset, std::ios::cur);
}

std::streampos BinaryWriter::tell() {
    return m_file.tellp();
}

size_t BinaryWriter::size() noexcept {
    const auto pos = tell();
    seek_absolute(End);
    const auto size = tell();
    seek_absolute(pos);
    return size;
}
