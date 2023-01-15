#include "pch.h"
#include "BinaryReader.h"

BinaryReader::BinaryReader(const std::string& path) : m_file(path, std::ios::binary) {
	if (!m_file.is_open()) {
		throw std::runtime_error("Failed to open file: " + path);
	}

	m_file.seekg(0, std::ios::end);
	m_size = m_file.tellg();
	m_file.seekg(0, std::ios::beg);
}

std::string BinaryReader::read_string(std::streamsize length) {
	std::string result;
	result.resize(length + 1); // +1 for null terminator
	m_file.read(result.data(), length);
	return result;
}

std::string BinaryReader::read_string() {
	std::string result;
	std::getline(m_file, result, '\0');

	return result;
}

void BinaryReader::read_bytes(std::span<char> buffer) {
	m_file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
}

std::string BinaryReader::abs_offset_read_string(std::streamsize length, std::streamoff offset, bool restore_position) {
	const auto pos = tell();
	seek_absolute(offset);

	auto result = read_string(length);

	if (restore_position) {
		seek_absolute(pos);
	}

	return result;
}

std::string BinaryReader::abs_offset_read_string(std::streamoff offset, bool restore_position) {
	const auto pos = tell();
	seek_absolute(offset);

	auto result = read_string();

	if (restore_position) {
		seek_absolute(pos);
	}

	return result;
}

std::string BinaryReader::rel_offset_read_string(std::streamsize length, std::streamoff offset, bool restore_position) {
	const auto pos = tell();
	seek_relative(offset);

	auto result = read_string(length);

	if (restore_position) {
		seek_absolute(pos);
	}

	return result;
}

std::string BinaryReader::rel_offset_read_string(std::streamoff offset, bool restore_position) {
	const auto pos = tell();
	seek_relative(offset);

	auto result = read_string();

	if (restore_position) {
		seek_absolute(pos);
	}

	return result;
}

void BinaryReader::seek_absolute(std::streamoff offset) {
	m_file.seekg(offset, std::ios::beg);
}

void BinaryReader::seek_relative(std::streamoff offset) {
	m_file.seekg(offset, std::ios::cur);
}

std::streampos BinaryReader::tell() {
	return m_file.tellg();
}

size_t BinaryReader::size() const {
	return m_size;
}
