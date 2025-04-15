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
    void write(T v) {
        m_file.write(reinterpret_cast<const char*>(&v), sizeof(T));
    }

    template<typename T>
    void write_n(T v, size_t n) {
        for (size_t i = 0; i < n; ++i) {
            write<T>(v);
        }
    }

    template<typename ...Args>
    void write_tuple(const std::tuple<Args...>& v) {
        std::apply([this](const auto&... args) { (write(args), ...); }, v);
    }

    void write(const std::string& v, bool nullterminator = true);
    void write(const char* str, size_t size);

    template<typename T>
    void write(std::span<const T> data) {
        m_file.write(reinterpret_cast<const char*>(data.data()), data.size_bytes());
    }

    void pad_to(s32 bytes) {
        const auto pos = tell();
        if (bytes <= 0 || pos == End || pos % bytes == 0) {
            return;
        }

        const auto padding = bytes - (pos % bytes);
        if (padding > 0) {
            write_n<u8>(0, padding);
        }
    }

    void seek_absolute(std::streamoff offset);
    void seek_relative(std::streamoff offset);
    [[nodiscard]] std::streampos tell();
    [[nodiscard]] u32 tell32();
    [[nodiscard]] size_t size() noexcept;

private:
    std::ofstream m_file;
    
};

