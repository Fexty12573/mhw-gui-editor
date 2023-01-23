#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <span>


class BinaryWriter {
    static constexpr std::streamoff End = -1;
    static constexpr std::streamoff Begin = 0;

public:
    explicit BinaryWriter(const std::string& path);
    explicit BinaryWriter(const std::filesystem::path& path);

    template<typename T>
    void write(const T& v) {
        m_file.write(reinterpret_cast<const char*>(&v), sizeof(T));
    }

    void write(const std::string& v, bool nullterminator = true);
    void write(const char* str, size_t size);
    void write(std::span<const uint8_t> data);

    void seek_absolute(std::streamoff offset);
    void seek_relative(std::streamoff offset);
    [[nodiscard]] std::streampos tell();
    [[nodiscard]] size_t size() noexcept;

private:
    std::ofstream m_file;
    
};

