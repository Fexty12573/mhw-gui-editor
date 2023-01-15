#pragma once

#include <fstream>
#include <string>
#include <span>

class BinaryReader {
public:
	explicit BinaryReader(const std::string& path);

	// ----------------------------------
	// Methods for reading data
	// ----------------------------------

	template<typename T>
	[[nodiscard]] T read() {
		T value;
		m_file.read(reinterpret_cast<char*>(&value), sizeof(T));
		return value;
	}

	template<typename T>
	[[nodiscard]] T read_skip(std::streamoff skip_after) {
		T v = read<T>();
		seek_relative(skip_after);
		return v;
	} 

	[[nodiscard]] std::string read_string(std::streamsize length);
	[[nodiscard]] std::string read_string();
	void read_bytes(std::span<char> buffer);

	// ----------------------------------
	// Methods for reading at an offset
	// ----------------------------------

	template<class T> [[nodiscard]] T abs_offset_read(std::streamoff offset, bool restore_position = true) {
		const auto pos = tell();
		seek_absolute(offset);

		T v = read<T>();

		if (restore_position) {
			seek_absolute(pos);
		}

		return v;
	}
	template<class T> [[nodiscard]] T rel_offset_read(std::streamoff offset, bool restore_position = true) {
		const auto pos = tell();
		seek_relative(offset);

		T v = read<T>();

		if (restore_position) {
			seek_absolute(pos);
		}

		return v;
	}

	[[nodiscard]] std::string abs_offset_read_string(std::streamsize length, std::streamoff offset, bool restore_position = true);
	[[nodiscard]] std::string abs_offset_read_string(std::streamoff offset, bool restore_position = true);
	[[nodiscard]] std::string rel_offset_read_string(std::streamsize length, std::streamoff offset, bool restore_position = true);
	[[nodiscard]] std::string rel_offset_read_string(std::streamoff offset, bool restore_position = true);

	// ----------------------------------
	// General methods
	// ----------------------------------

	void seek_absolute(std::streamoff offset);
	void seek_relative(std::streamoff offset);

	[[nodiscard]] std::streampos tell();
	[[nodiscard]] size_t size() const;

private:
	std::ifstream m_file;
	size_t m_size;
};

